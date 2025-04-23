#include "DISCAT.h"

void DISCAT::add(const std::string& particle,
                 const std::string& simParticle,
                 const std::string& trainingDir,
                 const std::string& predictionDir,
                 const std::string& outputDir,
                 const std::string& inputRootFile) {
    models_.emplace_back(std::make_shared<FastMCModel>(
        particle, simParticle, trainingDir, predictionDir, outputDir, inputRootFile));
}

void DISCAT::run() {
    for (const auto& model : models_) {
        model->run();
    }
}
