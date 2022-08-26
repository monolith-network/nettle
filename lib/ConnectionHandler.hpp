//
// Created by bosley on 3/31/20.
//

#ifndef NETTLE_SOCKETHANDLER_HPP
#define NETTLE_SOCKETHANDLER_HPP

#include "Socket.hpp"

//!
//! \file ConnectionHandler.hpp
//!
namespace nettle
{
    //!
    //! \class TcpConnectionHandler
    //!
    class TcpConnectionHandler {
    public:
        virtual void serverStarted()  = 0;
        virtual void serverStopping() = 0;
        virtual void serverStopped()  = 0;

        //! \brief Retrieve a new connection
        //! \note This call is not handled asynchronously and will be 
        //!       blocking the tcp server. It is up to the callee to 
        //!       manage any asynchronous work to ensure that the 
        //!       server is non blocking 
        virtual void newConnection(nettle::Socket connection) = 0;
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
