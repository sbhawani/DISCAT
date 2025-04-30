#include "DISCATMath.h"

constexpr double pi = 3.14159265358979323846;

DISCATMath::DISCATMath(const TLorentzVector &electron_in,
                       const TLorentzVector &electron_out,
                       const TLorentzVector &proton_in,
                       const TLorentzVector &proton_out,
                       const TLorentzVector &photon) {
  ComputeKinematics(electron_in, electron_out, proton_in, proton_out, photon);
}

void DISCATMath::ComputeKinematics(const TLorentzVector &electron_in,
                                   const TLorentzVector &electron_out,
                                   const TLorentzVector &proton_in,
                                   const TLorentzVector &proton_out,
                                   const TLorentzVector &photon) {
  TLorentzVector q = electron_in - electron_out;
  Q2_ = -q.Mag2();
  nu_ = q.E();
  y_ = nu_ / electron_in.E();
  W_ = (proton_in + q).Mag();
  xB_ = Q2_ / (2.0 * proton_in.Dot(q));
  t_ = (proton_in - proton_out).Mag2();

  // Plane angles
  TVector3 k_in_vec = electron_in.Vect();
  TVector3 k_out_vec = electron_out.Vect();
  TVector3 q_vec = q.Vect();
  TVector3 photon_vec = photon.Vect();

  TVector3 n_ep = k_in_vec.Cross(k_out_vec).Unit();
  TVector3 n_gamma_p = q_vec.Cross(photon_vec).Unit();

  double cos_phi = n_ep.Dot(n_gamma_p);
  cos_phi = std::clamp(cos_phi, -1.0, 1.0);
  double phi = std::acos(cos_phi);

  // Sign of phi
  if (k_in_vec.Dot(n_gamma_p) < 0)
    phi = 2 * pi - phi;

  phi_deg_ = phi * 180.0 / pi;
}
constexpr double phi_min = 0.0;
constexpr double phi_max = 360.0;
constexpr int n_phi_bins = 12;

// Integrated luminosity in cm⁻² (example, you can scale it out if not known)

std::vector<TH1D *> DISCATMath::ComputeDVCS_CrossSection(ROOT::RDF::RNode df,
                                                         const BinManager &bins,
                                                         double luminosity) {
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

        std::string cut = Form("(Q2 > %.3f && Q2 <= %.3f) && (t > %.3f && t <= "
                               "%.3f) && (xB > %.3f && xB <= %.3f)",
                               qmin, qmax, tmin, tmax, xbmin, xbmax);

        df.Display({"Q2", "t", "xB"})->Print();
        auto df_bin = df.Filter(cut);
        auto nEntries = df_bin.Count();
        std::cout << "Entries in bin (Q2=[" << qmin << "," << qmax << "], t=["
                  << tmin << "," << tmax << "], xB=[" << xbmin << "," << xbmax
                  << "]): " << *nEntries << std::endl;

        std::string histName =
            Form("hphi_q%.1f_t%.1f_xb%.2f", qmin, tmin, xbmin);
        std::string histTitle = Form(
            "d#sigma/d#phi (Q^{2}=[%.1f,%.1f], t=[%.1f,%.1f], xB=[%.2f,%.2f])",
            qmin, qmax, tmin, tmax, xbmin, xbmax);

        auto hphi = df_bin.Histo1D(
            {histName.c_str(), histTitle.c_str(), n_phi_bins, phi_min, phi_max},
            "phi");
        hphi->Draw(); // materialize
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
