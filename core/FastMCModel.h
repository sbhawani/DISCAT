#ifndef FASTMCMODEL_H
#define FASTMCMODEL_H

#include <pybind11/embed.h>

#include <iostream>
#include <string>
#include <unordered_map>

#include "DVCSplotter.h"
#include "DataConverter.h"
#include "InputHandler.h"
#include "DVCSEventGen.h"


class FastMCModel {
 public:
  FastMCModel(const std::string& outputDir);
  void run();
  void createKinematicsPlots() { fCreateKinPlot = true; };
  std::string getOutputPath();

  // Setter for dynamic Python script arguments
  void useEventGen(const bool useEventgen) { fUseEventgen = useEventgen; }
  void setEvengGenPars(const std::string& key, const std::string& value) { config_[key] = value; }
  void setOutputEventGen(const string outputfile) { outputfile_evntgen = outputfile; }

  /////
  void configFastMC(const std::string& runGroupName = "A", const std::string& setYear = "2018", const std::string& runSeason = "spring", const std::string& magnetPolarity = "inbending", const std::string& backgroundMerge = " ") {
    inputHandler->setRunGroup(runGroupName);
    inputHandler->setYear(setYear);
    inputHandler->setSeason(runSeason);
    inputHandler->setMagnetPolarity(magnetPolarity);
    inputHandler->setBackgroundMerge(backgroundMerge);
    std::string fullInputPath = inputHandler->getMappedInputPath();

    ////// in case using the direct input from the event gen for the studies
    if (fUseEventgen) {
        fEventGen = std::make_unique<DVCSEventGen>();
        fEventGen->run(config_);  
    }

    modelDir_ = fullInputPath;
    predictionDir_ = modelDir_ + "pred_output/";
    trainingDir_ = modelDir_ + "training_output/";
    particle_ = "all";
    simParticle_ = "all";

    if (fUseEventgen) {
      std::string txtfile_from_eventgen = config_["fname"];
      std::string converted_output_evtgen = txtfile_from_eventgen;
      DataConverter convert_to_root(txtfile_from_eventgen, converted_output_evtgen);
      convert_to_root.processFile();
      inputRootFile_ = converted_output_evtgen + ".root";  // this is input for FastMC
    } else {
      inputRootFile_ = modelDir_ + "all_events_test_test.root";
    }
  }

 private:
  bool fCreateKinPlot = false;
  bool fUseEventgen = false;
  std::unique_ptr<DVCSEventGen> fEventGen;
  std::shared_ptr<DVCSplotter> fDvcsPlotter;
  std::unique_ptr<InputHandler> inputHandler;
  std::string particle_;
  std::string simParticle_;
  std::string modelDir_;
  std::string trainingDir_;
  std::string predictionDir_;
  std::string outputDir_;
  std::string inputRootFile_;
  std::string outputfile_evntgen;
  std::unordered_map<std::string, std::string> config_ = {{"model", "km15"}, {"nentries", "10"}, {"fname", "output_file"}, {"beam", "10.604"}, {"xBmin", "0.05"},  {"xBmax", "0.75"}, {"Q2min", "1.0"}, {"Q2max", "15.0"},  {"tmin", "0.0"},
  {"tmax", "1.0"},  {"ymin", "0.05"},   {"ymax", "0.9"},  {"w2min", "4.0"},  {"radgen", "true"}, {"seed", "1234"},  {"bin", "0"}, {"fringe", "false"}};
};

#endif
