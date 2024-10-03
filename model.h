#pragma once
#include "Instance.h"
#include <chrono>
#include <ilcplex/ilocplex.h>

class Solution {
public:
    Solution() {
        obj = 0;
        bound = 0;
        time = 0;
        status = "No Solution";
    }
    int obj;
    int bound;
    double time; //in second;
    std::string status; //no solution, feasible or optimal
    std::vector< int > st;
};

class Model {
public:
    virtual Solution run() = 0;
};