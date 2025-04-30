#include "BinManager.h"

BinManager::BinManager() {
    // Default binning (can be overridden using setters)
    q2_bins_ = {1.0, 2.0, 4.0, 6.0};
    t_bins_  = {0.1, 0.3, 0.6, 1.0};
    xb_bins_ = {0.1, 0.2, 0.4, 0.6};
}
