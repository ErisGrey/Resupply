
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
class PDSTSP : public Model {
private:
    //Variable
    IloEnv env;
    NumVar3D x;
    NumVar3D h;
    IloNumVarArray z;
    IloNumVarArray a;


    //Paramter
    int numNode;
    int numUAV;
    std::vector<int> N;
    std::vector<int> C;
    std::vector<int> truckOnly;
    std::vector<int> freeCustomers;
    std::vector<int> freeCustomers0;
    std::vector<int> arr;
    std::vector<int> a_inv;
    std::vector<int> new_freeCustomers;
public:
    PDSTSP() {
        Instance* instance = Instance::getInstance();
        numNode = instance->num_nodes - 1; //not include depot (n + 1)
        numUAV = 1;
        std::cout << "num_nodes = " << numNode << '\n';

        /*  SET -------------------------------- */
        for (int i = 0; i < instance->num_nodes - 1; ++i)
            N.push_back(i);
        for (int i = 1; i < instance->num_nodes - 1; ++i)
            C.push_back(i);
        truckOnly = instance->truckonly;
        freeCustomers = instance->freemode;
        freeCustomers.pop_back();
        freeCustomers0 = instance->freemode;
        freeCustomers0.insert(freeCustomers0.begin(), 0);
        freeCustomers0.pop_back();
        
        for (int i : freeCustomers0)
        {
            std::cout << i << " ";
        }
        std::cout << '\n';

        arr.resize(instance->num_nodes, -1);
        arr[0] = 0;
        int tg1;
        for (int i = 1; i < instance->num_nodes; i++)
        {
            arr[i] = i;
        }
        for (int i = 1; i < instance->num_nodes; i++)
        {
            if (instance->tdrone_all(i) > 1000) continue;
            for (int j = i + 1; j < instance->num_nodes; j++) {
                if (instance->tdrone_all(j) > 1000) continue;
                if (instance->tdrone_all(arr[i]) < instance->tdrone_all(arr[j])) {
                    tg1 = arr[i];
                    arr[i] = arr[j];
                    arr[j] = tg1;
                }
            }
        }

        for (int i = 1; i < instance->num_nodes; i++)
        {
            std::cout << arr[i] << std::endl;
        }

        a_inv.resize(numNode, 0);

        // Populate the inverse vector
        for (int i = 0; i < numNode; ++i) {
            a_inv[arr[i]] = i;
        }

        // Output the inverse vector
        std::cout << "a^-1 = [ ";
        for (int i = 0; i < numNode; ++i) {
            std::cout << a_inv[i] << " ";
        }
        std::cout << "]" << std::endl;

        for (int j : freeCustomers)
        {
            new_freeCustomers.push_back(a_inv[j]);
        }

        env = IloEnv();
        x = NumVar3D(env, numNode); // x_ijk
        h = NumVar3D(env, numNode); // h_ijk
        a = IloNumVarArray(env, numNode);
        z = IloNumVarArray(env, numNode);



        std::stringstream name;
        // x_ijk
        for (int i : N)
        {
            x[i] = NumVar2D(env, numNode);
            for (int j : N) {
                if (j == i) continue;
                x[i][j] = IloNumVarArray(env, numNode);
                for (int k : N) {
                    name << "x." << i << "." << j << "." << k;
                    x[i][j][k] = IloNumVar(env, 0, 1, ILOFLOAT, name.str().c_str());
                    name.str("");
                }
            }
        }


        // h_ijk
        for (int i : N)
        {
            h[i] = NumVar2D(env, numNode);
            for (int j : N) {
                if (j == i) continue;
                h[i][j] = IloNumVarArray(env, numNode);
                for (int k : N) {
                    name << "h." << i << "." << j << "." << k;
                    h[i][j][k] = IloNumVar(env, 0, IloInfinity, ILOFLOAT, name.str().c_str());
                    name.str("");
                }
            }
        }

        //a_i
        for (int i : new_freeCustomers)
        {
            name << "a." << i;
            a[i] = IloNumVar(env, 0, IloInfinity, ILOFLOAT, name.str().c_str());
            name.str("");
        }

        //z_i
        for (int i : C)
        {
            name << "z." << i;
            z[i] = IloNumVar(env, 0, 1, ILOFLOAT, name.str().c_str());
            name.str("");

        }

    }
    IloModel createModel() {
        Instance* instance = Instance::getInstance();
        Param* param = Param::getInstance();
        IloModel model(env);
        IloExpr exprSolution(env);
        for (int k : N) {
            for (int i : N) {
                for (int j : N) {
                    if (j == i) continue;
                    exprSolution += h[i][j][k] * instance->time_truck[arr[i]][arr[j]];
                }
            }
        }

        for (int i : new_freeCustomers)
        {
            exprSolution += a[i] * instance->tdrone_all(arr[i]);
        }

        for (int i : new_freeCustomers)
        {
            exprSolution -= (1 - z[i]) * instance->time_drone[0][arr[i]];
        }

        model.add(IloMinimize(env, exprSolution));
        // CONSTRAINT -------------------------------------
        //Truck routing contraints
        for (int j : truckOnly) {
            model.add(z[a_inv[j]] == 1);
        }
        //truck
        for (int i : C)
        {
            for (int k : C)
            {
                IloExpr condij(env);
                for (int j : N)
                {
                    if (j == i) continue;
                    condij += x[i][j][k] - x[j][i][k - 1];
                }
                model.add(condij == 0);
                condij.end();
            }
        }

        for (int i : C) {
            for (int j : C) {
                if (i == j) continue;
                model.add(x[i][j][0] == 0);
            }
        }

        for (int i : C)
        {
            model.add(x[i][0][0] == 0);
        }



        IloExpr sums(env);
        for (int j : C) {
            sums += x[0][j][0];
        }
        model.add(sums == 1);
        sums.end();


        IloExpr sume(env);
        for (int i : C) {
            for (int k : C) {
                sume += x[i][0][k];

            }
        }
        model.add(sume == 1);
        sume.end();


        for (int k : C) {
            for (int i : C) {
                model.add(x[0][i][k] == 0);
            }
        }



        for (int j : C)
        {
            IloExpr sumj(env);
            for (int i : N) {
                if (i == j) continue;
                for (int k : N) {
                    sumj += x[i][j][k];
                }
            }
            model.add(sumj - z[j] == 0);
            sumj.end();
        }



        for (int i : C)
        {
            IloExpr sumi(env);
            for (int j : N) {
                if (i == j) continue;
                for (int k : N) {
                    sumi += x[i][j][k];
                }
            }
            model.add(sumi - z[i] == 0);
            sumi.end();
        }


        for (int k : N)
        {
            IloExpr condik(env);
            for (int i : C)
            {
                condik += z[i];
            }

            IloExpr condiij(env);
            for (int i : N)
            {
                for (int j : N)
                {
                    if (i == j) continue;
                    condiij += x[i][j][k];
                }
            }

            model.add((condik - k) * 1.0 / (numNode - 1) <= condiij);
            condik.end();
            condiij.end();
        }




        for (int i : N)
        {
            for (int j : C)
            {
                if (i == j) continue;
                for (int k : N)
                {
                    model.add(x[i][j][k] <= h[i][j][k]);
                    model.add(h[i][j][k] <= x[i][j][k] * (numNode - k));
                }
            }
        }

        for (int i : C)
        {
            for (int k : N)
            {
                model.add(h[i][0][k] == 0);
            }
        }


        IloExpr numEd(env);
        for (int i : C)
        {
            numEd += z[i];
        }
        IloExpr condi1(env);
        for (int i : N)
        {

            for (int j : C)
            {
                if (i == j) continue;
                condi1 += h[i][j][0];
            }
        }
        model.add(condi1 == numEd);
        condi1.end();
        numEd.end();


        for (int k : C)
        {
            IloExpr condi1(env);
            IloExpr condi2(env);
            IloExpr condi3(env);
            IloExpr numEd(env);
            for (int i : C)
            {
                numEd += z[i];
            }

            for (int i : N)
            {
                for (int j : C)
                {
                    if (i == j) continue;
                    condi1 += h[i][j][k];
                }
            }

            for (int i : N)
            {
                for (int j : C)
                {
                    if (i == j) continue;
                    condi3 += h[i][j][k - 1];
                }
            }

            model.add(condi1 >= condi3 - 1);
            condi1.end();
            condi2.end();
            condi3.end();
            numEd.end();
        }


        //drone
        if (numUAV >= 1)
        {
            for (int j : new_freeCustomers)
            {

                model.add(1 - z[j] <= a[j]);
                model.add(a[j] <= (1 + j * 1.0 / numUAV) * (1 - z[j]));
            }

            for (int j : new_freeCustomers)
            {
                IloExpr condij(env);
                for (int i = 1; i <= j; i++)
                {
                    condij += z[i];
                }
                model.add((j - condij) * 1.0 / numUAV - (j * 1.0 / numUAV) * z[j] <= a[j]);

                condij.end();
            }
        }

        else
        {
            for (int i : C)
            {
                model.add(z[i] == 1);
            }
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
        std::cout << "have solve??" << '\n';
        if (cplex.solve()) {
            std::cout << "solve 0" << '\n';
            auto end_time = std::chrono::high_resolution_clock::now();
            sol.time = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();

            if (cplex.getStatus() == IloAlgorithm::Optimal) {
                sol.status = "Optimal";
                std::cout << "optimal" << '\n';
                std::cout << cplex.getObjValue() << '\n';
                sol.obj = cplex.getObjValue();
                /*for (int i = 0; i < s.getSize(); i++)
                    sol.st.push_back(cplex.getValue(s[i]));
                sol.obj = cplex.getObjValue();*/
            }
            else if (cplex.getStatus() == IloAlgorithm::Feasible) {

                sol.status = "Feasible";
                /*for (int i = 0; i < s.getSize(); i++)
                    sol.st.push_back(cplex.getValue(s[i]));*/
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