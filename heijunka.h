#pragma once

#include <boost/format.hpp>
#include <iostream>
#include <map>
#include <set>
#include <vector>

namespace com::xxx {
template <class Family = std::string, class Feature = std::string> struct VC {
  std::string name;
  std::map<Family, Feature> props;
  int n;
};

template <class Family = std::string, class Feature = std::string>
struct Definition {
  std::map<Family, std::vector<std::set<Feature>>> groups;
};

template <class Family = std::string, class Feature = std::string>
struct Combination {
  std::string name;
  std::map<Family, std::set<Feature>> combinations;
  std::vector<VC<Family, Feature>> vcs;

  static std::map<std::string, Combination<Family, Feature>>
  init(const Definition<Family, Feature> &definitions,
       const std::vector<VC<Family, Feature>> &vcs);
  static std::string _id(const Definition<Family, Feature> &definitions,
                         const VC<Family, Feature> &vc);
};

template <class Family = std::string, class Feature = std::string>
struct Heijunka {
  static void
  heijunka(std::map<std::string, int> &plan_orig_map,
           std::function<void(int, const std::string &)> show_up_func,
           long verbose = 0);
  static void
  heijunka(std::map<std::string, int> &plan_orig_map,
           std::map<std::string, Combination<Family, Feature>> &combinations,
           std::function<void(int, const VC<Family, Feature> &)> show_up_func,
           long verbose = 0);
};

template <class Family, class Feature>
std::string Combination<Family, Feature>::_id(
    const Definition<Family, Feature> &definitions,
    const VC<Family, Feature> &vc) {
  std::string str;

  for (auto &[first, second] : vc.props) {
    std::string values;

    auto itor = definitions.groups.find(first);

    if (itor != definitions.groups.end()) {
      for (auto &set : itor->second) {
        if (set.find(second) != set.end()) {
          for (auto &i : set) {
            values.append(i);
            values.append(",");
          }
          break;
        }
      }
    }

    str.append((boost::format("%s=%s") % first % values).str());
    str.append("|");
  }

  return str;
}

template <class Family, class Feature>
std::map<std::string, Combination<Family, Feature>>
Combination<Family, Feature>::init(
    const Definition<Family, Feature> &definitions,
    const std::vector<VC<Family, Feature>> &vcs) {
  std::map<std::string, Combination<Family, Feature>> ret;

  for (VC<Family, Feature> vc : vcs) {
    std::string id = _id(definitions, vc);

    if (ret.find(id) == ret.end()) {
      ret.emplace(id, std::move(Combination<Family, Feature>()));
    }

    Combination<Family, Feature> &comb = ret[id];

    comb.name = id;
    comb.vcs.push_back(vc);

    for (const auto &[first, second] : vc.props) {
      if (comb.combinations.find(first) == comb.combinations.end()) {
        comb.combinations.emplace(first, std::move(std::set<Feature>()));
      }

      std::vector<std::set<Feature>> &group =
          const_cast<std::map<Family, std::vector<std::set<Feature>>> &>(
              definitions.groups)[first];

      auto &final_second = second;

      auto s = std::find_if(
          group.begin(), group.end(),
          [&final_second](const std::set<Feature> &features) -> bool {
            return features.find(final_second) != features.end();
          });

      if (s != group.end()) {
        for (auto i : *s) {
          comb.combinations[first].emplace(i);
        }
      }
    }
  }

  return ret;
}

template <class Family, class Feature>
void Heijunka<Family, Feature>::heijunka(
    std::map<std::string, int> &plan_orig_map,
    std::function<void(int, const std::string &)> show_up_func, long verbose) {
  if (verbose > 0)
    std::cout << ">> [heijunka]" << std::endl;

  std::map<std::string, int> plan_map;
  std::map<std::string, double> ratio_map;
  std::map<std::string, int> show_up_map;
  int N = 0;

  for (const auto &[first, second] : plan_orig_map) {
    if (second > 0) {
      plan_map.emplace(first, second);
      show_up_map.emplace(first, 0);
      N += second;
    }
  }

  if (verbose > 0)
    std::cout << "N : " << N << std::endl;

  for (const auto &[first, second] : plan_map) {
    ratio_map.emplace(first, ((double)second) / (double)N);
    if (verbose > 0)
      std::cout << "ratio_map[" << first << "] : " << ratio_map[first]
                << std::endl;
  }

  for (int i = 0; i < N; ++i) {
    double max = -101010;
    std::string ref;

    for (const auto &[first, second] : plan_map) {
      double should_be = (((double)(i + 1)) * ratio_map[first]);
      double acctual = (double)show_up_map[first];

      if (verbose > 0)
        std::cout << "key : " << first << ", should_be : " << should_be
                  << ", acctual : " << acctual << std::endl;

      if (should_be - acctual > max) {
        if (verbose > 0)
          std::cout << "should_be - acctual > max" << std::endl;
        max = should_be - acctual;
        ref = first;
      } else if (should_be - acctual == max) {
        if (verbose > 0)
          std::cout << "should_be - acctual == max" << std::endl;
        if (plan_map[ref] < plan_map[first]) {
          if (verbose > 0)
            std::cout << "plan_map[ref] < plan_map[first]" << std::endl;
          max = should_be - acctual;
          ref = first;
        }
      }
    }

    if (std::abs(-101010 - max) < 1e-5) {
      ref = (*plan_map.begin()).first;
    }

    if (verbose > 0)
      std::cout << "max : " << max << ", ref : " << ref << std::endl;

    show_up_func(i, ref);

    show_up_map[ref] += 1;
  }

  if (verbose > 0) {
    for (const auto &[first, second] : show_up_map) {
      std::cout << "show_up_map[" << first << "] : " << show_up_map[first]
                << std::endl;
    }
  }

  if (verbose > 0)
    std::cout << "<< [heijunka]" << std::endl;
}

template <class Family, class Feature>
void Heijunka<Family, Feature>::heijunka(
    std::map<std::string, int> &plan_orig_map,
    std::map<std::string, Combination<Family, Feature>> &combinations,
    std::function<void(int, const VC<Family, Feature> &)> show_up_func,
    long verbose) {
  std::map<std::string, std::vector<std::string>> id_cache;
  std::map<std::string, std::map<std::string, VC<Family, Feature>>> vc_cache;

  for (auto &comb : combinations) {
    auto &key = comb.first;
    std::vector<VC<Family, Feature>> vcs = comb.second.vcs;

    id_cache.emplace(key, std::vector<std::string>());
    vc_cache.emplace(key, std::map<std::string, VC<Family, Feature>>());

    std::map<std::string, int> plan_map;

    for (const VC<Family, Feature> &vc : vcs) {
      plan_map.emplace(vc.name, vc.n);
      vc_cache[key].emplace(vc.name, vc);
    }

    heijunka(
        plan_map,
        [&](int i, const std::string &vc_name) -> bool {
          id_cache[key].push_back(vc_name);
          return true;
        },
        verbose - 1);

    std::reverse(id_cache[key].begin(), id_cache[key].end());
  }

  heijunka(
      plan_orig_map,
      [&](int i, const std::string &combination_id) -> bool {
        std::string vc = id_cache[combination_id].back();
        id_cache[combination_id].pop_back();

        if (verbose > 0)
          std::cout << "combination : " << combination_id << ", vc : " << vc
                    << std::endl;

        show_up_func(i, vc_cache[combination_id][vc]);

        return true;
      },
      verbose - 1);
}
} // namespace com::xxx
