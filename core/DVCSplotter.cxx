#include "DVCSplotter.h"

#include <cmath>
#include <iostream>
#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TStyle.h"

DVCSplotter::DVCSplotter(DataLoader &loader, ConfigureSimulation &conf, bool AccOnly, const std::string &outputDir) : fLoader(&loader), fConf(&conf), fAccOnly(AccOnly), fSaveDir(outputDir) {
  if (!fLoader) {
    return;
  } else {
    std::cout << " *************************************" << std::endl;
    std::cout << " DVCS plotter is plotting the outputs " << std::endl;
    std::cout << " ************************************ " << std::endl;
  }
}

DVCSplotter::~DVCSplotter() {
  fVhistFullFast.clear();
  fVhistFullOrig.clear();
  fVhistDiffFast.clear();
  fVhistDiffOrig.clear();
  fVhist2DDiffFast.clear();
  fVhist2DDiffOrig.clear();
  fDVCSVarFast.clear();
  fDiffRanges.clear();
  if (fCanvas) {
    delete fCanvas;
    fCanvas = nullptr;
  }
  if (fAccEvents) {
    fAccEvents.reset();
  }
  if (fComputed_df) {
    fComputed_df.reset();
  }
  // Add other cleanup if needed (e.g., other ROOT-owned objects)
}

void DVCSplotter::LoadDataFrames() { this->fillHistogramsRDF(); }
// I used this part of the codes from macparticle libs since its same functions

