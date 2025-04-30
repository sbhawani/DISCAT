#include "FastMCModel.h"
#include "ConfigureSimulation.h"
#include "DataLoader.h"
#include "ResolutionPlotter.h"
#include "SimulationFrameworks.h"
#include "TrainingInfo.h"
#include <TMath.h>
#include <iostream>

using namespace std;
FastMCModel::FastMCModel(const std::string &particle,
                         const std::string &simParticle,
                         const std::string &trainingDir,
                         const std::string &predictionDir,
                         const std::string &outputDir,
                         const std::string &inputRootFile)
    : particle_(particle), simParticle_(simParticle), trainingDir_(trainingDir),
      predictionDir_(predictionDir), outputDir_(outputDir),
      inputRootFile_(inputRootFile) {}

void FastMCModel::run() {

  std::cout << "input directory is " << trainingDir_ << std::endl;
  auto info = TrainingInfo(particle_, trainingDir_ + "training.root");
  ConfigureSimulation config;

  gBenchmark = new TBenchmark(); // this is required since root interactive

  config.Load(predictionDir_);
  /// the directory where you want to put your output
  config.Simulate(outputDir_);
  config.SetParticleName(particle_);
  config.UsePid(simParticle_);
  config.SetTrainingDir(predictionDir_); // e.g. "training_output/"
  // sets current_pdg -> "all"

  std::cout << "Resolution directory is: " << config.ResolutionDir()
            << std::endl;
  std::cout << "predictionDir_: " << predictionDir_ << std::endl;

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
  BinManager xBins;
  xBins.SetQ2Bins({1.0, 2.5, 5.0});
  xBins.SetTBins({0.1, 0.5, 1.0});
  xBins.SetXBBins({0.1, 0.3, 0.5});
  if (fCreateKinPlot) {
    fDvcsPlotter->plotKinematics();
    fDvcsPlotter->SetXBinsRanges(xBins);
    fDvcsPlotter->plotDVCSVars();
    fDvcsPlotter->plotDVCSX();
    fDvcsPlotter->saveAll("fastMC_2.root");
  } else {
    std::cout << "Model ran but kin plots are not created\n";
  }
}

std::string FastMCModel::getOutputPath() { return outputDir_; }
