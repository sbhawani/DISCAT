#include "FastMCModel.h"

#include <TMath.h>

#include <iostream>

#include "ConfigureSimulation.h"
#include "DataLoader.h"
#include "ResolutionPlotter.h"
#include "SimulationFrameworks.h"
#include "TrainingInfo.h"

using namespace std;
FastMCModel::FastMCModel(const std::string &outputDir) : outputDir_(outputDir) {
  inputHandler = std::make_unique<InputHandler>("../source/core/paths_AI_model.txt");
  try {
    std::string fullInputPath = inputHandler->getMappedInputPath();
    std::cout << "Mapped input path: " << fullInputPath << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return;
  }
}

void FastMCModel::run() {
  std::cout << "input model directory is " << modelDir_ << std::endl;
  auto info = TrainingInfo(particle_, modelDir_ + "training.root");

  ConfigureSimulation config;
  gBenchmark = new TBenchmark();  // this is required since root interactive

  config.Load(trainingDir_);
  /// the directory where you want to put your output
  config.Simulate(outputDir_);
  config.SetParticleName(particle_);
  config.UsePid(simParticle_);
  config.SetTrainingDir(trainingDir_);  // e.g. "training_output/"

  std::cout << "Resolution directory is: " << config.ResolutionDir() << std::endl;
  //inputRootFile_ = "/w/hallb-scshelf2102/clas12/singh/HighLevelAN/Generators/dvcs_simulation/vgg_test.root"; // this is input for FastMC
  std::cout<<" picking up file from the eventgen dir while defualt issue = "<< inputRootFile_<<std::endl;
  gBenchmark = new TBenchmark();
  DataLoader dl("tree", inputRootFile_);
  //DataLoader  dl("tree", "/w/hallb-scshelf2102/clas12/singh/HighLevelAN/Generators/dvcs_simulation/vgg_test.root"); /// this is only for the test with the event generat
  dl.SimVars(info.variables);
  dl.SetFractionToProcess(1);
  SimWithKerasAccDTRes(config, dl);
  std::vector<Double_t> diffRanges = {0.1, 0.005, 0.05, 0.1, 0.08, 0.03, 0.5, 0.003, 0.03};
  // ResolutionPlotter(dl, config, 1700, 1500, diffRanges).PlotSimulation();
  fDvcsPlotter = std::make_shared<DVCSplotter>(dl, config, false, outputDir_);
  fDvcsPlotter->SetRangesKinPlots(diffRanges);
  fDvcsPlotter->LoadDataFrames();
  BinManager xBins;
  xBins.SetQ2Bins({1.0, 2.5, 5.0});
  xBins.SetTBins({0.1, 0.5, 1.0});
  xBins.SetXBBins({0.1, 0.3, 0.5});
  if (fCreateKinPlot) {
    fDvcsPlotter->plotKinematics(true);
    fDvcsPlotter->SetXBinsRanges(xBins);
    fDvcsPlotter->plotDVCSVars();
    fDvcsPlotter->plotDVCSX();
    fDvcsPlotter->saveAll("fastMC_2.root");
  } else {
    std::cout << "Model ran but kin plots are not created\n";
  }
}

std::string FastMCModel::getOutputPath() { return outputDir_; }