void DVCSplotter::fillHistogramsRDF() {
  gSystem->Exec(Form("mkdir -p %s", fSaveDir.data()));
  names_t cachevars;
  /////////////========
  fVars = fLoader->GetDetailedVars();
  // Load all fast variables
  for (const auto &var : fVars) {
    auto fast_var = fLoader->CopyColumnFromTree("recon", fSaveDir + "all/predictions.root", var.rec_name);
    std::string fastDiffName = var.rec_name + "Diff";
    fLoader->DefineColumn(fastDiffName, var.acc_name + "-" + fast_var);
    std::string origDiffName = var.rec_name + "DiffReal";
    fLoader->DefineColumn(origDiffName, var.acc_name + "-" + var.rec_name);
    fFullFastVars.push_back(fast_var);
    cachevars.push_back(fast_var);
    fFullOrigVars.push_back(var.rec_name);
    cachevars.push_back(var.rec_name);
    fDiffFastVars.push_back(fastDiffName);
    cachevars.push_back(fastDiffName);
    fDiffOrigVars.push_back(origDiffName);
    cachevars.push_back(origDiffName);
    std::cout<<"fFullOrigVars = "<<var.rec_name<<"  fFullFastVars =  "<<fast_var<<std::endl;
  }
  fLoader->Cache(cachevars);
  //this works !!
  fAccEvents = std::make_shared<ROOT::RDF::RNode>(fLoader->GetAcceptedFrame());// accept_all fast mc prediction
  fComputed_df = std::make_optional<ROOT::RDF::RNode>(*fAccEvents);

  // compute DVCS variables 
  *fComputed_df = define_DISCAT(*fComputed_df, "Q2", &DISCATMath::GetQ2);
  *fComputed_df = define_DISCAT(*fComputed_df, "xB", &DISCATMath::GetxB);
  *fComputed_df = define_DISCAT(*fComputed_df, "t", &DISCATMath::GetT);
  *fComputed_df = define_DISCAT(*fComputed_df, "phi", &DISCATMath::GetPhi);
  *fComputed_df = define_DISCAT(*fComputed_df, "W", &DISCATMath::GetW);
  *fComputed_df = define_DISCAT(*fComputed_df, "nu", &DISCATMath::GetNu);
  *fComputed_df = define_DISCAT(*fComputed_df, "y", &DISCATMath::Gety);

  //// just to check the filtering
  //*fComputed_df = fComputed_df->Filter("abs(t) < 1.1");

  //// get the rest of the histograms
  auto h_Q2 = fComputed_df->Histo1D({"h_Q2", "Q^{2};Q^{2} [GeV^{2}];Counts", 100, 0, 10}, "Q2");
  auto h_xB = fComputed_df->Histo1D({"h_xB", "x_{B};x_{B};Counts", 100, 0, 1}, "xB");
  auto h_t = fComputed_df->Histo1D({"h_t", "t;|t| [GeV^{2}];Counts", 100, -2, 2}, "t");
  auto h_phi = fComputed_df->Histo1D({"h_phi", "phi;|phi| [rad];Counts", 100, 0, 360}, "phi");
  auto h_W = fComputed_df->Histo1D({"h_W", "W; W [GeV^{2}];Counts", 100, 0, 10}, "W");
  fHistQ2_vs_xB = fComputed_df->Histo2D({"fHistQ2_vs_xB", "Q^{2} vs x_{B};x_{B};Q^{2} [GeV^{2}]", 100, 0, 1, 100, 0, 10}, "xB", "Q2");

  fDVCSVarFast.push_back(h_Q2);
  fDVCSVarFast.push_back(h_xB);
  fDVCSVarFast.push_back(h_t);
  fDVCSVarFast.push_back(h_phi);
  fDVCSVarFast.push_back(h_W);

  ////////////////////
  std::vector<Double_t> valStdev;
  for (UInt_t i = 0; i < fVars.size(); ++i) {
    auto &var = fVars[i];
    auto stdev = fComputed_df->StdDev(fDiffFastVars[i].data());
    valStdev.push_back(*stdev);
  }

  for (UInt_t i = 0; i < fVars.size(); ++i) {
    auto &var = fVars[i];
    // plot histrograms via RDataFrame
    auto h1d = fComputed_df->Histo1D({fFullFastVars[i].data(), fVars[i].title.data(), 200, var.min - 3.0, var.max}, fFullFastVars[i].data());
    fVhistFullFast.push_back(h1d);
    if (fFastOnly == kFALSE) {
      h1d = fComputed_df->Histo1D({fFullOrigVars[i].data(), fVars[i].title.data(), 200, var.min - 3.0, var.max}, fFullOrigVars[i].data());
      fVhistFullOrig.push_back(h1d);
    }
    Double_t drange = 0.5 * valStdev[i];
    std::cout << "print the size of vector of the ranges for plotting =" << fDiffRanges.size() << std::endl;
    if (fDiffRanges.size() != 0) {
      drange = fDiffRanges[i];
    }
    // 1D resolutions
    // need to find max and min values
    h1d = fComputed_df->Histo1D({fDiffFastVars[i].data(), (string("Fast #Delta") + fVars[i].title).data(), 100, -drange, drange}, fDiffFastVars[i].data());
    fVhistDiffFast.push_back(h1d);
    if (fFastOnly == kFALSE) {
      h1d = fComputed_df->Histo1D({fDiffOrigVars[i].data(), (string("Original #Delta") + fVars[i].title).data(), 100, -drange, drange}, fDiffOrigVars[i].data());
      fVhistDiffOrig.push_back(h1d);
    }
    // 2D resolutions versus variable
    auto h2d = fComputed_df->Histo2D<Double_t, Double_t>({(fDiffFastVars[i] + "2D").data(), (string("Fast #Delta") + fVars[i].title + " v " + fVars[i].title).data(), 50, var.min, var.max, 50, -drange, drange}, fFullFastVars[i].data(), fDiffFastVars[i].data());
    fVhist2DDiffFast.push_back(h2d);
    std::cout << "After resolutions-3 " << std::endl;
    if (fFastOnly == kFALSE) {
      h2d = fComputed_df->Histo2D({(fDiffOrigVars[i] + "2D").data(), (string("Original #Delta") + fVars[i].title + " v " + fVars[i].title).data(), 50, var.min, var.max, 50, -drange, drange}, fFullOrigVars[i].data(), fDiffOrigVars[i].data());
      fVhist2DDiffOrig.push_back(h2d);
    }
  }
}

