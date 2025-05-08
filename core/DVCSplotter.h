#ifndef DVCS_PLOTTER_H
#define DVCS_PLOTTER_H

#include <optional>
#include <string>

#include "BinManager.h"
#include "ConfigureSimulation.h"
#include "DISCATMath.h"
#include "DataLoader.h"
#include "TH1D.h"

class DVCSplotter {
  using varnames_t = std::vector<std::string>;
  using datavars_t = std::vector<DataVariable>;
  using dfth1ds_t = std::vector<ROOT::RDF::RResultPtr<TH1D>>;
  using dfth2ds_t = std::vector<ROOT::RDF::RResultPtr<TH2D>>;
  using names_t = std::vector<std::string>;

 public:
  DVCSplotter();

  DVCSplotter(DataLoader &loader, ConfigureSimulation &conf, bool AccOnly, const std::string &outputDir);
  ~DVCSplotter();
  void LoadDataFrames();
  void plotKinematics(bool plotFastOnly);
  void plotDVCSVars();
  void plotDVCSX();
  void saveAll(const char *outFileName);

  /// settters
  void SetRangesKinPlots(const std::vector<Double_t> &diffRanges) {
    fDiffRanges = diffRanges;
    std::cout << "SetRangesKinPlots called. fDiffRanges size: " << fDiffRanges.size() << "and " << diffRanges.size() << std::endl;
  }
  void SetXBinsRanges(BinManager bins) { fXbins = bins; }
  void SetXLuminosity(double lum) { fLuminosity = lum; }
  template <typename Method>
  ROOT::RDF::RNode define_DISCAT(const ROOT::RDF::RNode &df, const std::string &name, const Method method) {
    return const_cast<ROOT::RDF::RNode &>(df).Define(name,
                                                     [this, method](double recel_p, double recel_theta, double recel_phi, double recp1_p, double recp1_theta, double recp1_phi, double recp2_p, double recp2_theta, double recp2_phi) {
                                                       return (DISCATMath(beam_energy, recel_p, recel_theta, recel_phi, recp1_p, recp1_theta, recp1_phi, recp2_p, recp2_theta, recp2_phi).*method)();
                                                     },
                                                     {"recon__recel_p", "recon__recel_theta", "recon__recel_phi", "recon__recp1_p", "recon__recp1_theta", "recon__recp1_phi", "recon__recp2_p", "recon__recp2_theta", "recon__recp2_phi"});
  }

 private:
  double beam_energy = 10.6;
  void fillHistogramsRDF();
  DataLoader *fLoader = nullptr;
  ConfigureSimulation *fConf = nullptr;

  BinManager fXbins;

  TCanvas *fCanvas = nullptr;
  std::shared_ptr<ROOT::RDF::RNode> fAccEvents;  // this is shaddy though
  std::optional<ROOT::RDF::RNode> fComputed_df;  // this is shaddy though

  //

  bool fFastOnly = false;
  bool fAccOnly = false;  /// if you want to have both generated and accepted particles

  datavars_t fVars;
  std::string fSaveDir;
  names_t fFullFastVars;
  names_t fFullOrigVars;
  names_t fDiffFastVars;
  names_t fDiffOrigVars;
  dfth1ds_t fVhistFullFast;
  dfth1ds_t fVhistFullOrig;
  dfth1ds_t fVhistDiffFast;
  dfth1ds_t fVhistDiffOrig;

  dfth2ds_t fVhist2DDiffFast;
  dfth2ds_t fVhist2DDiffOrig;
  Double_t fCanvasWidth{1700};
  Double_t fCanvasHeight{1500};
  vector<Double_t> fDiffRanges;
  // Q2 vs xB histograms for binning
  ROOT::RDF::RResultPtr<TH2D> fHistQ2_vs_xB;
  // Electron
  TH1D *h_ep, *h_eth, *h_ephi;
  // Proton
  TH1D *h_pp, *h_pth, *h_pphi;
  // Photon
  TH1D *h_gp, *h_gth, *h_gphi;
  // DVCS
  dfth1ds_t fDVCSVarFast;

  static constexpr double E_beam = 10.6;   // GeV
  static constexpr double m_e = 0.000511;  // GeV
  static constexpr double m_p = 0.938;     // GeV
  double fLuminosity = 1;
};

#endif
