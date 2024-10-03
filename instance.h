#pragma once
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>


struct Job {
    int processing_time;
    int weight;
};

class Instance
{
protected:
    Instance(const std::string input)
    {
        std::ifstream input_file(input);

        if (!input_file.is_open())
            std::cerr << "File input is missing !!";

        input_file >> nb_job;
        input_file >> nb_machine;
        input_file >> nb_conflict;
        input_file >> horizon;
        jobs.resize(nb_job);
        conflicts.resize(nb_conflict);

        for (int i = 0; i < nb_job; i++)
            input_file >> jobs[i].processing_time >> jobs[i].weight;
        for (int i = 0; i < nb_conflict; i++)
            input_file >> conflicts[i].first >> conflicts[i].second;
        std::cerr << nb_job << ' ' << nb_machine << ' ' << nb_conflict << '\n';
    }
    static Instance* singleton_;

    int nb_job, nb_machine, nb_conflict, horizon;
    std::vector < Job > jobs;
    std::vector< std::pair< int, int > > conflicts;
public:
    Instance(Instance& other) = delete;
    void operator=(const Instance&) = delete;
    static Instance* getInstance(const std::string input);

    int getNbJob() { return nb_job; }
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
    }
};

Instance* Instance::singleton_ = nullptr;;
Instance* Instance::getInstance(const std::string input = "")
{
    if (singleton_ == nullptr) {
        singleton_ = new Instance(input);
    }
    return singleton_;
}