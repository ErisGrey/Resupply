#pragma once
#include "Model.h"
#include "Params.h"
#include "fstream"

class Runner {
private:
    Model* model;
public:
    Runner(Model* model_) {
        model = model_;
    }
    void run(std::ofstream out) {
        Solution sol = model->run();
        out << sol.status << '\n';
        out << sol.obj << ' ' << sol.time << '\n';
        std::cerr << sol.st.size() << '\n';
        for (auto i : sol.st)
            out << i << '\n';

        Param* param = Param::getInstance();
        std::ofstream log("C:/Users/admin/Downloads/Instances/Sol/run_log.csv", std::ios_base::app);
        log << param->getInstanceName() << ',' << sol.status << ',' << sol.obj << ',' << sol.time << '\n';
    }
};