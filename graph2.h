#pragma once

#include <algorithm>
#include <functional>
#include <iterator>
#include <map>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace com::xxx {
template <class V, class K = std::string> class Graph2 {
public:
  Graph2() {}
  ~Graph2() {}

  inline bool add_vertex(const K &id, V &&v);
  bool add_edge(const K &from, const K &to);

  bool visit(const K &start,
             std::function<bool(const K &id, const V &vertex)> visitor,
             bool is_forward = true);

  std::string to_string();

  const V &operator[](const K &id);

private:
  std::unordered_map<K, V> vertices;
  std::unordered_map<K, std::vector<K>> forwards;
  std::unordered_map<K, std::vector<K>> backwards;
};

template <class V, class K> bool Graph2<V, K>::add_vertex(const K &id, V &&v) {
  this->vertices.emplace(id, v);
  return true;
}

template <class V, class K>
bool Graph2<V, K>::add_edge(const K &from, const K &to) {
  if (this->vertices.find(from) == this->vertices.end() ||
      this->vertices.find(to) == this->vertices.end()) {
    throw std::domain_error("from or to does not exist");
  }

  if (this->forwards.find(from) == this->forwards.end()) {
    this->forwards.emplace(from, std::move(std::vector<K>()));
  }

  if (std::find(this->forwards[from].begin(), this->forwards[from].end(), to) ==
      this->forwards[from].end()) {
    this->forwards[from].push_back(to);
  }

  if (this->backwards.find(to) == this->backwards.end()) {
    this->backwards.emplace(to, std::move(std::vector<K>()));
  }

  if (std::find(this->backwards[to].begin(), this->backwards[to].end(), from) ==
      this->backwards[to].end()) {
    this->backwards[to].push_back(from);
  }

  return true;
}

template <class V, class K>
bool Graph2<V, K>::visit(
    const K &start, std::function<bool(const K &id, const V &vertex)> visitor,
    bool is_forward) {
  std::unordered_map<K, std::vector<K>> &this_forwards =
      is_forward ? this->forwards : this->backwards;
  std::unordered_map<K, std::vector<K>> &this_backwards =
      is_forward ? this->backwards : this->forwards;

  std::unordered_set<K> visited, unvisited;

  bool keepGoing = visitor(start, this->vertices[start]);
  unvisited.erase(start);
  visited.emplace(start);

  if (!keepGoing) {
    return false;
  }

  std::for_each(this_forwards[start].begin(), this_forwards[start].end(),
                [&unvisited](const std::string &backward) {
                  unvisited.emplace(backward);
                });

  while (keepGoing && unvisited.size() > 0) {
    do {
      for (auto itor = unvisited.begin(); itor != unvisited.end(); itor++) {
        const K &id = *itor;

        if (std::find(unvisited.begin(), unvisited.end(), id) !=
            unvisited.end()) {
          std::vector<K> &id_backwards = this_backwards[id];

          if (std::any_of(id_backwards.begin(), id_backwards.end(),
                          [&unvisited](const K &backward) -> bool {
                            return std::find(unvisited.begin(), unvisited.end(),
                                             backward) == unvisited.end();
                          })) {
            keepGoing = visitor(id, this->vertices[id]);
            unvisited.erase(id);
            visited.emplace(id);
            break;
          }
        }

        if (!keepGoing) {
          break;
        }
      }
    } while (unvisited.size() > 0);

    for (auto itor = visited.begin(); itor != visited.end(); itor++) {
      const std::string &id = *itor;

      std::vector<K> &id_forwards = this_forwards[id];

      std::for_each(
          id_forwards.begin(), id_forwards.end(),
          [&unvisited, &visited, &this_forwards, &this_backwards,
           this](const std::string &forward) {
            if (visited.find(forward) == visited.end()) {
              std::vector<K> &id_backwards = this_backwards[forward];

              if (std::all_of(id_backwards.begin(), id_backwards.end(),
                              [&visited](const std::string &backward) -> bool {
                                return visited.find(backward) != visited.end();
                              })) {
                unvisited.emplace(forward);
              }
            }
          });
    }
  }

  return true;
}

template <class V, class K> std::string Graph2<V, K>::to_string() {
  std::ostringstream outs;

  outs << "[forwards]" << std::endl;
  std::for_each(this->forwards.begin(), this->forwards.end(),
                [&outs](const auto &item) -> void {
                  outs << (item.first) << " -> " << std::endl;

                  std::for_each(item.second.begin(), item.second.end(),
                                [&outs](const K &i) -> void {
                                  outs << "    : " << i << std::endl;
                                });
                });

  outs << "[backwards]" << std::endl;
  std::for_each(this->backwards.begin(), this->backwards.end(),
                [&outs](const auto &item) -> void {
                  outs << (item.first) << " <- " << std::endl;

                  std::for_each(item.second.begin(), item.second.end(),
                                [&outs](const K &i) -> void {
                                  outs << "    : " << i << std::endl;
                                });
                });

  return outs.str();
}

template <class V, class K> const V &Graph2<V, K>::operator[](const K &id) {
  auto itor = this->vertices.find(id);

  if (itor != this->vertices.end()) {
    return itor->second;
  }

  throw std::domain_error((boost::format("Invalid id : %s") % id).str());
}
} // namespace com::xxx