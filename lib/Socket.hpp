#ifndef NET_SOCKET_HPP
#define NET_SOCKET_HPP

#ifdef _MSC_VER
#include <winsock.h>
#include <ws2tcpip.h>

#define CLOSE_FD(fd) closesocket(fd)
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>

#define CLOSE_FD(fd) close(fd)
#endif

#include <string>
#include <functional>
#include <iostream>

//!
//! \file Sockets.hpp
//! \brief Header for abstracting sockets
//!
namespace nettle
{
   constexpr int SOCKET_RECV_TIMEOUT_SEC = 5;  //! Socket timeout sec for recv sockets
   constexpr int SOCKET_RECV_TIMEOUT_MS = 0;   //! Socket timeout ms for recv sockets
   constexpr int SOCKET_SEND_TIMEOUT_SEC = 10; //! Socket timeout sec for send sockets
   constexpr int SOCKET_SEND_TIMEOUT_MS = 0;   //! Socket timeout ms for send sockets

   //!
   //! \brief A socket error type
   //!
   enum class SocketError
   {
      SET_SOCK_OPT_RECV_TO,      //! Unable to set recv timeout
      SET_SOCK_OPT_SEND_TO,      //! Unable to set send timeout
      SOCKET_WRITE,              //! Error to write data
      ATTEMPT_INIT_SETUP_SOCKET, //! Trying to init an initialized socket
      SOCKET_CREATE,             //! Couldn't create a socket
      SOCKET_BIND,               //! Socket bind fail
      SOCKET_LISTEN,             //! Socket listen fail
      SOCKET_REUSEADDR,          //! Socket reuse fail
      WSAStartup,                //! Socket setup fail
      SOCKET_CONNECT             //! Unable to connect to remote
   };

   //!
   //! \brief A convenient socket error sink
   //!
   static void ErrorSink(nettle::SocketError err)
   {
      switch (err)
      {
      case SocketError::SET_SOCK_OPT_RECV_TO:
         std::cerr << "SET_SOCK_OPT_RECV_TO" << std::endl;
         break;
      case SocketError::SET_SOCK_OPT_SEND_TO:
         std::cerr << "SET_SOCK_OPT_SEND_TO" << std::endl;
         break;
      case SocketError::SOCKET_WRITE:
         std::cerr << "SOCKET_WRITE" << std::endl;
         break;
      case SocketError::ATTEMPT_INIT_SETUP_SOCKET:
         std::cerr << "ATTEMPT_INIT_SETUP_SOCKET" << std::endl;
         break;
      case SocketError::SOCKET_CREATE:
         std::cerr << "SOCKET_CREATE" << std::endl;
         break;
      case SocketError::SOCKET_BIND:
         std::cerr << "SOCKET_BIND" << std::endl;
         break;
      case SocketError::SOCKET_LISTEN:
         std::cerr << "SOCKET_LISTEN" << std::endl;
         break;
      case SocketError::WSAStartup:
         std::cerr << "WSAStartup" << std::endl;
         break;
      case SocketError::SOCKET_REUSEADDR:
         std::cerr << "SOCKET_REUSEADDR" << std::endl;
         break;
      default:
         break;
      }
   }

   //!
   //! \class Socket
   //! \brief Socket abstraction with read/write functionality
   //!
   class Socket
   {
   public:
      //!
      //! \brief Socket Constructor
      //!        Note: init method must be called before use
      //! \param errorCallback Function to call when an error occurs - Defaults to nettle::ErrorSink
      //!
      Socket(std::function<void(SocketError)> errorCallback = ErrorSink);

      //!
      //! \brief Socket Constructor - Construct and setup
      //! \param errorCallback Function to call when an error occurs
      //! \param socketFd Socket file desc
      //! \param sockAddr Socket address
      //!
      Socket(std::function<void(SocketError)> &errorCallback, int socketFd, sockaddr_in sockAddr);

      //!
      //! \brief Socket Destructor
      //!
      ~Socket();

      //!
      //! \brief socketWriteOut - Errors reported via errorCallback
      //! \param buffer The buffer to write out
      //! \param bufferLen Length of the given buffer
      //! \returns Number of bytes sent
      //!
      int socketWriteOut(const void *buffer, int bufferLen);

      //!
      //! \brief Socket read
      //! \param buffer The buffer to read to
      //! \param bufferLen Length of the given buffer
      //! \retval Total bytes received
      //!
      int socketReadIn(void *buffer, int bufferLen);

      //!
      //! \brief Setup a socket - Errors reported via errorCallback
      //! \param socketFd Socket file desc
      //! \param sockAddr Socket address
      //! \returns true iff socket is setup
      //!
      bool setupSocket(int socketFd, sockaddr_in sockAddr);

      //!
      //! \brief Close the socket
      //! \post Socket _COULD_ be re-setup
      //!
      void socketClose();

   protected:
      bool isInitd;
      int socketFd;
      sockaddr_in sockAddr;

      struct timeval recvTimeout;
      struct timeval sendTimeout;

      std::function<void(SocketError)> infoCb;
   };
}

#endif
