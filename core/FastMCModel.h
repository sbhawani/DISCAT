#ifndef FASTMCMODEL_H
#define FASTMCMODEL_H

#include <string>

#include "DVCSplotter.h"

class FastMCModel
{
   public:
    FastMCModel(const std::string& particle, const std::string& simParticle, const std::string& trainingDir, const std::string& predictionDir, const std::string& outputDir, const std::string& inputRootFile);

    void run();
    void createKinematicsPlots() { fCreateKinPlot = true; };
    std::string getOutputPath();

   private:
    bool fCreateKinPlot = false;
    std::shared_ptr<DVCSplotter> fDvcsPlotter;
    std::string particle_;
    std::string simParticle_;
    std::string trainingDir_;
    std::string predictionDir_;
    std::string outputDir_;
    std::string inputRootFile_;

    /// set up for
};

#endif
