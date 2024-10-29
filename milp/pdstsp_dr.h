#pragma once
#include "Model.h"
#include "Params.h"
#include <algorithm>
#include <unordered_map>
#include <unordered_set>


typedef IloArray<IloArray<IloNumVarArray>> NumVar3D;
typedef IloArray<IloArray<IloNumArray>> Num3D;
typedef IloArray<IloNumVarArray> NumVar2D;
typedef IloArray<IloNumArray> Num2D;

typedef std::unordered_map<int, int> MapSol2D;
typedef std::vector<std::unordered_map<int, int>> MapSol3D;
typedef std::vector<std::tuple<int, int>> Sol2D;
typedef std::vector<std::tuple<int, int, int>> Sol3D;
class PDSTSP_DR : public Model {
private:
    IloEnv env;
    NumVar2D x;
    NumVar2D r;
    IloNumVarArray z;
    IloNumVarArray u;
    NumVar2D y;
    IloNumVarArray T;
    IloNumVarArray s;
    IloNumVarArray e;
    IloNumVar T_max;
    IloNumVar Td;
    int n;
    std::vector<int> N;
    std::vector<int> D;
    std::vector<int> N0;
    std::vector<int> Ne;
    std::vector<int> D0;
    std::vector<int> De;
    std::vector<int> Na;
    std::vector<std::vector<int>> time_truck;
    std::string insName;
public:

