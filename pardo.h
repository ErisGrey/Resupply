#pragma once
#include "Model.h"
#include "Params.h"
#include <algorithm>

class RankBase : public Model {
private:
    IloEnv env;
    IloArray < IloArray< IloBoolVarArray > >  x;
    IloBoolVarArray z;
    IloArray< IloBoolVarArray > y;
    IloArray< IloNumVarArray > t;
    IloIntVarArray s;
    int K;
public:
    RankBase() {
        Instance* instance = Instance::getInstance();
        env = IloEnv();
        int p_min = 1e9;
        for (Job j : instance->getJobsList())
            p_min = std::min(p_min, j.processing_time);

        K = instance->getSumPt() / p_min;
        x = IloArray< IloArray< IloBoolVarArray > >(env, instance->getNbJob());
        for (int i = 0; i < instance->getNbJob(); i++) {
            x[i] = IloArray< IloBoolVarArray >(env, K);
            for (int k = 0; k < K; k++) {
                x[i][k] = IloBoolVarArray(env, instance->getNbMachine());
                for (int m = 0; m < instance->getNbMachine(); m++)
                    x[i][k][m] = IloBoolVar(env);
            }
        }
        z = IloBoolVarArray(env, instance->getNbJob());
        for (int i = 0; i < instance->getNbJob(); i++)
            z[i] = IloBoolVar(env);
        y = IloArray< IloBoolVarArray >(env, instance->getNbJob());
        for (int i = 0; i < instance->getNbJob(); i++) {
            y[i] = IloBoolVarArray(env, instance->getNbJob());
            for (int j = 0; j < instance->getNbJob(); j++) {
                y[i][j] = IloBoolVar(env);
            }
        }

        t = IloArray< IloNumVarArray >(env, K);
        for (int i = 0; i < K; i++) {
            t[i] = IloNumVarArray(env, instance->getNbMachine());
            for (int j = 0; j < instance->getNbMachine(); j++) {
                t[i][j] = IloNumVar(env);
            }
        }
        s = IloIntVarArray(env, instance->getNbJob());
        for (int i = 0; i < instance->getNbJob(); i++)
            s[i] = IloIntVar(env);
    }
    IloModel createModel() {
        Instance* instance = Instance::getInstance();
        Param* param = Param::getInstance();
        IloModel model(env);
        IloExpr objExpr(env);
        int B = instance->getSumPt() + 1;
        int T = param->getHorizonRatio() * instance->getSumPt();
        std::cerr << "T = " << T << '\n';
        if (param->getProbId() == 1) {
            for (int i = 0; i < instance->getNbJob(); ++i)
                objExpr += instance->getJobsList()[i].weight * z[i];
            model.add(IloMaximize(env, objExpr));
        }
        else if (param->getProbId() == 2) {
            for (int i = 0; i < instance->getNbJob(); ++i)
                objExpr += instance->getJobsList()[i].weight * s[i];
            model.add(IloMinimize(env, objExpr + 0.2));
        }
        else if (param->getProbId() == 3) {
            IloNumVar T_max(env);
            for (int i = 0; i < instance->getNbJob(); ++i)
                model.add(T_max >= s[i] + instance->getJob(i).processing_time + 0.2);
            model.add(T >= T_max);
            model.add(IloMinimize(env, T_max));
        }

        for (int k = 0; k < K; k++) {
            for (int m = 0; m < instance->getNbMachine(); m++) {
                IloExpr sum_pt(env);
                for (int i = 0; i < instance->getNbJob(); i++)
                    sum_pt += instance->getJob(i).processing_time * x[i][k][m];

                model.add(t[k][m] + sum_pt <= T + 1);

                IloExpr sum(env);
                for (int i = 0; i < instance->getNbJob(); i++)
                    sum += x[i][k][m];
                model.add(sum <= 1);
            }
        }

        for (int i = 0; i < instance->getNbJob(); i++) {
            IloExpr sum(env);
            for (int k = 0; k < K; k++)
                for (int m = 0; m < instance->getNbMachine(); m++)
                    sum += x[i][k][m];
            model.add(sum == z[i]);
            if (param->getProbId() > 1)
                model.add(sum == 1);
            model.add(s[i] >= 1);
        }

        for (int k = 0; k < K - 1; k++) {
            for (int m = 0; m < instance->getNbMachine(); m++) {
                IloExpr sum_pt(env);
                for (int i = 0; i < instance->getNbJob(); i++)
                    sum_pt += instance->getJob(i).processing_time * x[i][k][m];
                model.add(t[k][m] + sum_pt <= t[k + 1][m]);
            }
        }

        for (int k = 0; k < K; k++) {
            for (int m = 0; m < instance->getNbMachine(); m++)
                for (int i = 0; i < instance->getNbJob(); i++) {
                    model.add(t[k][m] <= s[i] + B * (1 - x[i][k][m]));
                    model.add(s[i] <= t[k][m] + B * (1 - x[i][k][m]));
                }
        }

        for (auto c : instance->getConflictList()) {
            int i = c.first, j = c.second;
            model.add(s[i] + instance->getJob(i).processing_time * z[i] - B * (1 - y[i][j]) <= s[j]);
            model.add(s[j] + instance->getJob(j).processing_time * z[j] - B * (1 - y[j][i]) <= s[i]);
        }

        for (auto c : instance->getConflictList()) {
            int i = c.first, j = c.second;
            model.add(y[i][j] + y[j][i] >= 1);
        }

        return model;
    }

    Solution run() override {
        Param* param = Param::getInstance();

        IloModel model = createModel();
        IloCplex cplex(model);
        cplex.setParam(IloCplex::Param::TimeLimit, param->getTimeLimit()); // Set a time limit if needed
        cplex.setParam(IloCplex::Param::Threads, 1);
        cplex.setParam(IloCplex::Param::MIP::Tolerances::MIPGap, param->getGap());
        cplex.setOut(env.getNullStream());

        auto start_time = std::chrono::high_resolution_clock::now();
        Solution sol;
        if (cplex.solve()) {
            auto end_time = std::chrono::high_resolution_clock::now();
            sol.time = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
            if (cplex.getStatus() == IloAlgorithm::Optimal) {
                sol.status = "Optimal";
                for (int i = 0; i < s.getSize(); i++)
                    sol.st.push_back(cplex.getValue(s[i]));
                sol.obj = cplex.getObjValue();
            }
            else if (cplex.getStatus() == IloAlgorithm::Feasible) {
                sol.status = "Feasible";
                for (int i = 0; i < s.getSize(); i++)
                    sol.st.push_back(cplex.getValue(s[i]));
                sol.obj = cplex.getObjValue();
            }
            else {
                sol.status = "Infeasible";
                sol.obj = -1;
            }
        }
        else {
            auto end_time = std::chrono::high_resolution_clock::now();
            sol.time = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
            sol.status = "NoSolution";
        }
        return sol;
    }
};