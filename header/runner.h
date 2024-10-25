#pragma once
#include "model.h"
#include "params.h"
#include "fstream"

class Runner {
private:
    Model* model;
public:
    Runner(Model* model_) {
        model = model_;
    }
    void run(std::string path) {
        
        std::ofstream out;
        out.open(path, std::ios_base::app);
        
        Solution sol = model->run();

        if (sol.name == "Instance_0")
        {
            out << "Instance Name" << ',';
            out << "Status" << ',';
            out << "Obj Value" << ',' << "Run Time" << ',';
            out << "Trip Resupply" << ',';
            out << "Order Resupply" << ',';
            out << "Order Served" << '\n';
        }
        out << sol.name << ',';
        out << sol.status << ',';
        out << sol.obj << ',' << sol.time << ',';
        out << sol.trip_resupply << ',';
        out << sol.order_resupply << ',';
        out << sol.order_served << '\n';

       /* std::cerr << sol.st.size() << '\n';
        for (auto i : sol.st)
            out << i << '\n';*/

       /* Param* param = Param::getInstance();
        std::ofstream log("C:/Users/admin/Downloads/Resupply_ins/Sol/run_log.csv", std::ios_base::app);
        log << param->getInstanceName() << ',' << sol.status << ',' << sol.obj << ',' << sol.time << '\n';*/
    }
};