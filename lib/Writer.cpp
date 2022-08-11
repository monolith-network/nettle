#include "Writer.hpp"

#include <cstring>

namespace nettle
{
   // -------------------------------------------------------
   // StdWriter
   // -------------------------------------------------------

   Writer::Writer(HostPort connectionInfo, WriterType connectionType, std::function<void(SocketError)> errorCb) : Socket(errorCb), errorPresent(false)
   {

#ifdef _MSC_VER
      WSADATA ws_data;
      if (WSAStartup(MAKEWORD(2, 0), &ws_data) != 0)
      {
         errorCb(SocketError::WSAStartup);
         errorPresent = true;
         return;
      }
#endif

      struct sockaddr_in serverAddr; // server address
      if (connectionType == WriterType::TCP)
      {
         // create a stream socket using TCP
         if ((this->socketFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
         {
            errorCb(SocketError::SOCKET_CREATE);
            errorPresent = true;
            return;
         }
      }
      else
      {
         // create a stream socket using TCP
         if ((this->socketFd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
         {
            errorCb(SocketError::SOCKET_CREATE);
            errorPresent = true;
            return;
         }
      }

      memset(&serverAddr, 0, sizeof(serverAddr));
      serverAddr.sin_family = AF_INET;
      if (inet_pton(AF_INET, connectionInfo.getAddress().c_str(), &serverAddr.sin_addr) <= 0)
      {
         errorCb(SocketError::SOCKET_CREATE);
         errorPresent = true;
         return;
      }

      serverAddr.sin_port = htons(connectionInfo.getPort());

      if (connect(this->socketFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
      {
         errorCb(SocketError::SOCKET_CONNECT);
         errorPresent = true;
         return;
      }

      this->setupSocket(this->socketFd, serverAddr);
   }

   // -------------------------------------------------------
   // ~StdWriter
   // -------------------------------------------------------

   Writer::~Writer()
   {
      this->socketClose();
   }

   bool Writer::hasError() const
   {

      return errorPresent;
   }
}