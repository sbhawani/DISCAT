#include <iostream>

#include "./../core/DISCAT.h"

int RunDVCSmodel()
{
    DISCAT discat;
    std::string input_dir = "/w/hallb-scshelf2102/clas12/singh/HighLevelAN/FastMC/Fast_MC_Models/rg_A_2018/spring/inbending/0nA/";
    // std::string output_dir ="/w/hallb-scshelf2102/clas12/singh/HighLevelAN/FastMC/Fast_MC_Models/rg_A_2018/spring/inbending/0nA/";
    //std::string output_dir = "/w/hallb-scshelf2102/clas12/singh/HighLevelAN/FastMC/DISCAT_output/test/";
    std::string output_dir = "/w/hallb-scshelf2102/clas12/singh/HighLevelAN/FastMC/DISCAT_output/test/pred_output/";

    std::cout << "Creating VGG model!\n";
    FastMCModel* modelVGG = new FastMCModel("all", "all", input_dir, input_dir + "training_output/", output_dir + "pred_output/", input_dir + "all_events_test_test.root");
    std::cout << "VGG model created!\n";
    modelVGG->createKinematicsPlots();
    std::cout << "Asking for plots\n";
    discat.addModel(modelVGG);
    // discat.add("pi+", "pi+", "models/pi_train/", "models/pi_pred/", "results/pi/");

    discat.run();

    std::cout << "All models finished running." << std::endl;
    return 0;
}
