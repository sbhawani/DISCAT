#include "FastMCModel.h"
#include "TrainingInfo.h"
#include "DataLoader.h"
#include "ConfigureSimulation.h"
#include "SimulationFrameworks.h"
#include "ResolutionPlotter.h"
#include <TMath.h>
#include <iostream>

using namespace std;
FastMCModel::FastMCModel(const std::string& particle,
                         const std::string& simParticle,
                         const std::string& trainingDir,
                         const std::string& predictionDir,
                         const std::string& outputDir,
                         const std::string& inputRootFile)
    : particle_(particle), simParticle_(simParticle),
      trainingDir_(trainingDir), predictionDir_(predictionDir),
      outputDir_(outputDir), inputRootFile_(inputRootFile) {}

void FastMCModel::run() const {

    std::cout<<"Error: "<< trainingDir_ + "training.root"<<std::endl;
    auto info = TrainingInfo(particle_, trainingDir_+"training.root");

    ConfigureSimulation config;
   
    config.Load(predictionDir_);
 /// the directory where you want to put your output
    config.Simulate(outputDir_);
    config.SetParticleName(particle_);
    config.UsePid(simParticle_);
    config.SetTrainingDir(predictionDir_); // e.g. "training_output/"
    config.SetParticleName("all");    // sets current_pdg -> "all"


    DataLoader dl("tree", inputRootFile_);
    std::cout<<"data loaded into the loader"<<std::endl;
    dl.SimVars(info.variables);
    std::cout<<"Checked sim variables"<<std::endl;
    dl.SetFractionToProcess(1.0);
    std::cout<<"Fractions set"<<std::endl;
    SimWithKerasAccDTRes(config, dl);
    std::cout<<"Kera acceptance done"<<std::endl;
    std::vector<Double_t> diffRanges = {0.1, 0.005, 0.05, 0.1, 0.08, 0.03, 0.5, 0.003, 0.03};
    ResolutionPlotter(dl, config, 1700, 1500, diffRanges).PlotSimulation();
}

std::string FastMCModel::getOutputPath() const {
    return outputDir_;
}
