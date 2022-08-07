//
// Created by bosley on 3/31/20.
//

#include <nettle/ConnectionHandler.hpp>
#include <nettle/UdpServerN.hpp>
#include <nettle/Writer.hpp>

#include <signal.h>
#include <iostream>

namespace {

    template <std::size_t N>
    class UdpConenctionHandler : public NETTLE::UdpConnectionHandlerN<N> {

    public:
        UdpConenctionHandler() {

        }

        void serverStarted() override  {
            std::cout << "Server started " << std::endl;
        }

        void serverStopping() override {
            std::cout << "Server is stopping " << std::endl;
        }

        void serverStopped() override  {
            std::cout << "Server is stopped " << std::endl;
        }

        void newData(uint8_t data[N]) override {

            std::cout << "UDP server got data : " << data << std::endl;
        }

    };

    bool run_example;
}

void signal_handler(int s){

    std::cout << "Signal handler got : " << s << std::endl;

    run_example = false;
}

int main() {

    NETTLE::HostPort hp("127.0.0.1", 6066);
    UdpConenctionHandler<15> handler;

    NETTLE::UdpServerN<15> server(hp, handler);

    signal(SIGINT, signal_handler);

    if(!server.serve()) {
        std::cerr << "Unable to start UDP server!" << std::endl;
        return 1;
    }

    int num = 0;
    run_example = true;
    while(run_example) {

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        std::cout << "Writing to server ..." << std::endl;

        NETTLE::Writer writer(hp, NETTLE::WriterType::UDP);

        if(writer.hasError()) {

            writer.socketClose();

            std::cout << "Writer experienced an error!" << std::endl;
            return 1;
        }

        std::string test = "UDP  write - " + std::to_string(num++);

        writer.socketWriteOut(test.c_str(), test.size());

        writer.socketClose();
    }

    if(!server.stop()) {

        std::cerr << "Unable to stop the server!" << std::endl;
    }

    return 0;
}