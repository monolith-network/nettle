//
// Created by bosley on 3/31/20.
//

#include "HostPort.hpp"

#include <sstream>

namespace nettle
{
   HostPort::HostPort(std::string address, short port) : address(address),
                                                         port(port)
   {
   }

   std::string HostPort::getAddress() const
   {

      return address;
   }

   short HostPort::getPort() const
   {

      return port;
   }

}