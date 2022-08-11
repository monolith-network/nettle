#ifndef NET_WRITER
#define NET_WRITER

#include "Socket.hpp"
#include "HostPort.hpp"
#include <string>
#include <functional>

//!
//! \file Writer.hpp
//! \brief A network writer that can write TCP and UDP messages
//!
namespace nettle
{
   //!
   //! \brief Enum for setting writer type
   //!
   enum class WriterType
   {
      TCP, //! A TCP writer
      UDP  //! A UDP writer
   };

   //!
   //! \class StdWriter
   //! \brief StdWriter creator for TCP and UDP connections
   //!
   class Writer : public Socket
   {
   public:
      //!
      //! \brief Construct a writer
      //! \param HostPort Ip and Port information for remote connection
      //! \param WriterType The type of writer to construct (TCP/UDP)
      //! \param errorCb Callback when an error occurs
      //!
      Writer(HostPort connectionInfo, WriterType connectionType, std::function<void(SocketError)> errorCb = nettle::ErrorSink);

      //!
      //! \brief Deconstruct a writer
      //!
      ~Writer();

      //!
      //! \retval true An error has been flagged, false otherwise
      //!
      bool hasError() const;

   private:
      bool errorPresent;
   };
}

#endif