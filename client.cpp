#include <array>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <iostream>
#include <string>

#include "Hello.pb.h"

using namespace boost::asio;
using namespace boost::asio::ip;

io_service ioservice;
tcp::resolver resolv{ioservice};
tcp::socket tcp_socket{ioservice};
std::array<char, 4096> bytes;

void read_handler(const boost::system::error_code &ec,
                  std::size_t bytes_transferred) {
  if (!ec) {
    // std::cout.write(bytes.data(), bytes_transferred);

    com::xxx::message::Hello hello;

    hello.ParseFromArray(bytes.data(), bytes_transferred);

    std::cout << " id : " << hello.id() << std::endl;
    std::cout << " name : " << hello.name() << std::endl;
    std::cout << " timestamp : " << hello.timestamp() << std::endl;

    tcp_socket.async_read_some(buffer(bytes), read_handler);
  }
}

void connect_handler(const boost::system::error_code &ec) {
  if (!ec) {
    // std::string r = "GET / HTTP/1.1\r\nHost:
    // theboostcpplibraries.com\r\n\r\n"; write(tcp_socket, buffer(r));
    tcp_socket.async_read_some(buffer(bytes), read_handler);
  }
}

void resolve_handler(const boost::system::error_code &ec,
                     tcp::resolver::iterator it) {
  if (!ec) {
    std::cout << (*it).host_name() << " -> "
              << (*it).endpoint().address().to_string() << " : "
              << (*it).endpoint().port() << std::endl;
    tcp_socket.async_connect(*it, connect_handler);
  }
}

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  tcp::resolver::query q{"localhost", "2014"};
  resolv.async_resolve(q, resolve_handler);
  ioservice.run();

  google::protobuf::ShutdownProtobufLibrary();
}