    PDSTSP_DR() {
        Instance* instance = Instance::getInstance();
        insName = instance->instanceName;
        n = instance->num_nodes;
        std::cout << "num_nodes = " << n << '\n';
        time_truck.resize(n);
        for (int i = 0; i < n; i++)
        {
            time_truck[i].resize(n, 0);
        }

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                time_truck[i][j] = instance->time_truck[i][j];
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

        /*std::cout << "N: ";
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
        std::cout << '\n';*/

        env = IloEnv();
        x = NumVar2D(env, n);
        y = NumVar2D(env, n);
        r = NumVar2D(env, n);
        z = IloNumVarArray(env, n);
        u = IloNumVarArray(env, n);
        T = IloNumVarArray(env, n);
        s = IloNumVarArray(env, n);
        e = IloNumVarArray(env, n);
        T_max = IloNumVar(env);
        Td = IloNumVar(env);

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
        //z_i
        for (int i : N)
        {
            name << "z." << i;
            z[i] = IloNumVar(env, 0, 1, ILOINT, name.str().c_str());
            name.str("");
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

        //T_max
        name << "T_max";
        T_max = IloNumVar(env, 0, IloInfinity, ILOFLOAT, name.str().c_str());
        name.str("");

        //Td
        name << "Td";
        Td = IloNumVar(env, 0, IloInfinity, ILOFLOAT, name.str().c_str());
        name.str("");

    }
    IloModel createModel() {
        Instance* instance = Instance::getInstance();
        Param* param = Param::getInstance();
        IloModel model(env);

        //Anh swuar chỗ này nhá anh :)))

        IloExpr objExpr(env);
        objExpr += T_max;
        model.add(IloMinimize(env, objExpr));

        

        //Makespan of whole tour
        model.add(T_max >= T[n - 1]);
        model.add(T_max >= Td);

        //test
        //model.add(z[1] == 0);
        //model.add(u[2] == 1);
        //model.add(T[0] == 10);




        //Truck routing contraints
        IloExpr sum_s(env);
        for (int j : N)
        {
            sum_s += x[0][j];
        }
        model.add(sum_s <= 1);

        IloExpr sum_e(env);
        for (int j : N)
        {
            sum_e += x[j][n - 1];
        }
        //model.add(sum_e <= 1);

        model.add(sum_s == sum_e);

        for (int j : N)
        {
            IloExpr sum_in(env);
            for (int i : N0)
            {
                if (i == j)
                    continue;
                sum_in += x[i][j];
            }

            IloExpr sum_out(env);
            for (int i : Ne)
            {
                if (i == j)
                    continue;
                sum_out += x[j][i];
            }

            model.add(sum_in == sum_out);
            model.add(sum_in == z[j]);
        }

        //Drone routing consrtaints
        for (int i : D)
        {
            model.add(u[i] <= u[0]);
        }

        IloExpr drin(env);
        for (int j : D)
        {
            drin += r[0][j];
        }
        model.add(drin == 1);

        IloExpr drout(env);
        for (int i : D)
        {
            drout += r[i][n - 1];
        }
        //model.add(drout == 1);

        model.add(drin == drout);
        drin.end();
        drout.end();
        for (int j : D)
        {
            model.add(r[0][j] <= u[j] + (1 - z[j]));
            model.add(u[j] <= z[j]);
        }

        for (int j : D)
        {
            IloExpr sum_drone_degree(env);
            for (int i : D0)
            {
                if (i == j)
                    continue;
                sum_drone_degree += r[i][j];
            }
            model.add(sum_drone_degree == u[j] + 1 - z[j]);
            sum_drone_degree.end();
        }

        for (int j : D)
        {
            IloExpr sum_drone_in(env);
            for (int i : D0)
            {
                if (i == j)
                    continue;
                sum_drone_in += r[i][j];
            }

            IloExpr sum_drone_out(env);
            for (int i : De)
            {
                if (i == j)
                    continue;
                sum_drone_out += r[j][i];
            }

            model.add(sum_drone_in == sum_drone_out);
        }

        for (int i : D)
        {
            IloExpr sum_drone_degree(env);
            for (int j : De)
            {
                if (i == j)
                    continue;
                sum_drone_degree += r[i][j];
            }
            model.add(sum_drone_degree == u[i] + 1 - z[i]);
            sum_drone_degree.end();
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

            //test
            //model.add((1 - z[i]) * 1 <= instance->drone_capacity);
        }
        double M = 500;
        //Constraint on loading the orders onto the truck
        for (int j : N)
        {
            IloExpr load_order(env);
            for (int i : D0)
            {
                load_order += y[i][j];
            }
            model.add(load_order == z[j]);
            load_order.end();
        }

        for (int j : D)
        {
            IloExpr load_order(env);
            for (int i : N)
            {
                load_order += y[j][i];
            }

            model.add(load_order <= (instance->num_nodes + 1) * z[j]);
        }


        //Synchronization and timing constraints
        for (int j : N)
        {
            model.add(T[j] >= (instance->release_time[j] + std::min(instance->time_drone[0][j], instance->time_truck[0][j])) * z[j]);
        }

        for (int i : D0)
        {
            for (int j : N)
            {
                if (i == j)
                    continue;
                model.add(T[j] >= T[i] + instance->time_truck[i][j] - M * (1 - y[i][j]));
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

        for (int i : N0)
        {
            for (int j : Ne)
            {
                if (i == j)
                    continue;
                if (isElementInVector(D, j))
                    continue;
                model.add(T[j] >= T[i] + instance->time_truck[i][j] - M * (1 - x[i][j]));
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
                model.add(s[j] >= T[i] + instance->time_drone[0][i] - M * (1 - r[i][j])/* + instance->time_drone[0][i] * (1 - z[i])*/);
            }
        }

        for (int i : D)
        {
            model.add(T[i] <= M * z[i]);
        }

        for (int i : D)
        {
            for (int j : D)
            {
                if (i == j)
                    continue;
                model.add(s[j] >= s[i] + 2 * instance->time_drone[0][i] + instance->delta * u[i] - M * (1 - r[i][j]) - M * z[i]);
            }
        }


        //Constraints on the start time of the truck route and drone ﬂights
        for (int i : N)
        {
            for (int j : D)
            {
                model.add(s[j] >= instance->release_time[i] * y[j][i] + instance->release_time[j] * (1 - z[j]));
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
            model.add(T[n - 1] >= T[i] + instance->time_truck[i][n - 1] - M * (1 - z[i]));
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

        for (int i : D)
        {
            model.add(Td >= s[i] + 2 * instance->time_drone[0][i] + instance->delta * z[i] - M * (1 - r[i][n - 1]));
        }

        IloExpr droneTime(env);
        for (int i : D)
        {
            droneTime += 2 * instance->time_drone[0][i] * (1 - z[i]) + (2 * instance->time_drone[0][i] + instance->delta) * u[i];
        }
        model.add(Td >= droneTime);
 
       
        std::cout << "done" << '\n';
        return model;
    }

    Solution run() override {

        Param* param = Param::getInstance();
     
        IloModel model = createModel();
        IloCplex cplex(model);

        //cplex.setParam(IloCplex::Param::TimeLimit, 200); // Set a time limit if needed
        cplex.setParam(IloCplex::Param::Threads, 1);
        //  cplex.setParam(IloCplex::Param::MIP::Tolerances::MIPGap, param->getGap());
        //cplex.setOut(env.getNullStream());
        cplex.exportModel("lpex.lp");
        auto start_time = std::chrono::high_resolution_clock::now();
        Solution sol;
        
        if (cplex.solve()) {
           
            auto end_time = std::chrono::high_resolution_clock::now();
            sol.time = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
            std::cout << cplex.getObjValue() << '\n';
            sol.obj = cplex.getObjValue();
            sol.name = insName;

            std::ofstream out;
            out.open("C:/Users/maiti/Downloads/TSPrd(time)/TSPrd(time)/Solomon/10/Sol/" + insName + "_sol_float.txt", std::ios_base::app);

            std::vector<std::vector<int>> x_sol(n, std::vector<int>(n, 0));

            int order_drone_served = 0;
            for (int i : N)
            {
                if (cplex.getValue(z[i]) >= 0.5)
                {
                    out << "z[" << i << "] = " << cplex.getValue(z[i]) << std::endl;
                }
                   
                else
                    order_drone_served += 1;
            }
            out << '\n';

            int order_resupply = 0;
            int trip_resupply = 0;
            int trip;
            for (int i : D0)
            {
                trip = 0;
                for (int j : N)
                {
                    if (cplex.getValue(y[i][j]) >= 0.5)
                    {
                        out << "y[" << i << "][" << j << "] = " << cplex.getValue(y[i][j]) << '\n';
                        if (i != 0) {
                            order_resupply += 1;
                            trip += 1;
                        }
                       
                    }
                }
                if (trip >= 1)
                    trip_resupply += 1;
                trip = 0;
            }

            out << "So don hang phuc vu boi drone: " << order_drone_served << std::endl;
            out << "So don hang resupply boi drone: " << order_resupply << std::endl;
            out << "So trip resupply boi drone: " << trip_resupply << std::endl;
            sol.order_resupply = order_resupply;
            sol.order_served = order_drone_served;
            sol.trip_resupply = trip_resupply;

            for (int i : D)
            {
                if (cplex.getValue(u[i]) >= 0.5)
                    out << "u[" << i << "] = " << cplex.getValue(u[i]) << std::endl;
            }
            out << '\n';

            for (int i : D)
            {
                if (cplex.getValue(e[i]) >= 0.5)
                    out << "e[" << i << "] = " << cplex.getValue(e[i]) << std::endl;
            }
            out << '\n';

            for (int i : Na)
            {
                out << "T[" << i << "] = " << cplex.getValue(T[i]) << std::endl;
            }

            for (int i : D)
            {
                out << "s[" << i << "] = " << cplex.getValue(s[i]) << std::endl;
            }
            out << cplex.getValue(Td) << std::endl;
            float sum = cplex.getValue(T[0]);
            
            std::vector<int> tour;
            int current = 0;   // start from point 0

           

            for (int i : N0)
            {
                for (int j : Ne)
                {
                    if (i == j) continue;
                    if (cplex.getValue(x[i][j]) >= 0.5)
                    {
                        sum += time_truck[i][j];
                        out << "x[" << i << "][" << j << "] = " << cplex.getValue(x[i][j]) << '\n';
                        out << i << " " << j << std::endl;
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
            out << "Truck tour: ";
            for (int node : tour) {
                if (node == n - 1)
                    out << 0 << " ";
                else
                    out << node << " ";
            }
            out << std::endl;
            out << "Drone tour: ";
            for (int i : D0)
            {
                for (int j : De)
                {
                    if (i == j) continue;
                    if (i == 0 && j == n - 1) continue;
                    if (cplex.getValue(r[i][j]) >= 0.5)
                    {
                        sum += time_truck[i][j];
                       // std::cout << "time_truck[" << i << "][" << j << "] = " << time_truck[i][j] << '\n';
                        out << "r[" << i << "][" << j << "] = " << cplex.getValue(r[i][j]) << std::endl;

                    }

                }
            }
            
            if (cplex.getStatus() == IloAlgorithm::Optimal) {
                sol.status = "Optimal";
                out << "optimal" << '\n';
                out << cplex.getObjValue() << '\n';
                sol.obj = cplex.getObjValue(); \
            }
            else if (cplex.getStatus() == IloAlgorithm::Feasible) {

                sol.status = "Feasible";
                /* for (int i = 0; i < s.getSize(); i++)
                     sol.st.push_back(cplex.getValue(s[i]));
                 sol.obj = cplex.getObjValue();*/
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