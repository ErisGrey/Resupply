#include "instance.h"
#include "params.h"
#include "runner.h"
#include "pardo.h"
#include "pdstsp.h"
#include "pdstsp_dr.h"
int main(int argc, char* argv[]) {
    std::string instance_folder = "/home/orlab/repo/quy/conflict_job/set_K/";
    std::string model_name = "TIF";
    std::string instance_name = "";
    std::string param_instance_name = "";
    std::string instance_mode = "";
    std::string model_mode = "";
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
        if (s == "-instanceMode") {
            instance_mode = std::string(argv[1]);
        }
        if (s == "-modelMode") {
            model_mode = std::string(argv[1]);
        }
    }

    //drone_resupply -instanceDir C:/Users/maiti/Downloads/TSPrd(time)/TSPrd(time)/Solomon/10 -instanceName C101_0.5.txt -paramName param_instance.txt -instanceMode solomon
    //drone_resupply -instanceDir C:/Users/maiti/Downloads/Resupply_ins -instanceName Instance_0.txt -paramName param_instance.txt -instanceMode solomon -modelMode PDSTSP_DR
    /*Param* param = Param::getInstance(instance_name, prob_id);*/

    std::string ins_path = instance_folder + "/" + instance_name;
    std::cout << ins_path << std::endl;

    std::string param_path = instance_folder + "/" + param_instance_name;
    std::cout << param_path << std::endl;
    Instance* instance = Instance::getInstance(ins_path, param_path, instance_mode);


    Model* model = nullptr;
    if (model_mode == "Pardo")
    {
        model = new Pardo();
        Runner runner(model);
        runner.run("C:/Users/maiti/Downloads/TSPrd(time)/TSPrd(time)/Solomon/10/Sol/run_log_pardo_float.csv");
    }
        
    if (model_mode == "PDSTSP_DR")
    {
        model = new PDSTSP_DR();
        Runner runner(model);
        runner.run("C:/Users/maiti/Downloads/TSPrd(time)/TSPrd(time)/Solomon/10/Sol/run_log_pdstsp_float.csv");
    }
    
       
    //model->run();


    //Model* model1 = nullptr;
    ////model = new Pardo();
    //model1 = new Pardo();
    ////model->run();
    //Runner runner(model1);


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