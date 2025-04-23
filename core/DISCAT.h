#ifndef DISCAT_H
#define DISCAT_H

#include <vector>
#include <memory>
#include <string>
#include "FastMCModel.h"

class DISCAT {
public:
    void add(const std::string& particle,
             const std::string& simParticle,
             const std::string& trainingDir,
             const std::string& predictionDir,
             const std::string& outputDir,
             const std::string& inputRootFile = "all_events_test_test.root");

    void run(); // runs all models

private:
    std::vector<std::shared_ptr<FastMCModel>> models_;
};

#endif
