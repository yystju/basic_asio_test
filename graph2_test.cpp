#include <iostream>
#include <chrono>
#include <boost/format.hpp>
#include "graph2.h"

class A {
    public:
        A(const std::string& name) : name(name) {}
        A() : name("") {}
        ~A() {}

        inline const std::string& get_name() const {return name;}
    private:
    std::string name;
};

int main(int argc, char *argv[]) {
    com::xxx::Graph2<A> g;

    int N = atoi(argv[1]), M = atoi(argv[1]);

    for(int i = 0; i < N; ++i) {
        for(int j = 0; j < M; ++j) {
            std::string id = (boost::format("%05d::%05d") % i % j).str();

            g.add_vertex(id, A(id));

            if(i > 0) {
                std::string pre = (boost::format("%05d::%05d") % (i - 1) % (j)).str();
                g.add_edge(pre, id);
            }

            if(j > 0) {
                std::string pre = (boost::format("%05d::%05d") % (i) % (j - 1)).str();
                g.add_edge(pre, id);
            }
        }
    }

    // std::cout << g.to_string() << std::endl << std::endl;

    auto start = std::chrono::system_clock::now();

    g.visit("00000::00000", [](const std::string& id, const A& vertex) -> bool {
        // std::cout << "[f] id : " << id << ", vertex : " << vertex.get_name() << std::endl;
        return true;
    }, true);

    // g.visit((boost::format("%05d::%05d") % (N - 1) % (M - 1)).str(), [](const std::string& id, const A& vertex) -> bool {
    //     std::cout << "[b] id : " << id << ", vertex : " << vertex.get_name() << std::endl;
    //     return true;
    // }, false);

    auto end = std::chrono::system_clock::now();

    auto interval = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(end - start);

    auto& a = g["00000::00000"];

    std::cout << "a : " << a.get_name() << std::endl;

    std::cout << "INTERVAL : " << interval.count() << " milliseconds" << std::endl;

    return 0;
}