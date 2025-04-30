#ifndef DVCS_PLOTTER_H
#define DVCS_PLOTTER_H

#include "ConfigureSimulation.h"
#include "DataLoader.h"
#include "BinManager.h"
#include "TH1D.h"
#include <string>
#include <optional>

class DVCSplotter {
  using varnames_t = std::vector<std::string>;
  using datavars_t = std::vector<DataVariable>;
  using dfth1ds_t = std::vector<ROOT::RDF::RResultPtr<TH1D>>;
  using dfth2ds_t = std::vector<ROOT::RDF::RResultPtr<TH2D>>;
  using names_t = std::vector<std::string>;

public:
  DVCSplotter();
  DVCSplotter(DataLoader &loader, ConfigureSimulation &conf,
              const std::string &outputDir);

  void plotKinematics();
  void plotDVCSVars();
  void plotDVCSX();
  void saveAll(const char *outFileName);

  /// settters
  void SetRangesKinPlots(std::vector<Double_t> Ranges) { fDiffRanges = Ranges; }
  void SetXBinsRanges(BinManager bins) { fXbins = bins; }
  void SetXLuminosity(double lum) { fLuminosity = lum; }
private:
  void fillHistogramsRDF();
  DataLoader *fLoader = nullptr;
  ConfigureSimulation *fConf = nullptr;
  
  BinManager fXbins;

  TCanvas *fCanvas = nullptr;
  std::shared_ptr<ROOT::RDF::RNode> fAccEvents; // this is shaddy though
  std::optional<ROOT::RDF::RNode> fComputed_df; // this is shaddy though

  //

  bool fFastOnly = false;

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
  Double_t fCanvasWidth{700};
  Double_t fCanvasHeight{500};
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

  static constexpr double E_beam = 10.6;  // GeV
  static constexpr double m_e = 0.000511; // GeV
  static constexpr double m_p = 0.938;    // GeV
  double fLuminosity = 1;
};

#endif
