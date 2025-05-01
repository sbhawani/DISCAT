#ifndef FASTMCMODEL_H
#define FASTMCMODEL_H

#include <string>

#include "DVCSplotter.h"
#include "InputHandler.h"

class FastMCModel
{
   public:
    FastMCModel(const std::string& outputDir);

    void run();
    void createKinematicsPlots() { fCreateKinPlot = true; };
    std::string getOutputPath();
    void configModel(const std::string& runGroupName = "A",const std::string& setYear = "2018", const std::string& runSeason = "spring",const std::string& magnetPolarity = "inbending", const std::string& backgroundMerge = " "){
        inputHandler->setRunGroup(runGroupName);
        inputHandler->setYear(setYear);
        inputHandler->setSeason(runSeason);
        inputHandler->setMagnetPolarity(magnetPolarity);
        inputHandler->setBackgroundMerge(backgroundMerge);
        //// safety in case if path is wrong you wont have erronious output
        std::string fullInputPath = inputHandler->getMappedInputPath();
       
        modelDir_ = fullInputPath;
        predictionDir_ = modelDir_ + "pred_output/";
        trainingDir_ = modelDir_ +"training_output/";
        particle_ = "all";
        simParticle_ = "all";
        inputRootFile_= modelDir_+"all_events_test_test.root";
    }

   private:
    bool fCreateKinPlot = false;
    std::shared_ptr<DVCSplotter> fDvcsPlotter;
    std::unique_ptr<InputHandler> inputHandler;
    std::string particle_;
    std::string simParticle_;
    std::string modelDir_;
    std::string trainingDir_;
    std::string predictionDir_;
    std::string outputDir_;
    std::string inputRootFile_;

    /// set up for
};

#endif
