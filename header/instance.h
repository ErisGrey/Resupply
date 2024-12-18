﻿#pragma once
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>


struct Point {
    float x;
    float y;
    int release_date;
};

class Instance
{
protected:
    Instance(const std::string input, const std::string param, const std::string mode)
    {
        std::vector<std::string> path;
        split(input, path, '/');
        instanceName = path[path.size() - 1];
        std::size_t dotPosition = instanceName.rfind('.');
        if (dotPosition != std::string::npos) {
            // Erase the part after the dot
            instanceName = instanceName.substr(0, dotPosition);
        }
        std::cout << instanceName << std::endl;
        read_input(input, mode);
        read_param_input(param);
        initialize();
        
    }
    static Instance* singleton_;
    std::vector < Point > Points;

public:
    std::string instanceName;
    int num_nodes;
    int v_truck;
    int v_drone;
    int delta;
    int drone_capacity;
    std::vector<float> x;
    std::vector<float> y;
    std::vector<int> release_time;
    std::vector< std::vector<float> >time_drone;
    std::vector< std::vector<float> >time_truck;
    std::vector< std::vector<float> > dist_truck;
    std::vector< std::vector<float> > dist_drone;
    std::vector<int> truckonly;
    std::vector<int> freemode;
    Instance(Instance& other) = delete;
    void operator=(const Instance&) = delete;
    static Instance* getInstance(const std::string input = "", const std::string param = "", const std::string mode = "");
    template <class Container>
    void split(const std::string& str, Container& cont, char delim);
    int getNumNode() { return num_nodes; }
    void read_input(const std::string input, const std::string mode);
    void read_param_input(const std::string param)
    {
        std::ifstream myFile(param);

        if (!myFile.is_open())
            std::cerr << "File input is missing !!";

        std::string line;
        std::vector<std::string> numbers;


        std::getline(myFile, line);
        split(line, numbers, ',');
        v_truck = stoi(numbers[1]);
        
        std::getline(myFile, line);
        split(line, numbers, ',');
        v_drone = stoi(numbers[1]);

        std::getline(myFile, line);
        split(line, numbers, ',');
        delta = stoi(numbers[1]);

        std::getline(myFile, line);
        split(line, numbers, ',');
        drone_capacity = stoi(numbers[1]);

        //std::cout << "drone_capacity = " << drone_capacity << '\n';
    }
    void initialize()
    {
        time_drone.resize(num_nodes);
        time_truck.resize(num_nodes);
        for (int i = 0; i < num_nodes; i++)
        {
            time_drone[i].resize(num_nodes);
            time_truck[i].resize(num_nodes);
        }

        dist_drone.resize(num_nodes);
        dist_truck.resize(num_nodes);
        for (int i = 0; i < num_nodes; i++)
        {
            dist_drone[i].resize(num_nodes);
            dist_truck[i].resize(num_nodes);
        }

        for (int i = 0; i < num_nodes; i++) {
            for (int j = 0; j < num_nodes; j++) {
                float euc_d = pow(pow(x[i] - x[j], 2) + pow(y[i] - y[j], 2), 0.5);
                float man_d = abs(x[i] - x[j]) + abs(y[i] - y[j]);

                dist_drone[i][j] = euc_d;
                dist_truck[i][j] = man_d;


                time_drone[i][j] = ((euc_d / v_drone) * 60); //unit minute
                time_truck[i][j] = ((man_d / v_truck) * 60); //unit minute
            }
        }

        for (int i = 0; i < num_nodes; i++) {
            
            std::cout << "time_drone[" << 0 << "][" << i << "] = " << time_drone[0][i] << '\n';

            //            assert(time_drone[i][j] * 2 <= maxTdrone);
            //            assert(time_truck[i][j] * 2 <= maxTtruck);
            
        }

        truckonly = {};
        freemode = {};
        for (int i = 1; i < num_nodes - 1; ++i) {
            if (time_drone[0][i] >= 45)
                truckonly.push_back(i);
            else
                freemode.push_back(i);

        }

        for (auto i : truckonly)
        {
            std::cout << i << " ";
        }
        std::cout << '\n';

    }

    double tdrone_all(int i)
    {
        return 2 * time_drone[0][i];
    }

    /*int getNbJob() { return nb_job; }
    int getNbMachine() { return nb_machine; }
    int getNbConflict() { return nb_conflict; }
    int getHorizon() { return horizon; }
    int getSumPt() {
        int sum = 0;
        for (int i = 0; i < nb_job; i++)
            sum += jobs[i].processing_time;
        return sum;
    }
    std::vector < Job > getJobsList() { return jobs; }
    Job getJob(int i) { return jobs[i]; }
    std::vector< std::pair< int, int > > getConflictList() {
        return conflicts;
    }*/
};
