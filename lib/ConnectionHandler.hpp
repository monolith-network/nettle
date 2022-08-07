//
// Created by bosley on 3/31/20.
//

#ifndef NETTLE_SOCKETHANDLER_HPP
#define NETTLE_SOCKETHANDLER_HPP

#include "Socket.hpp"

//!
//! \file ConnectionHandler.hpp
//!
namespace NETTLE
{
    //!
    //! \class TcpConnectionHandler
    //!
    class TcpConnectionHandler {
    public:
        virtual void serverStarted()  = 0;
        virtual void serverStopping() = 0;
        virtual void serverStopped()  = 0;
        virtual void newConnection(NETTLE::Socket connection) = 0;

    };

    //!
    //! \class UdpConnectionHandlerN
    //!
    template<std::size_t N>
    class UdpConnectionHandlerN {
    public:
        virtual void serverStarted()  = 0;
        virtual void serverStopping() = 0;
        virtual void serverStopped()  = 0;
        virtual void newData(uint8_t data[N]) = 0;
    };
}

#endif //NETTLE_SOCKETHANDLERIF_HPP
