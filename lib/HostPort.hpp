//
// Created by bosley on 3/31/20.
//

#ifndef NETTLE_HOSTPORT_HPP
#define NETTLE_HOSTPORT_HPP

#include <string>

namespace nettle
{
   class HostPort
   {
   public:
      HostPort(std::string address, short port);

      std::string getAddress() const;
      short getPort() const;

   private:
      std::string address;
      short port;
   };
}

#endif // NETTLE_HOSTPORT_HPP
