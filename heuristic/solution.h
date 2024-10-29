#pragma once
#include <vector>

class Sol {
private:
    std::vector<int> truck_tour;
    std::vector<std::vector<std::vector<int>>> resupply_drone;
    std::vector<std::vector<int>> drone_tour;
    double truck_makespan;
    double drone_makespan;
    double make_span;
    std::vector<double> truck_departure_time;

public:
    Sol();
    Sol& operator=(const Sol& s);

    std::vector<int> getTruckTour();
    void setTruckTour(const std::vector<int>& tour);

    std::vector<std::vector<std::vector<int>>> getResupplyDrone();
    void setResupplyDrone(const std::vector<std::vector<std::vector<int>>>& drone);

    std::vector<std::vector<int>> getDroneTour();
    void setDroneTour(const std::vector<std::vector<int>>& tour);

    double getObjective();
    bool isFeasible();
    void greedy();
};