/// well as the DVCS variables
void DVCSplotter::plotKinematics(bool plotFastOnly) {
  gStyle->SetOptStat(111111);  // Show entries, mean, RMS, under/overflow
  fFastOnly = plotFastOnly;
  fCanvas = new TCanvas(Form("FullVariables"), Form("FullVariables"), 100, 100, fCanvasWidth,
                        fCanvasHeight);               // ROOT will delete this
  auto ncols = std::ceil(((float)fVars.size()) / 3);  // max 3 columns
  fCanvas->Divide(3, ncols);
  // Plot the full variables
  for (UInt_t i = 0; i < fVars.size(); ++i) {
    fCanvas->cd(i + 1);
    if (fFastOnly == kFALSE) {
      fVhistFullOrig[i]->SetMinimum(0.1);
      fVhistFullOrig[i]->SetLineWidth(2);
      fVhistFullOrig[i]->DrawCopy();
      fVhistFullFast[i]->SetLineColor(2);
      fVhistFullFast[i]->SetLineWidth(2);
      fVhistFullFast[i]->DrawCopy("same");
    } else {
      fVhistFullFast[i]->SetMinimum(0.1);
      fVhistFullFast[i]->SetLineColor(2);
      fVhistFullFast[i]->SetLineWidth(2);
      fVhistFullFast[i]->DrawCopy("");
    }
  }

  fCanvas->SaveAs((fSaveDir + fCanvas->GetName() + "_test.png").data());
  fCanvas = new TCanvas(Form("DeltaVariables"), Form("DeltaVariables"), 100, 100, fCanvasWidth,
                        fCanvasHeight);  // ROOT will delete this
  fCanvas->Divide(3, ncols);
  // Plot the resolution variables
  for (UInt_t i = 0; i < fVars.size(); ++i) {
    fCanvas->cd(i + 1);
    if (fFastOnly == kFALSE) {
      fVhistDiffOrig[i]->SetMinimum(0.1);
      fVhistDiffOrig[i]->SetLineWidth(2);
      fVhistDiffOrig[i]->DrawCopy();
      fVhistDiffFast[i]->SetLineColor(2);
      fVhistDiffFast[i]->SetLineWidth(2);
      fVhistDiffFast[i]->DrawCopy("same");
    } else {
      fVhistDiffFast[i]->SetMinimum(0.1);
      fVhistDiffFast[i]->SetLineColor(2);
      fVhistDiffFast[i]->SetLineWidth(2);
      fVhistDiffFast[i]->DrawCopy("");
    }
  }

  fCanvas->SaveAs((fSaveDir + fCanvas->GetName() + "_test.png").data());
  fCanvas = new TCanvas(Form("FastVariables2D"), Form("FastVariables2D"), 100, 100, fCanvasWidth, fCanvasHeight);        // ROOT will delete this
  auto fCanvas2 = new TCanvas(Form("OrigVariables2D"), Form("OrigVariables2D"), 100, 100, fCanvasWidth, fCanvasHeight);  // ROOT will delete this

  fCanvas->Divide(3, ncols);
  fCanvas2->Divide(3, ncols);
  // Plot the 2Dresolution variables
  for (UInt_t i = 0; i < fVars.size(); ++i) {
    fCanvas->cd(i + 1);
    if (fFastOnly == kFALSE) {
      fVhist2DDiffFast[i]->SetMinimum(0);
      fVhist2DDiffOrig[i]->SetMinimum(0);
      fVhist2DDiffOrig[i]->DrawCopy("col");
      fCanvas2->cd(i + 1);
      fVhist2DDiffFast[i]->DrawCopy("col");
    } else {
      fVhist2DDiffFast[i]->SetMinimum(0);
      fVhist2DDiffFast[i]->DrawCopy("col");
    }
  }

  fCanvas->SaveAs((fSaveDir + fCanvas->GetName() + "_test.png").data());
  fCanvas = nullptr;
}

