#include <iostream>

#include "./../core/DISCAT.h"
#include "./../core/InputHandler.h"

int RunDVCSmodel() {

  DISCAT discat;
  std::string output_dir = "/w/hallb-scshelf2102/clas12/singh/HighLevelAN/FastMC/DISCAT_output/test/pred_output/";

  std::cout << "Creating VGG model!\n";
  FastMCModel *modelVGG = new FastMCModel(output_dir);
  modelVGG->configModel("A","2018","spring","inbending","0nA");
  std::cout << "VGG model created!\n";
  modelVGG->createKinematicsPlots();
  std::cout << "Asking for plots\n";
  discat.addModel(modelVGG);
  // discat.add("pi+", "pi+", "models/pi_train/", "models/pi_pred/",
  // "results/pi/");

  discat.run();

  std::cout << "All models finished running." << std::endl;
  return 0;
}
