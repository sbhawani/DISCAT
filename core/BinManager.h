#ifndef BINMANAGER_H
#define BINMANAGER_H

#include <vector>

class BinManager {
public:
    BinManager();

    // Getters
    const std::vector<double>& GetQ2Bins() const { return q2_bins_; }
    const std::vector<double>& GetTBins() const { return t_bins_; }
    const std::vector<double>& GetXBBins() const { return xb_bins_; }

    // Setters
    void SetQ2Bins(const std::vector<double>& bins) { q2_bins_ = bins; }
    void SetTBins(const std::vector<double>& bins)  { t_bins_ = bins; }
    void SetXBBins(const std::vector<double>& bins) { xb_bins_ = bins; }

private:
    std::vector<double> q2_bins_;
    std::vector<double> t_bins_;
    std::vector<double> xb_bins_;
};

#endif // BINMANAGER_H
