// DISCATMath.cpp (updated)
#include "DISCATMath.h"
#include "TMath.h"
#include <cmath>
#include <iostream>

constexpr double pi = 3.14159265358979323846;
const double m_e = 0.000511;  // GeV
const double m_p = 0.938272;  // GeV

namespace {
TVector3 SphericalToCartesian(double p, double theta, double phi) {
  double px = p * std::sin(theta) * std::cos(phi);
  double py = p * std::sin(theta) * std::sin(phi);
  double pz = p * std::cos(theta);
  return TVector3(px, py, pz);
}

TLorentzVector Build4Vector(double p, double theta, double phi, double mass) {
  TVector3 vec = SphericalToCartesian(p, theta, phi);
  double E = std::sqrt(vec.Mag2() + mass * mass);
  return TLorentzVector(vec, E);
}
} // anonymous namespace

DISCATMath::DISCATMath(double e_in_E,
                       double e_out_p, double e_out_theta, double e_out_phi,
                       double p_out_p, double p_out_theta, double p_out_phi,
                       double g_p, double g_theta, double g_phi) {
  TLorentzVector electron_in(0, 0, e_in_E, e_in_E);
  TLorentzVector electron_out = Build4Vector(e_out_p, e_out_theta, e_out_phi, m_e);
  TLorentzVector proton_in(0, 0, 0, m_p);
  TLorentzVector proton_out = Build4Vector(p_out_p, p_out_theta, p_out_phi, m_p);
  TLorentzVector photon = Build4Vector(g_p, g_theta, g_phi, 0.0);
  ComputeKinematics(electron_in, electron_out, proton_in, proton_out, photon);
}

void DISCATMath::ComputeKinematics(const TLorentzVector &electron_in, const TLorentzVector &electron_out,
                                   const TLorentzVector &proton_in, const TLorentzVector &proton_out,
                                   const TLorentzVector &photon) {
  TLorentzVector q = electron_in - electron_out;
  Q2_ = -q.Mag2();
  nu_ = q.E();
  y_ = nu_ / electron_in.E();
  W_ = (proton_in + q).Mag();
  xB_ = Q2_ / (2.0 * proton_in.Dot(q));
  t_ = (proton_in - proton_out).Mag2();

  TVector3 n_L = electron_in.Vect().Cross(electron_out.Vect()).Unit();
  TVector3 n_H = q.Vect().Cross(proton_out.Vect()).Unit();

  double cos_phi = n_L.Dot(n_H);
  double sin_phi = (n_L.Cross(n_H)).Dot(q.Vect().Unit());

  double phi = std::atan2(sin_phi, cos_phi);
  phi = phi + TMath::Pi();
  phi = phi * 180.0 / TMath::Pi();

  phi_deg_ = phi;
}

// Integrated luminosity in cm⁻² (example, you can scale it out if not known)

std::vector<TH1D *> DISCATMath::ComputeDVCS_CrossSection(ROOT::RDF::RNode df, const BinManager &bins, double luminosity) {
  constexpr double phi_min = 0.0;
  constexpr double phi_max = 360.0;
  constexpr int n_phi_bins = 12;

  const auto &q2_bins = bins.GetQ2Bins();
  const auto &t_bins = bins.GetTBins();
  const auto &xb_bins = bins.GetXBBins();

  // before computing the cross-section
  auto cols = df.GetColumnNames();
  for (const auto &col : cols) {
    std::cout << col << "\n";
  }

  std::vector<TH1D *> histograms;

  for (size_t iq = 0; iq < q2_bins.size() - 1; ++iq) {
    for (size_t it = 0; it < t_bins.size() - 1; ++it) {
      for (size_t ix = 0; ix < xb_bins.size() - 1; ++ix) {
        double qmin = q2_bins[iq], qmax = q2_bins[iq + 1];
        double tmin = t_bins[it], tmax = t_bins[it + 1];
        double xbmin = xb_bins[ix], xbmax = xb_bins[ix + 1];

        std::string cut = Form(
            "(Q2 > %.3f && Q2 <= %.3f) && (t > %.3f && t <= "
            "%.3f) && (xB > %.3f && xB <= %.3f)",
            qmin, qmax, tmin, tmax, xbmin, xbmax);

        df.Display({"Q2", "t", "xB"})->Print();
        auto df_bin = df.Filter(cut);
        auto nEntries = df_bin.Count();
        std::cout << "Entries in bin (Q2=[" << qmin << "," << qmax << "], t=[" << tmin << "," << tmax << "], xB=[" << xbmin << "," << xbmax << "]): " << *nEntries << std::endl;

        std::string histName = Form("hphi_q%.1f_t%.1f_xb%.2f", qmin, tmin, xbmin);
        std::string histTitle = Form("d#sigma/d#phi (Q^{2}=[%.1f,%.1f], t=[%.1f,%.1f], xB=[%.2f,%.2f])", qmin, qmax, tmin, tmax, xbmin, xbmax);

        auto hphi = df_bin.Histo1D({histName.c_str(), histTitle.c_str(), n_phi_bins, phi_min, phi_max}, "phi");
        // hphi->Draw(); // materialize
        std::cout << " hphi values are obtained here" << std::endl;
        // Clone histogram from RDataFrame-managed memory
        TH1D *hclone = dynamic_cast<TH1D *>(hphi->Clone(histName.c_str()));
        double bin_width = (phi_max - phi_min) / n_phi_bins;

        for (int b = 1; b <= hclone->GetNbinsX(); ++b) {
          double raw = hclone->GetBinContent(b);
          double norm = raw / (luminosity * bin_width);
          double err = std::sqrt(raw) / (luminosity * bin_width);
          hclone->SetBinContent(b, norm);
          hclone->SetBinError(b, err);
        }

        histograms.push_back(hclone);
      }
    }
  }

  std::cout << "DVCS cross-sections computed and histograms returned.\n";
  return histograms;
}
