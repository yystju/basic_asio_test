#include <boost/asio/buffer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <ctime>
#include <string>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <list>

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
  std::ostringstream outs;

  std::time_t now = std::time(nullptr);

  std::string time = std::ctime(&now);

  outs << time << std::endl;

  std::map<std::string, int> plan_map;

  int sed = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

  srand(sed);

  plan_map.emplace("SOME_ZERO", 0);
  outs << "plan_map[SOME_ZERO] : " << plan_map["SOME_ZERO"] << std::endl;

  for (int i = 0; i < 3; ++i) {
    std::string key = (boost::format("%05d") % i).str();
    plan_map.emplace(key, abs(rand() % 10));

    outs << "plan_map[" << key << "] : " << plan_map[key] << std::endl;
  }

  com::xxx::Heijunka<std::string>::heijunka(
      plan_map, [&outs](int i, const std::string &key) -> void {
        outs << std::setw(5) << i << ":" << key << std::endl;
      });

  return outs.str();
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
  tcp_acceptor.listen();
  spawn(ioservice, do_accept);
  ioservice.run();
}
