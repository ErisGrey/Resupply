#include "Instance.h"
#include "Params.h"
#include "runner.h"

int main(int argc, char* argv[]) {
   std::string instance_folder = "/home/orlab/repo/quy/conflict_job/set_K/";
    std::string model_name = "TIF";
    std::string instance_name = "";
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
    }

    /*Param* param = Param::getInstance(instance_name, prob_id);*/
    Instance* instance = Instance::getInstance(instance_folder + "/" + instance_name);
    Model* model = nullptr;
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
