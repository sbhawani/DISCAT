#include <iostream>
#include <chrono>

#include "./../core/DISCAT.h"
#include "./../core/InputHandler.h"

int RunDVCSmodel() {


  /// Define event generator here
  //std::unique_ptr<DVCSEventGen> fEventGen = std::make_unique<DVCSEventGen>();


  DISCAT discat;
  std::string output_dir = "/w/hallb-scshelf2102/clas12/singh/HighLevelAN/FastMC/DISCAT_output/test/pred_output_2/";

  std::cout << "Creating VGG model!\n";
  FastMCModel *modelVGG = new FastMCModel(output_dir);
  //// if you use the eventgen as input 
  // to set Event generator parameters and settings, 
  // it creates a directory in your output directory where it stores the outputs of eventgen in txt and root files
  //  then it reads those files for FastMC operations
  modelVGG ->useEventGen(true);
  modelVGG ->setEvengGenPars("model", "vgg");
  modelVGG ->setEvengGenPars("beam", "10.594");
  modelVGG ->setEvengGenPars("nentries", "600000");
  modelVGG ->setEvengGenPars("fname", "vgg_test");
  modelVGG ->setEvengGenPars("radgen", "false");
  //// to set detector Fast MC module from the Nerual Networks trainings  

  modelVGG->configFastMC("A","2018","spring","inbending","0nA");
  modelVGG->createKinematicsPlots();
  discat.addModel(modelVGG);
  discat.run();

  std::cout << "All models finished running." << std::endl;
  return 0;
}
