#include "DISCATMath.h"

constexpr double pi = 3.14159265358979323846;

DISCATMath::DISCATMath(const TLorentzVector& electron_in, 
                       const TLorentzVector& electron_out,
                       const TLorentzVector& proton_in,
                       const TLorentzVector& proton_out,
                       const TLorentzVector& photon)
{
    ComputeKinematics(electron_in, electron_out, proton_in, proton_out, photon);
}

void DISCATMath::ComputeKinematics(const TLorentzVector& electron_in,
                                   const TLorentzVector& electron_out,
                                   const TLorentzVector& proton_in,
                                   const TLorentzVector& proton_out,
                                   const TLorentzVector& photon)
{
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
        phi = 2*pi - phi;

    phi_deg_ = phi * 180.0 / pi;
}
