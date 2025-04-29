#ifndef DISCAT_H
#define DISCAT_H

#include <memory>
#include <string>
#include <vector>

#include "FastMCModel.h"

class DISCAT
{
   public:
    void addModel(FastMCModel* model);
    void run();  // runs all models

   private:
    std::vector<std::shared_ptr<FastMCModel>> fModels;
};

#endif
