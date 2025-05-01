#include "FastMCModel.h"
#include "ConfigureSimulation.h"
#include "DataLoader.h"
#include "ResolutionPlotter.h"
#include "SimulationFrameworks.h"
#include "TrainingInfo.h"
#include <TMath.h>
#include <iostream>

using namespace std;
FastMCModel::FastMCModel(const std::string &outputDir)
    : outputDir_(outputDir) {
  inputHandler =
      std::make_unique<InputHandler>("../source/core/paths_AI_model.txt");
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
  auto info = TrainingInfo(particle_, modelDir_+ "training.root");

  ConfigureSimulation config;
  gBenchmark = new TBenchmark(); // this is required since root interactive

  config.Load(trainingDir_);
  /// the directory where you want to put your output
  config.Simulate(outputDir_);
  config.SetParticleName(particle_);
  config.UsePid(simParticle_);
  config.SetTrainingDir(trainingDir_); // e.g. "training_output/"

  std::cout << "Resolution directory is: " << config.ResolutionDir()<< std::endl;
  DataLoader dl("tree", inputRootFile_);
  std::cout << "data loaded into the loader" << std::endl;
  dl.SimVars(info.variables);
  std::cout << "Checked sim variables" << std::endl;
  dl.SetFractionToProcess(1);
  std::cout << "Fractions set" << std::endl;
  SimWithKerasAccDTRes(config, dl);
  std::cout << "Keras acceptance done" << std::endl;

  std::vector<Double_t> diffRanges = {0.1,  0.005, 0.05,  0.1, 0.08,
                                      0.03, 0.5,   0.003, 0.03};
  //ResolutionPlotter(dl, config, 1700, 1500, diffRanges).PlotSimulation();
  fDvcsPlotter = std::make_shared<DVCSplotter>(dl, config, outputDir_);
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
