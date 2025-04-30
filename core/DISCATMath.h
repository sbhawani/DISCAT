#ifndef DISCATMATH_H
#define DISCATMATH_H

#include "TLorentzVector.h"
#include <ROOT/RDataFrame.hxx>
#include <algorithm>
#include <cmath>
#include "BinManager.h"


class DISCATMath {
public:
  DISCATMath() = default;
  // Constructor: takes momenta and computes kinematics
  DISCATMath(const TLorentzVector &electron_in,
             const TLorentzVector &electron_out,
             const TLorentzVector &proton_in, const TLorentzVector &proton_out,
             const TLorentzVector &photon);

  // Getter functions
  double GetQ2() const { return Q2_; }
  double GetxB() const { return xB_; }
  double GetT() const { return t_; }
  double GetPhi() const { return phi_deg_; }
  double GetW() const { return W_; }
  double GetNu() const { return nu_; }
  double Gety() const { return y_; }
  std::vector<TH1D*> ComputeDVCS_CrossSection(ROOT::RDF::RNode df, const BinManager &bins, double luminosity);

private:
  // Internal kinematic variables
  double Q2_;
  double xB_;
  double t_;
  double phi_deg_;
  double W_;
  double nu_;
  double y_;

  // Internal compute function
  void ComputeKinematics(const TLorentzVector &electron_in,
                         const TLorentzVector &electron_out,
                         const TLorentzVector &proton_in,
                         const TLorentzVector &proton_out,
                         const TLorentzVector &photon);
};

#endif // DISCATMATH_H
