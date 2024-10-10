#include "Instance.h"
#include "Params.h"
#include "runner.h"
#include "pardo.h"
#include "pdstsp.h"
#include "pdstsp_dr.h"
int main(int argc, char* argv[]) {
    std::string instance_folder = "/home/orlab/repo/quy/conflict_job/set_K/";
    std::string model_name = "TIF";
    std::string instance_name = "";
    std::string param_instance_name = "";
    int prob_id = 1;

    while (--argc) {
        argv++;
        std::string s(argv[0]);
        /*if (s == "-model") {
            model_name = std::string(argv[1]);
        }
        if (s == "-prob_id") {
            prob_id = stoi(std::string(argv[1]));
        } */
        if (s == "-instanceDir") {
            instance_folder = std::string(argv[1]);
        }
        if (s == "-instanceName") {
            instance_name = std::string(argv[1]);
        }
        if (s == "-paramName") {
            param_instance_name = std::string(argv[1]);
        }
    }

    //Resupply -instanceDir C:/Users/admin/Downloads/Instances -instanceName R_10_50_1.txt -paramName param_instance.txt
    /*Param* param = Param::getInstance(instance_name, prob_id);*/
    std::string ins_path = instance_folder + "/" + instance_name;
    std::cout << ins_path << std::endl;

    std::string param_path = instance_folder + "/" + param_instance_name;
    std::cout << param_path << std::endl;
    Instance* instance = Instance::getInstance(ins_path, param_path);


    Model* model = nullptr;
    //model = new Pardo();
    //model = new PDSTSP();
    
    model = new PDSTSP_DR();
    //model->run();
    Runner runner(model);
    runner.run(std::ofstream("C:/Users/admin/Downloads/Instances/Sol"));




    //if (prob_id == 1)
    //    param->setHorizonRatio(2.0 / (3 * instance->getNbMachine()));
    //else
    //    param->setHorizonRatio(1);

    //Runner runner(model);

    //if (model_name.find("BS") != std::string::npos) {
    //    param->setGap(0.15);
    //    runner.run(std::ofstream("/home/orlab/repo/quy/conflict_job/sol/P" + std::to_string(prob_id) + "_15/" + model_name + "_" + instance_name));
    //}
    //else if (prob_id == 1)
    //    runner.run(std::ofstream("/home/orlab/repo/quy/conflict_job/sol/P" + std::to_string(prob_id) + "/2_3/" + model_name + "_" + instance_name));
    //else
    //    //runner.run(std::ofstream("/home/orlab/repo/quy/conflict_job/sol/P" + std::to_string(prob_id) + "/"+ model_name + "_" + instance_name));
    //    runner.run(std::ofstream("/home/orlab/repo/quy/conflict_job/sol/new_instance/" + model_name + "_" + instance_name));
    return 0;
}