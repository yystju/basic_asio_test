#include <iostream>
#include <iomanip>
#include <chrono>
#include <map>
#include <array>

#include "heijunka.h"

int main(int argc, char *argv[]) {
  long verbose = 0;

  com::xxx::Definition<> definitions 
  {{
    {"A", {{"1", "2"}, {"3", "4"}}},
    {"B", {{"v", "w"}, {"x", "y"}}},
    {"C", {{"1", "2"}, {"3", "4"}}},
    {"D", {{"a", "b"}, {"c", "d"}}},
  }};

  std::vector<com::xxx::VC<>> items
  {{
    {"VC1", {{"A", "1"}, {"B", "x"}, {"C", "1"}, {"D", "a"}}, 10},
    {"VC2", {{"A", "2"}, {"B", "x"}, {"C", "1"}, {"D", "a"}}, 10},
    {"VC3", {{"A", "4"}, {"B", "x"}, {"C", "4"}, {"D", "a"}}, 20}
  }};

  std::map<std::string, com::xxx::Combination<>> combinations = com::xxx::Combination<>::init(definitions, items);

  std::map<std::string, int> plan_map;

  for(auto &entry : combinations) {
    int sum = 0;
    for(auto &vc : entry.second.vcs) {
      sum += vc.n;
    }
    plan_map.emplace(entry.first, sum);
  }

  com::xxx::Heijunka<>::heijunka(plan_map, combinations, [](int i, const com::xxx::VC<> &vc) -> void {
    std::cout << std::setw(5) << i << ":" << vc.name << std::endl;
  }, verbose);

  return 0;
}
