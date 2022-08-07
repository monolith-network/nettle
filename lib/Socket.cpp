#include "Socket.hpp"

#include <iostream>
#include <cstring>

namespace NETTLE
{
    // ---------------------------------------------------------------
    // socket
    // ---------------------------------------------------------------

    Socket::Socket(std::function<void(SocketError)> infoCb) : isInitd(false),
                                                                socketFd(-1),
                                                                infoCb(infoCb)
    {
        memset(&sockAddr, 0, sizeof(sockAddr));

        
        // Time value to time-out socksts
        recvTimeout.tv_sec  = SOCKET_RECV_TIMEOUT_SEC; 
        recvTimeout.tv_usec = SOCKET_RECV_TIMEOUT_MS;
        sendTimeout.tv_sec  = SOCKET_SEND_TIMEOUT_SEC; 
        sendTimeout.tv_usec = SOCKET_SEND_TIMEOUT_MS;
    }

    // ---------------------------------------------------------------
    // socket
    // ---------------------------------------------------------------

    Socket::Socket(std::function<void(SocketError)> & errorCallback,
                    int socketFd,
                    sockaddr_in sockAddr):
                                            isInitd(false),
                                            socketFd(-1),
                                            infoCb(errorCallback)

    {
        setupSocket(socketFd, sockAddr);
    }

    // ---------------------------------------------------------------
    // ~socket
    // ---------------------------------------------------------------

    Socket::~Socket()
    {
        if(isInitd)
        {
            CLOSE_FD(socketFd);
        }
    }

    // ---------------------------------------------------------------
    //init
    // ---------------------------------------------------------------

    void Socket::setupSocket(int socketFd, sockaddr_in sockAddr)
    {
        if(!this->isInitd)
        {
            this->socketFd = socketFd;
            this->sockAddr = sockAddr;

            // Set sock options
            if (setsockopt(this->socketFd, SOL_SOCKET, SO_RCVTIMEO, (char*)&recvTimeout, sizeof(recvTimeout)) < 0)
            {
                infoCb(SocketError::SET_SOCK_OPT_RECV_TO);
                return;
            }

            if (setsockopt(this->socketFd, SOL_SOCKET, SO_SNDTIMEO, (char*)&sendTimeout, sizeof(sendTimeout)) < 0)
            {
                infoCb(SocketError::SET_SOCK_OPT_SEND_TO);
                return;
            }

            int enable = 1;
            if (setsockopt(this->socketFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
            {
                infoCb(SocketError::SOCKET_REUSEADDR);
                return;
            }
            
            this->isInitd  = true;
        }
        else
        {
            infoCb(SocketError::ATTEMPT_INIT_SETUP_SOCKET);
            return;
        }
        return;
    }

    // ---------------------------------------------------------------
    // write
    // ---------------------------------------------------------------

    void Socket::socketWriteOut(const void* buffer, int bufferLen)
    {
        int sizeSent;
        size_t remaining  = bufferLen;
        const char *cBuff = static_cast<const char*>(buffer);

        while(remaining > 0)
        {
            do
            {
                sizeSent = send(socketFd,
                                cBuff,
                                remaining,
                                0);
            }
            while(sizeSent == -1 && EINTR == errno);

            if(sizeSent == -1)
            {
                infoCb(SocketError::SOCKET_WRITE);
            }

            cBuff     += sizeSent;
            remaining -= sizeSent;
        }
        return;
    }

    // ---------------------------------------------------------------
    // read
    // ---------------------------------------------------------------

    int Socket::socketReadIn(void * buffer, int bufferLen)
    {
        int recvSize;
        int totalRecv    =  0;
        size_t remaining =  bufferLen;
        char *cBuff = static_cast<char*>(buffer);

        while(remaining > 0)
        {
            do
            {
                recvSize = recv(socketFd,
                                cBuff,
                                remaining,
                                0);
            }
            while(recvSize == -1 && EINTR == errno);

            if(recvSize == -1)
            {
                return recvSize;
            }

            if(0 == recvSize)
            {
                break;
            }
            
            cBuff       += recvSize;
            totalRecv   += recvSize; 
            remaining   -= recvSize;
        }

        return totalRecv;
    }

    // ---------------------------------------------------------------
    // close
    // ---------------------------------------------------------------

    void Socket::socketClose()
    {
        if(this->isInitd)
        {
            CLOSE_FD(socketFd);
            this->isInitd = false;
        }
    }
}