#include <chrono>
#include <iostream>
#include <thread>

#include <cstdio>

#include <boost/asio.hpp>
#include <boost/fiber/all.hpp>

#include "fiber_example/yield.hpp"

/**
 * https://www.boost.org/doc/libs/1_74_0/libs/fiber/doc/html/fiber/scheduling.html
 */
template <typename payload_type> class BuzzThread {
public:
  struct traits_type {
    using channel_t = boost::fibers::buffered_channel<payload_type>;

    using alg_t = boost::fibers::algo::work_stealing;
  };

  BuzzThread(int n = std::thread::hardware_concurrency())
      : n(n), thread(this->run) {}

  ~BuzzThread() {}

  void run() {
    boost::fibers::use_scheduling_algorithm<traits_type::alg_t>(this->n);
  }

private:
  std::uint32_t n;
  std::thread thread;
};

typedef boost::fibers::buffered_channel<uint64_t> channel_t;

boost::asio::io_context context;
boost::asio::io_service service;
boost::asio::ip::tcp::resolver resolver{service};
boost::asio::ip::tcp::socket the_socket{service};
std::array<char, 4096> bytes;

void read_handler(const boost::system::error_code &ec,
                  std::size_t bytes_transferred) {
  if (!ec) {
    std::cout.write(bytes.data(), bytes_transferred);
    the_socket.async_read_some(boost::asio::buffer(bytes), read_handler);
  }
}

int main(int argc, char *argv[]) {
  boost::asio::ip::tcp::resolver::query q{argv[1], argv[2]};

  std::array<char, 1024 * 8> buff;

  snprintf(buff.data(), buff.size(), "GET / HTTP/1.1\r\nHost: %s\r\n\r\n",
           argv[1]);

  resolver.async_resolve(q, [&buff](
                                const boost::system::error_code &ec,
                                boost::asio::ip::tcp::resolver::iterator it) {
    if (!ec) {
      the_socket.async_connect(*it, [&buff](
                                        const boost::system::error_code &ec) {
        if (!ec) {
          write(the_socket, boost::asio::buffer(buff));
          the_socket.async_read_some(boost::asio::buffer(bytes), read_handler);
        }
      });
    }
  });
  service.run();
  return 0;
}

int main_fiber() {
  channel_t c(8);
  uint64_t receive;

  boost::fibers::fiber f(
      boost::fibers::launch::dispatch, std::allocator_arg,
      boost::fibers::fixedsize_stack(
          boost::fibers::fixedsize_stack::traits_type::default_size()),
      [&c]() {
        for (int i = 0; i < 1000; ++i) {
          boost::this_fiber::sleep_for(std::chrono::milliseconds(20));
          std::cout << "FROM FIBER" << boost::this_fiber::get_id() << " PUSH "
                    << i << "." << std::endl;
          c.push(i);
        }

        c.close();
      });
  f.detach();

  for (;;) {
    boost::fibers::channel_op_status s =
        c.pop_wait_for(receive, std::chrono::milliseconds(5000));

    if (s == boost::fibers::channel_op_status::success) {
      std::cout << "RECEIVED : " << receive << std::endl;
    } else if (s == boost::fibers::channel_op_status::closed) {
      break;
    }
  }
  return 0;
}