#include <vector>

class Solution {
private:
    std::vector<int> truck_tour;
    std::vector<std::vector<int>> resupply_drone;
    std::vector<std::vector<int>> drone_tour;

public:
    std::vector<int> getTruckTour() {
        return truck_tour;
    }

    void setTruckTour(const std::vector<int>& tour) {
        truck_tour = tour;
    }

    std::vector<std::vector<int>> getResupplyDrone() {
        return resupply_drone;
    }

    void setResupplyDrone(const std::vector<std::vector<int>>& drone) {
        resupply_drone = drone;
    }

    std::vector<std::vector<int>> getDroneTour() {
        return drone_tour;
    }

    void setDroneTour(const std::vector<std::vector<int>>& tour) {
        drone_tour = tour;
    }

    double getObjective();
    bool isFeasible();
    void greedy();
};