#include <iostream>
#include <lib/Writer.hpp>

#include "HostPort.hpp"
#include "Socket.hpp"
#include "TcpServer.hpp"
#include "UdpServerN.hpp"
#include "ConnectionHandler.hpp"

#include "CppUTest/TestHarness.h"

namespace
{
    constexpr int MAX_TRYS               = 100;
    constexpr int TCP_TEST_PORT          = 8009;
    constexpr int UDP_TEST_PORT          = 8001;

    // -----------------------------------------------------------------------------------------------------------------

    class TestConnectionHandler : public NETTLE::TcpConnectionHandler {

    public:
        TestConnectionHandler(std::string name) : name(name), gotAConnection(false) {

        }

        ~TestConnectionHandler() {

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

        void newConnection(NETTLE::Socket connection) override {

            gotAConnection = true;

            char buffer[11];
            connection.socketReadIn(buffer, 11);

            std::cout << "Handler [ " << name << " ] got data : " << buffer << std::endl;

            connection.socketClose();
        }

        bool handlerGotAConenction() const {

            return gotAConnection;
        }

    private:
        std::string name;
        bool gotAConnection;
    };

    // -----------------------------------------------------------------------------------------------------------------

    template <std::size_t N>
    class UdpConenctionHandler : public NETTLE::UdpConnectionHandlerN<N> {

    public:
        UdpConenctionHandler() : gotConn(false) {

        }

        void serverStarted() override  {
            std::cout << "UDP<" << N << "> Was informed that the server started " << std::endl;
        }

        void serverStopping() override {
            std::cout << "UDP<" << N << "> Was informed that the server is stopping " << std::endl;
        }

        void serverStopped() override  {
            std::cout << "UDP<" << N << "> Was informed that the server stopped " << std::endl;
        }

        void newData(uint8_t data[N]) override {

            std::cout << "UDP<" << N << "> got data : " << data << std::endl;

            gotConn = true;
        }

        bool handlerGotAConnection() const {

            return gotConn;
        }

    private:

        bool gotConn;
    };
}

TEST_GROUP(Tcp)
{
};

TEST(Tcp, TcpTest)
{
    NETTLE::HostPort hp("127.0.0.1", TCP_TEST_PORT);
    TestConnectionHandler handler("TcpTest");
    NETTLE::TcpServer server(hp, handler);

    CHECK_FALSE_TEXT(server.stop(), "Stopped inactive server?");

    CHECK_TRUE_TEXT(server.serve(), "Unable to start server thread");

    CHECK_FALSE_TEXT(server.serve(), "Started active server?");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    NETTLE::Writer writer(hp, NETTLE::WriterType::TCP);

    if(writer.hasError()) {

        writer.socketClose();

        FAIL("Writer reported an error!");
    }

    std::string test = "TCP  write";

    writer.socketWriteOut(test.c_str(), test.size());

    writer.socketClose();

    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    CHECK_TRUE_TEXT(handler.handlerGotAConenction(), "Handler did not get requested writer connection");

    CHECK_TRUE_TEXT(server.stop(), "Unable to stop active server..");
}

TEST_GROUP(Udp)
{

};

TEST(Udp, UdpTest)
{
    NETTLE::HostPort hp("127.0.0.1", UDP_TEST_PORT);
    UdpConenctionHandler<10> handler;

    NETTLE::UdpServerN<10> server(hp, handler);

    CHECK_FALSE_TEXT(server.stop(), "Stopped inactive server?");

    CHECK_TRUE_TEXT(server.serve(), "Unable to start server thread");

    CHECK_FALSE_TEXT(server.serve(), "Started active server?");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    NETTLE::Writer writer(hp, NETTLE::WriterType::UDP);

    if(writer.hasError()) {

        writer.socketClose();

        FAIL("Writer reported an error!");
    }

    std::string test = "UDP  write";

    writer.socketWriteOut(test.c_str(), test.size());

    writer.socketClose();

    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    CHECK_TRUE_TEXT(handler.handlerGotAConnection(), "Handler did not get requested writer connection");

    CHECK_TRUE_TEXT(server.stop(), "Unable to stop active server..");

}


