#include "DISCAT.h"

///////////////////
void DISCAT::addModel(FastMCModel* model) {
    fModels.emplace_back(std::shared_ptr<FastMCModel>(model));
}
///////////////////
void DISCAT::run() {
    for (const auto& model : fModels) {
        model->run();
    }
}
