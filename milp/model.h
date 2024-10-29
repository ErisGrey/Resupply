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
    int order_resupply = 0;
    int trip_resupply = 0;
    int order_served = 0;
};

class Model {
public:
    virtual Solution run() = 0;
};