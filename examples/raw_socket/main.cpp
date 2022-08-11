//
// Created by bosley on 3/31/20.
//

#include <nettle/HostPort.hpp>
#include <nettle/Socket.hpp>

#include <signal.h>
#include <iostream>
#include <string.h>

namespace {

    bool run_example;


    class MySocketObj : protected nettle::Socket{

    public:
        MySocketObj(nettle::HostPort hostAndPort) : Socket() {

            if ((this->socketFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
            {
                this->infoCb(nettle::SocketError::SOCKET_CREATE);
                return;
            }

            memset(&this->sockAddr, 0, sizeof(this->sockAddr));
            this->sockAddr.sin_family = AF_INET;
            this->sockAddr.sin_addr.s_addr = inet_addr(hostAndPort.getAddress().c_str());
            this->sockAddr.sin_port = htons(hostAndPort.getPort());

            if (::bind(this->socketFd, (sockaddr*)&this->sockAddr, sizeof(this->sockAddr)) < 0)
            {
                this->infoCb(nettle::SocketError::SOCKET_BIND);
                return;
            }

            // mark the socket so it will listen for incoming connections, with 10 Maximum pending connections
            if (::listen(this->socketFd, 10) < 0)
            {
                this->infoCb(nettle::SocketError::SOCKET_LISTEN);
                return;
            }

            this->setupSocket(this->socketFd, this->sockAddr);
        }

        void listenIn() {

            while(run_example) {

                int clientFd;
                sockaddr_in clientAddr;
                socklen_t addrLen = sizeof(clientAddr);

                if ((clientFd = accept(this->socketFd, (struct sockaddr*) &clientAddr,  &addrLen)) < 0)
                {
                    continue;
                }

                Socket clientSocket;
                clientSocket.setupSocket(clientFd, clientAddr);

                std::cout << "Got a connection, and created an object for them. Feel free to pass it somewhere to handle!" << std::endl;

                clientSocket.socketClose();
            }

            this->socketClose();
        }
    };

}

void signal_handler(int s){

    std::cout << "Signal handler got : " << s << std::endl;

    run_example = false;
}

int main() {

    signal(SIGINT, signal_handler);

    nettle::HostPort hp("127.0.0.1", 4097);

    MySocketObj mySocketObject(hp);

    run_example = true;

    std::cout << "Use [ netcat -t 127.0.0.1 4097 ] to connect to the example socket object" << std::endl;
    std::cout << "Use CTRL+C to stop example" << std::endl;

    mySocketObject.listenIn();

    return 0;
}