void DVCSplotter::plotDVCSVars() {
  gStyle->SetOptStat(111111);  // Show entries, mean, RMS, under/overflow
  TCanvas *fCanvas = new TCanvas("cDVCS", "DVCS Variables", 1700, 1500);
  fCanvas->Divide(3, 2);

  fCanvas->cd(1);
  fDVCSVarFast[0]->SetLineColor(kRed);
  fDVCSVarFast[0]->Draw();
  fCanvas->cd(2);
  fDVCSVarFast[1]->SetLineColor(kBlue);
  fDVCSVarFast[1]->Draw();
  fCanvas->cd(3);
  fDVCSVarFast[2]->SetLineColor(kGreen + 2);
  fDVCSVarFast[2]->Draw();
  fCanvas->cd(4);
  fDVCSVarFast[3]->SetLineColor(kOrange + 2);
  fDVCSVarFast[3]->Draw();
  fCanvas->cd(5);
  fDVCSVarFast[4]->SetLineColor(kRed + 2);
  fDVCSVarFast[4]->Draw();
  fCanvas->cd(6);
  fHistQ2_vs_xB->Draw("hist");
  fCanvas->SaveAs((fSaveDir + fCanvas->GetName() + "Variables.png").data());
}

void DVCSplotter::saveAll(const char *outFileName) {
  std::string fullPath = fSaveDir + outFileName;
  TFile *fout = new TFile(fullPath.c_str(), "RECREATE");

  fout->cd();  // <-- Ensures histograms are written to this file
  fComputed_df->Snapshot("recon_Fast",
                         fullPath.c_str(),  // filename ignored when fOutputFile is set
                         {"recon__recel_theta", "recon__recel_phi", "recon__recel_p", "recon__recp1_theta", "recon__recp1_phi", "recon__recp1_p", "recon__recp2_theta", "recon__recp2_phi", "recon__recp2_p", "Q2", "xB", "t", "phi"});

  for (auto &h : fDVCSVarFast) {
    h->SetDirectory(fout);  // Optional: ensures file ownership
    h->Write();
  }
  fout->Close();
}
void DVCSplotter::plotDVCSX() {
  gStyle->SetOptStat(111111);  // Show entries, mean, RMS, under/overflow
  DISCATMath kinCalc;
  auto crossSectionHistos = kinCalc.ComputeDVCS_CrossSection(*fComputed_df, fXbins, fLuminosity);

  /// plot the output in canvas
  size_t n_q2 = fXbins.GetQ2Bins().size() - 1;
  size_t n_t = fXbins.GetTBins().size() - 1;
  size_t n_xb = fXbins.GetXBBins().size() - 1;

  size_t totalBins = n_q2 * n_t * n_xb;

  if (crossSectionHistos.size() != totalBins) {
    std::cerr << "Mismatch: " << crossSectionHistos.size() << " histograms but expected " << totalBins << " bins.\n";
    return;
  }
  // Grid layout: sqrt layout or 1 row per Q² slice
  int cols = n_t * n_xb;
  int rows = n_q2;

  TCanvas *fCanvas = new TCanvas("DVCS_CS_AllBins", "DVCS Cross Sections", 1700, 1500);
  fCanvas->Divide(cols, rows);

  int pad = 1;
  for (size_t iq = 0; iq < n_q2; ++iq) {
    for (size_t it = 0; it < n_t; ++it) {
      for (size_t ix = 0; ix < n_xb; ++ix) {
        int index = iq * n_t * n_xb + it * n_xb + ix;
        TH1D *hist = crossSectionHistos[index];
        if (!hist) continue;
        fCanvas->cd(pad);
        hist->SetLineColor(kBlue + 1);
        hist->SetLineWidth(2);
        hist->GetXaxis()->SetTitle("#phi [deg]");
        hist->GetYaxis()->SetTitle("d#sigma/d#phi [nb/deg]");
        hist->Draw("E1");
        pad++;
      }
    }
  }

  auto hist_ = crossSectionHistos[2];
  int nbins = hist_->GetNbinsX();
  for (int bin = 1; bin <= nbins; ++bin) {  // bin 0 is underflow, nbins+1 is overflow
    double center = hist_->GetBinCenter(bin);
    double content = hist_->GetBinContent(bin);
    double error = hist_->GetBinError(bin);
    std::cout << "  Bin center: " << center << " | Content: " << content << " | Error: " << error << "\n";
  }
  fCanvas->SetLogy(true);
  fCanvas->Update();
  fCanvas->SaveAs((fSaveDir + fCanvas->GetName() + "DVCSX_all_bins.png").data());
  std::cout << "-------------------------------------\n";
  std::cout << "Saved canvas with all histograms to DVCS_CrossSections_AllBins.png\n";
}
