#pragma once
#include "model.h"
#include "params.h"
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <ilcplex/ilocplex.h>


typedef IloArray<IloArray<IloNumVarArray>> NumVar3D;
typedef IloArray<IloArray<IloNumArray>> Num3D;
typedef IloArray<IloNumVarArray> NumVar2D;
typedef IloArray<IloNumArray> Num2D;

typedef std::unordered_map<int, int> MapSol2D;
typedef std::vector<std::unordered_map<int, int>> MapSol3D;
typedef std::vector<std::tuple<int, int>> Sol2D;
typedef std::vector<std::tuple<int, int, int>> Sol3D;
class Pardo : public Model {
private:
    std::string insName;
    IloEnv env;
    NumVar2D x;
    NumVar2D r;
    IloNumVarArray u;
    NumVar2D y;
    IloNumVarArray T;
    IloNumVarArray s;
    IloNumVarArray e;
    int n;
    std::vector<std::vector<float>> time_truck;
    std::vector<std::vector<float>> time_drone;
    std::vector<int> N;
    std::vector<int> D;
    std::vector<int> N0;
    std::vector<int> Ne;
    std::vector<int> D0;
    std::vector<int> De;
    std::vector<int> Na;
public:

    Pardo() {
        Instance* instance = Instance::getInstance();
        n = instance->num_nodes;
        insName = instance->instanceName;
        std::cout << "num_nodes = " << n << '\n';
        
        time_drone.resize(n);
        time_truck.resize(n);
        for (int i = 0; i < n; i++)
        {
            time_drone[i].resize(n);
            time_truck[i].resize(n);
        }

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                time_truck[i][j] = instance->time_truck[i][j];
                time_drone[i][j] = instance->time_drone[i][j];
            }
        }

        for (int i = 1; i < n - 1; i++)
        {
            N.push_back(i);
            N0.push_back(i);
            Ne.push_back(i);
            Na.push_back(i);
        }
        N0.push_back(0);
        Ne.push_back(n - 1);
        Na.push_back(0);
        Na.push_back(n - 1);


        for (int i : instance->freemode)
        {
            D.push_back(i);
            D0.push_back(i);
            De.push_back(i);
        }
        D0.push_back(0);
        De.push_back(n - 1);

        std::cout << "N: ";
        for (auto i : N)
            std::cout << i << " ";
        std::cout << '\n';

        std::cout << "N0: ";
        for (auto i : N0)
            std::cout << i << " ";
        std::cout << '\n';

        std::cout << "Ne: ";
        for (auto i : Ne)
            std::cout << i << " ";
        std::cout << '\n';

        std::cout << "Na: ";
        for (auto i : Na)
            std::cout << i << " ";
        std::cout << '\n';

        std::cout << "D: ";
        for (auto i : D)
            std::cout << i << " ";
        std::cout << '\n';

        std::cout << "D0: ";
        for (auto i : D0)
            std::cout << i << " ";
        std::cout << '\n';

        std::cout << "De: ";
        for (auto i : De)
            std::cout << i << " ";
        std::cout << '\n';

        env = IloEnv();
        x = NumVar2D(env, n);
        y = NumVar2D(env, n);
        r = NumVar2D(env, n);
        u = IloNumVarArray(env, n);
        T = IloNumVarArray(env, n);
        s = IloNumVarArray(env, n);
        e = IloNumVarArray(env, n);


        std::stringstream name;
        // x_ij
        for (int i : N0)
        {
            x[i] = IloNumVarArray(env, n);
            for (int j : Ne)
            {
                if (i == j)
                    continue;
                name << "x." << i << "." << j;
                x[i][j] = IloNumVar(env, 0, 1, ILOINT, name.str().c_str());
                name.str("");
            }
        }

        //y_ij
        for (int i : D0)
        {
            y[i] = IloNumVarArray(env, n);
            for (int j : N)
            {
                name << "y." << i << "." << j;
                y[i][j] = IloNumVar(env, 0, 1, ILOINT, name.str().c_str());
                name.str("");
            }
        }

        //r_ij
        for (int i : D0)
        {
            r[i] = IloNumVarArray(env, n);
            for (int j : De)
            {
                if (i == j)
                    continue;
                name << "r." << i << "." << j;
                r[i][j] = IloNumVar(env, 0, 1, ILOINT, name.str().c_str());
                name.str("");
            }
        }

        //u_i
        for (int i : D0)
        {
            name << "u." << i;
            u[i] = IloNumVar(env, 0, 1, ILOINT, name.str().c_str());
            name.str("");
        }

        //T_i
        for (int i : Na)
        {
            name << "T." << i;
            T[i] = IloNumVar(env, 0, IloInfinity, ILOFLOAT, name.str().c_str());
            name.str("");
        }

        //s_i
        for (int i : D)
        {
            name << "s." << i;
            s[i] = IloNumVar(env, 0, IloInfinity, ILOFLOAT, name.str().c_str());
            name.str("");
        }

        //e_i
        for (int i : D)
        {
            name << "e." << i;
            e[i] = IloNumVar(env, 0, IloInfinity, ILOFLOAT, name.str().c_str());
            name.str("");
        }

    }
    IloModel createModel() {
        Instance* instance = Instance::getInstance();
        //Param* param = Param::getInstance();
        IloModel model(env);
        IloExpr objExpr(env);
        objExpr += T[n - 1];
        model.add(IloMinimize(env, objExpr));

        //Truck routing contraints
        IloExpr sum_s(env);
        for (int j : N)
        {
            sum_s += x[0][j];
        }
        model.add(sum_s == 1);

        IloExpr sum_e(env);
        for (int j : N)
        {
            sum_e += x[j][n - 1];
        }
        model.add(sum_e == 1);

        for (int j : N)
        {
            IloExpr sum_in(env);
            for (int i : N0)
            {
                if (i == j)
                    continue;
                sum_in += x[i][j];
            }
            model.add(sum_in == 1);
            sum_in.end();
        }

        for (int i : N)
        {
            IloExpr sum_out(env);
            for (int j : Ne)
            {
                if (i == j)
                    continue;
                sum_out += x[i][j];
            }
            model.add(sum_out == 1);
            sum_out.end();
        }

        //Drone routing consrtaints
        //model.add(u[0] == 1);
        for (int i : D)
        {
            model.add(u[i] <= u[0]);
        }

        IloExpr drin(env);
        for (int j : D)
        {
            drin += r[0][j];
        }
        model.add(drin == u[0]);
        drin.end();

        IloExpr drout(env);
        for (int i : D)
        {
            drout += r[i][n - 1];
        }
        model.add(drout == u[0]);
        drout.end();

        for (int j : D)
        {
            IloExpr sum_ru(env);
            for (int i : D0)
            {
                if (i == j)
                    continue;
                sum_ru += r[i][j];
            }
            model.add(sum_ru == u[j]);
            sum_ru.end();
  
        }

        for (int i : D)
        {
            IloExpr sum_ru(env);
            for (int j : De)
            {
                if (i == j)
                    continue;
                sum_ru += r[i][j];
            }
            model.add(sum_ru == u[i]);
            sum_ru.end();
        }

        //Drone capacity constraint
        for (int i : D)
        {
            IloExpr sum_order(env);
            for (int j : N)
            {
                sum_order += y[i][j] * 1; //1 is weight of order, all order have weight 1
            }
            model.add(sum_order <= instance->drone_capacity * u[i]);
            sum_order.end();
        }

        //Constraint on loading the orders onto the truck
        for (int j : N)
        {
            IloExpr load_order(env);
            for (int i : D0)
            {
                load_order += y[i][j];
            }
            model.add(load_order == 1);
            load_order.end();
        }

        double M = 99999;
        //Synchronization and timing constraints
        for (int j : N)
        {
            float min = std::min(instance->time_drone[0][j], instance->time_truck[0][j]);
            //std::cout << "min = " << min << '\n';
            model.add(T[j] >= instance->release_time[j] + min);
        }

        for (int i : D0)
        {
            for (int j : N)
            {
                if (i == j)
                    continue;
                model.add(T[j] >= T[i] - M * (1 - y[i][j]));
            }
        }

        for (int i : N0)
        {
            for (int j : Ne)
            {
                if (i == j)
                    continue;
                if (isElementInVector(D, j)) {
                    //std::cout << j << '\n';
                    continue;
                }
                    
                model.add(T[j] >= T[i] + instance->time_truck[i][j] - M * (1 - x[i][j]));
            }
        }

        for (int i : N0)
        {
            for (int j : D)
            {
                if (i == j)
                    continue;
                model.add(T[j] >= T[i] + instance->time_truck[i][j] + instance->delta * u[j] - M * (1 - x[i][j]));
            }
        }

        for (int j : D)
        {
            model.add(T[j] >= s[j] + instance->time_drone[0][j] + instance->delta - M * (1 - u[j]));
        }

        for (int i : D)
        {
            for (int j : D)
            {
                if (i == j)
                    continue;
                model.add(s[j] >= T[i] + instance->time_drone[0][i] - M * (1 - r[i][j]));
            }
        }

        //Constraints on the start time of the truck route and drone ﬂights
        for (int i : N)
        {
            for (int j : D)
            {
                model.add(s[j] >= instance->release_time[i] * y[j][i]);
            }
        }

        for (int j : N)
        {
            model.add(T[0] >= instance->release_time[j] * y[0][j]);
        }

        //Computation of wait times and lower bounds for the total delivery time
        for (int j : D)
        {
            for (int i : N0)
            {
                if (i == j)
                    continue;
                model.add(e[j] >= T[j] - (T[i] + instance->time_truck[i][j]) - M * (1 - x[i][j]));
            }
        }

        for (int j : D)
        {
            model.add(e[j] <= M * u[j]);
        }

        for (int i : N)
        {
            model.add(T[n - 1] >= T[i] + instance->time_truck[i][n - 1]);
        }

        IloExpr sum_cost(env);
        sum_cost += T[0];
        for (int i : N0)
        {
            for (int j : Ne)
            {
                if (i == j)
                    continue;
                sum_cost += instance->time_truck[i][j] * x[i][j];
            }
        }

        for (int i : D)
        {
            sum_cost += e[i];
        }

        model.add(T[n - 1] >= sum_cost);
        sum_cost.end();
        
        return model;
    }

    Solution run() override {

        Param* param = Param::getInstance();

        IloModel model = createModel();
        IloCplex cplex(model);
        cplex.extract(model);
        //cplex.setParam(IloCplex::Param::Parallel, 1);
        cplex.setParam(IloCplex::Param::Threads, 1);
        /*cplex.setParam(IloCplex::TiLim, 3600);
        cplex.setParam(IloCplex::TreLim, 28000);
        cplex.setParam(IloCplex::Param::MIP::Strategy::RINSHeur, 10);*/
        //cplex.setParam(IloCplex::Param::TimeLimit, param->getTimeLimit()); // Set a time limit if needed
        //cplex.setParam(IloCplex::Param::Threads, 1);
        //cplex.setParam(IloCplex::Param::MIP::Tolerances::MIPGap, param->getGap());
        //cplex.setOut(env.getNullStream());

        auto start_time = std::chrono::high_resolution_clock::now();
        Solution sol;
        cplex.exportModel("lpex.lp");
        if (cplex.solve()) {
            sol.name = insName;
            auto end_time = std::chrono::high_resolution_clock::now();
            sol.time = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();

            if (cplex.getStatus() == IloAlgorithm::Optimal) {
                sol.status = "Optimal";
                
                std::cout << cplex.getObjValue() << '\n';
                sol.obj = cplex.getObjValue();
                std::vector<std::vector<int>> x_sol(n, std::vector<int>(n, 0));
                for (int i : D)
                {
                    if (cplex.getValue(u[i]) >= 0.5)
                        std::cout << "u[" << i << "] = " << cplex.getValue(u[i]) << std::endl;
                }
                std::cout << '\n';

                for (int i : D)
                {
                    if (cplex.getValue(e[i]) >= 0.5)
                        std::cout << "e[" << i << "] = " << cplex.getValue(e[i]) << std::endl;
                }
                std::cout << '\n';

                for (int i : Na)
                {
                    std::cout << "T[" << i << "] = " << cplex.getValue(T[i]) << std::endl;
                }
                float sum = cplex.getValue(T[0]);
                std::vector<int> tour;
                int current = 0;   // start from point 0
                std::cout << "sum = " << sum << "\n";
                for (int i : N0)
                {
                    for (int j : Ne)
                    {
                        if (i == j) continue;
                        if (cplex.getValue(x[i][j]) >= 0.5)
                        {
                            sum += time_truck[i][j];
                            std::cout << "time_truck[" << i << "][" << j << "] = " << time_truck[i][j] << '\n';
                            std::cout << i << " " << j << std::endl;
                            x_sol[i][j] = 1;
                        }

                    }
                }

                tour.push_back(current);

                while (true) {
                    int next_point = -1;
                    for (int j = 0; j < n; j++) {
                        if (x_sol[current][j] == 1) {  // if there's a tour from current to j
                            next_point = j;
                            break;
                        }
                    }

                    if (next_point == -1) break;  // no next point, end the tour

                    tour.push_back(next_point);
                    current = next_point;  // move to the next point

                    if (current == n - 1) break;  // if we've reached the last point, stop
                }

                // Output the tour
                std::cout << "Truck tour: ";
                for (int node : tour) {
                    if (node == n - 1)
                        std::cout << 0 << " ";
                    else
                        std::cout << node << " ";
                }
                std::cout << std::endl;
               
                std::cout << "Drone tour " << "\n";
                for (int i : D0)
                {
                    for (int j : De)
                    {
                        if (i == j) continue;
                        if (i == 0 && j == n - 1) continue;
                        if (cplex.getValue(r[i][j]) >= 0.5)
                        {
                           
                            //std::cout << "time_truck[" << i << "][" << j << "] = " << time_truck[i][j] << '\n';
                            std::cout << i << " " << j << std::endl;
                           // x_sol[i][j] = 1;
                        }

                    }
                }




                std::cout << "sum = " << sum << "\n";
                /*for (int i : Na)
                {
                    for (int j : Na)
                    {
                        std::cout << x_sol[i][j] << " ";
                    }
                    std::cout << '\n';
                }*/

                /*for (int i = 0; i < s.getSize(); i++)
                    sol.st.push_back(cplex.getValue(s[i]));
                sol.obj = cplex.getObjValue();*/
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


    int isElementInVector(const std::vector<int>& vec, int element) {
        for (int i = 0; i < vec.size(); i++) {
            if (vec[i] == element) {
                return 1;  // Element found
            }
        }
        return 0;  // Element not found
    }
};