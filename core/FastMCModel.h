#ifndef FASTMCMODEL_H
#define FASTMCMODEL_H

#include <string>

class FastMCModel {
public:
    FastMCModel(const std::string& particle,
                const std::string& simParticle,
                const std::string& trainingDir,
                const std::string& predictionDir,
                const std::string& outputDir,
                const std::string& inputRootFile);

    void run() const;
    std::string getOutputPath() const;

private:
    std::string particle_;
    std::string simParticle_;
    std::string trainingDir_;
    std::string predictionDir_;
    std::string outputDir_;
    std::string inputRootFile_;
};

#endif
