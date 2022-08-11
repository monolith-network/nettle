#ifndef NET_UDP_SERVER_HPP
#define NET_UDP_SERVER_HPP

#include "Socket.hpp"
#include "HostPort.hpp"
#include "ConnectionHandler.hpp"
#include <iostream>
#include <cstring>
#include <string>
#include <atomic>
#include <mutex>
#include <thread>

//!
//! \file UdpServerN.hpp
//! \brief A Udp server that sets up the UDP socket and once ready, will
//!        callback on the connectionHandler to handle the connection
//!
namespace nettle
{
   //!
   //! \class UdpServerN
   //! \brief A Udp server that sets up a socket and hands it back to a user to serve
   //!
   template <std::size_t N>
   class UdpServerN : protected Socket
   {
   public:
      //!
      //! \brief Construct a UdpServerN
      //! \param hostPort Address and port to listen on
      //! \param connectionHandler Connection handler class
      //! \param errorCb The error callback - Defaults to ErrorSink
      //!
      UdpServerN(HostPort hostPort,
                 UdpConnectionHandlerN<N> &connectionHandler,
                 std::function<void(SocketError)> errorCb = nettle::ErrorSink) : Socket(errorCb),
                                                                                 hostPort(hostPort),
                                                                                 connectionHandler(connectionHandler),
                                                                                 errorCb(errorCb),
                                                                                 threadRunning(false)
      {
      }

      //!
      //! \brief Destructs a server
      //!
      ~UdpServerN()
      {

         this->socketClose();
#if defined(_MSC_VER)
         WSACleanup();
#endif
      }

      //!
      //! \brief Start the server
      //!
      bool serve()
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
             [](UdpServerN *server)
             {

#ifdef _MSC_VER
                WSADATA ws_data;
                if (WSAStartup(MAKEWORD(2, 0), &ws_data) != 0)
                {
                   errorCb(SocketError::WSAStartup);
                   return;
                }
#endif
                if ((server->socketFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
                {
                   server->errorCb(SocketError::SOCKET_CREATE);
                   return;
                }

                memset(&server->sockAddr, 0, sizeof(server->sockAddr));
                server->sockAddr.sin_family = AF_INET;
                server->sockAddr.sin_addr.s_addr = inet_addr(server->hostPort.getAddress().c_str());
                server->sockAddr.sin_port = htons(server->hostPort.getPort());

                if (::bind(server->socketFd, (sockaddr *)&server->sockAddr, sizeof(server->sockAddr)) < 0)
                {
                   server->errorCb(SocketError::SOCKET_BIND);
                   return;
                }

                if ((server->socketFd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
                {
                   server->errorCb(SocketError::SOCKET_CREATE);
                   return;
                }

                memset(&server->sockAddr, 0, sizeof(server->sockAddr));
                server->sockAddr.sin_family = AF_INET;
                server->sockAddr.sin_addr.s_addr = inet_addr(server->hostPort.getAddress().c_str());
                server->sockAddr.sin_port = htons(server->hostPort.getPort());

                if (::bind(server->socketFd, (sockaddr *)&server->sockAddr, sizeof(server->sockAddr)) < 0)
                {
                   server->errorCb(SocketError::SOCKET_BIND);
                   return;
                }

                server->setupSocket(server->socketFd, server->sockAddr);

                while (server->threadRunning)
                {

                   std::this_thread::sleep_for(std::chrono::milliseconds(1));

                   uint8_t buffer[N];

                   int recvSize = server->socketReadIn(buffer, N);

                   if (recvSize > 0)
                   {
                      server->connectionHandler.newData(buffer);
                   }
                }

                server->socketClose();
             } // End func
             ,
             this);

         return true;
      }

      //!
      //! \brief Stop the server
      //!
      bool stop()
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

   private:
      HostPort hostPort;
      UdpConnectionHandlerN<N> &connectionHandler;
      std::function<void(SocketError)> errorCb;

      std::atomic<bool> threadRunning;
      std::mutex threadMut;
      std::thread serverThread;
   };
}

#endif