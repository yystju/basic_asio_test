#include <boost/asio/buffer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/write.hpp>
#include <chrono>
#include <ctime>
#include <string>

#include <iomanip>
#include <iostream>
#include <list>
#include <sstream>

#include "Hello.pb.h"
#include "heijunka.h"

/**
 * Copied from https://theboostcpplibraries.com/boost.asio-coroutines
 */

using namespace boost::asio;
using namespace boost::asio::ip;

io_service ioservice;
tcp::endpoint tcp_endpoint{tcp::v6(), 2014};
tcp::acceptor tcp_acceptor{ioservice, tcp_endpoint};
std::list<tcp::socket> tcp_sockets;
std::string data;

std::string do_business() {
  com::xxx::message::Hello hello;

  hello.set_id(1024);
  hello.set_name("THIS IS A TEST.");

  int64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch())
                    .count();

  hello.set_timestamp(now);

  return hello.SerializeAsString();
}

void do_write(tcp::socket &tcp_socket, yield_context yield) {
  std::string data = do_business();
  async_write(tcp_socket, buffer(data), yield);
  tcp_socket.shutdown(tcp::socket::shutdown_send);
}

void do_accept(yield_context yield) {
  for (;;) {
    tcp_sockets.emplace_back(ioservice);
    tcp_acceptor.async_accept(tcp_sockets.back(), yield);
    spawn(ioservice,
          [](yield_context yield) { do_write(tcp_sockets.back(), yield); });
  }
}

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  tcp_acceptor.listen();
  spawn(ioservice, do_accept);
  ioservice.run();

  google::protobuf::ShutdownProtobufLibrary();
}
