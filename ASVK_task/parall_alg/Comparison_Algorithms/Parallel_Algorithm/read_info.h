#ifndef READ_INFO_H
#define READ_INFO_H

#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

#define NUM_ITERATIONS 5000

class Information {
    int processors_count;
    int programs_count;
    int network_load_limit;

    map<int, int> prog_info; // key - num_prog, val - proc_load
public:
    int Get_processors_count() const;
    int Get_programs_count() const;
    int Get_network_load_limit() const;

    int Get_proc_load(int num) const;

    map<pair<int, int>, int> exchange_pair; // key: first - Prog1, second - Prog2; val - intensity_exchange
                                           // Prog1 < Prog2 (т.е. first < second)
    explicit Information(const string &file_name);
};

#endif