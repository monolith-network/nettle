#ifndef NET_TCP_SERVER_HPP
#define NET_TCP_SERVER_HPP

#include "HostPort.hpp"
#include "Socket.hpp"
#include "ConnectionHandler.hpp"
#include <string>

#include <atomic>
#include <mutex>
#include <thread>

//!
//! \file TcpServer.hpp
//! \brief A Tcp server that listens for connections and sends back the connection
//!        via a function callback
//!
namespace nettle
{
   //!
   //! \class TcpServer
   //! \brief A Tcp server used to listen for connections and callback on
   //!
   class TcpServer : protected Socket
   {
   public:
      //!
      //! \brief Construct a TcpServer
      //! \param hostPort The host and port information to run the tcp server
      //! \param connectionHandler The connection handler that handles in-bound sockets
      //! \param errorCb The error callback function - defaults to a cerr sink
      //! \param maxPendingRequests The maximum allowed pending requests
      //!
      TcpServer(HostPort hostPort,
                TcpConnectionHandler &connectionHandler,
                std::function<void(SocketError)> errorCb = ErrorSink,
                int maxPendingRequests = 10,
                int msSleepBetweenReq = 0);

      //!
      //! \brief Destructs a server
      //!
      ~TcpServer();

      //!
      //! \brief Start the server
      //!
      bool serve();

      //!
      //! \brief Stop the server
      //!
      bool stop();

   private:
      std::function<void(SocketError)> errorCb;
      TcpConnectionHandler &connectionHandler;
      HostPort hostPort;
      int maxConnectionThreads;
      int msSleepBetweenReq;
      bool ready;

      std::atomic<bool> threadRunning;
      std::mutex threadMut;
      std::thread serverThread;
   };
}

#endif