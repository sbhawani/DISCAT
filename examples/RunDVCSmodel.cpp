#include "./../core/DISCAT.h"
#include <iostream>

int RunDVCSmodel() {
    DISCAT discat;
    std::string input_dir ="/w/hallb-scshelf2102/clas12/singh/HighLevelAN/FastMC/Fast_MC_Models/rg_A_2018/spring/inbending/0nA/";
    discat.add("all","all",input_dir,input_dir+"training_output/",input_dir+"pred_output/", input_dir+"all_events_test_test.root");
    //discat.add("pi+", "pi+", "models/pi_train/", "models/pi_pred/", "results/pi/");

    discat.run();

    std::cout << "All models finished running." << std::endl;
    return 0;
}
