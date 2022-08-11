//
// Created by bosley on 3/31/20.
//

#include <nettle/ConnectionHandler.hpp>
#include <nettle/HostPort.hpp>
#include <nettle/TcpServer.hpp>
#include <nettle/Writer.hpp>

#include <signal.h>
#include <iostream>

namespace {
    class Handler : public nettle::TcpConnectionHandler {

    public:
        Handler(std::string name) : name(name) {

        }

        ~Handler() {

        }

        void serverStarted() override {

            std::cout << "Handler [ " << name << " ] was informed that the server started!" << std::endl;
        }

        void serverStopping() override {

            std::cout << "Handler [ " << name << " ] was informed that the server is stopping!" << std::endl;
        }

        void serverStopped() override {

            std::cout << "Handler [ " << name << " ] was informed that the server stopped!" << std::endl;
        }

        void newConnection(nettle::Socket connection) override {

            char buffer[15];
            connection.socketReadIn(buffer, 15);

            std::cout << "Handler [ " << name << " ] got data : " << buffer << std::endl;

            connection.socketClose();
        }

    private:
        std::string name;
    };

    bool run_example;
}

void signal_handler(int s){

    std::cout << "Signal handler got : " << s << std::endl;

    run_example = false;
}

int main() {

    nettle::HostPort hp("127.0.0.1", 4097);
    Handler handler("TcpExample");
    nettle::TcpServer server(hp, handler);

    signal(SIGINT, signal_handler);

    if(!server.serve()) {
        std::cerr << "Unable to start TCP server!" << std::endl;
        return 1;
    }

    int num = 0;
    run_example = true;
    while(run_example) {

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        std::cout << "Writing to server ..." << std::endl;

        nettle::Writer writer(hp, nettle::WriterType::TCP);

        if(writer.hasError()) {

            writer.socketClose();

            std::cout << "Writer experienced an error!" << std::endl;
            return 1;
        }

        std::string test = "TCP  write - " + std::to_string(num++);

        writer.socketWriteOut(test.c_str(), test.size());

        writer.socketClose();
    }

    if(!server.stop()) {

        std::cerr << "Unable to stop the server!" << std::endl;
    }

    return 0;

}