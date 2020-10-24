#include <iostream>
#include <map>

#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <stack>

template <class K = std::string, class V = int> struct alignas(8) Data {
  std::string name;
  std::map<K, std::vector<V>> map;

  [[deprecated("THIS IS A TEST.")]] void do_something() {
    for (auto &[first, second] : this->map) {
      std::cout << first << " : " << second.size() << std::endl;
    }
  }
};

template <typename... Ts>
const std::array<std::common_type_t<Ts...>, sizeof...(Ts)>
make_array(Ts &&... ts) {
  return {std::forward<Ts>(ts)...};
}

int main() {
  Data<> data{"hello", {{"a", {1, 2, 3}}}};

  data.do_something();

  auto b = make_array(1, 2, 3);

  for (auto &i : b) {
    std::cout << i << std::endl;
  }

  return 0;
}