#include "TcpServer.hpp"
#include <cstring>

namespace nettle
{
   // ---------------------------------------------------------
   // TcpServer
   // ---------------------------------------------------------
   TcpServer::TcpServer(HostPort hostPort,
                        TcpConnectionHandler &connectionHandler,
                        std::function<void(SocketError)> errorCb,
                        int maxPendingRequests,
                        int msSleepBetweenReq) : Socket(errorCb),
                                                 errorCb(errorCb),
                                                 connectionHandler(connectionHandler),
                                                 hostPort(hostPort),
                                                 maxConnectionThreads(maxConnectionThreads),
                                                 msSleepBetweenReq(msSleepBetweenReq),
                                                 ready(false),
                                                 threadRunning(false)

   {

#ifdef _MSC_VER
      WSADATA ws_data;
      if (WSAStartup(MAKEWORD(2, 0), &ws_data) != 0)
      {
         errorCb(SocketError::WSAStartup);
         return;
      }
#endif
      if ((this->socketFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
      {
         errorCb(SocketError::SOCKET_CREATE);
         return;
      }

      memset(&this->sockAddr, 0, sizeof(this->sockAddr));
      this->sockAddr.sin_family = AF_INET;
      this->sockAddr.sin_addr.s_addr = inet_addr(this->hostPort.getAddress().c_str());
      this->sockAddr.sin_port = htons(this->hostPort.getPort());

      if (::bind(this->socketFd, (sockaddr *)&this->sockAddr, sizeof(this->sockAddr)) < 0)
      {
         errorCb(SocketError::SOCKET_BIND);
         return;
      }

      // mark the socket so it will listen for incoming connections
      if (::listen(this->socketFd, maxPendingRequests) < 0)
      {
         errorCb(SocketError::SOCKET_LISTEN);
         return;
      }

      this->setupSocket(this->socketFd, this->sockAddr);
      ready = true;
   }

   // ---------------------------------------------------------
   // ~TcpServer
   // ---------------------------------------------------------
   TcpServer::~TcpServer()
   {
#if defined(_MSC_VER)
      WSACleanup();
#endif
      this->socketClose();
   }

   // ---------------------------------------------------------
   // startThreaded
   // ---------------------------------------------------------
   bool TcpServer::serve()
   {
      {
         std::lock_guard<std::mutex> lock(threadMut);

         if (threadRunning)
         {
            std::cerr << "Thread already started" << std::endl;
            return false;
         }
      }

      threadRunning = true;

      serverThread = std::thread(
          [](TcpServer *server)
          {
             server->connectionHandler.serverStarted();

             while (server->threadRunning)
             {

                if (!server->ready)
                {
                   std::cerr << "Server not ready" << std::endl;
                   return;
                }

                int clientFd;
                sockaddr_in clientAddr;
#ifdef _MSC_VER
                int addrLen;
#else
                socklen_t addrLen;
#endif
                addrLen = sizeof(clientAddr);

                if ((clientFd = accept(server->socketFd, (struct sockaddr *)&clientAddr, &addrLen)) < 0)
                {
                  std::this_thread::sleep_for(std::chrono::milliseconds(10));
                  continue;
                }

                Socket clientSocket(server->errorCb);
                
                if (clientSocket.setupSocket(clientFd, clientAddr) )
                {
                  server->connectionHandler.newConnection(clientSocket);
                }
                
                std::this_thread::sleep_for(std::chrono::milliseconds(server->msSleepBetweenReq));
             }
          } // End func
          ,
          this);

      return true;
   }

   // ---------------------------------------------------------
   // stopThreaded
   // ---------------------------------------------------------
   bool TcpServer::stop()
   {
      std::lock_guard<std::mutex> lock(threadMut);

      if (!threadRunning)
      {
         std::cerr << "Thread not running" << std::endl;
         return false;
      }

      threadRunning = false;

      connectionHandler.serverStopping();

      serverThread.join();

      connectionHandler.serverStopped();

      return true;
   }
}