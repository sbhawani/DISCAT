#include "DVCSplotter.h"

#include <cmath>
#include <iostream>

#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "DISCATMath.h"

DVCSplotter::DVCSplotter(DataLoader &loader, ConfigureSimulation &conf, const std::string &outputDir) : fLoader(&loader), fConf(&conf), fSaveDir(outputDir)
{
    if (!fLoader)
    {
        return;
    }
    else
    {
        // fConf->ResolutionDir()+"/resolution_plots/";
        std::cout << "the save dir from config is:" << fSaveDir << std::endl;
        gSystem->Exec(Form("mkdir -p %s", fSaveDir.data()));
        fAccEvents = std::make_shared<ROOT::RDF::RNode>(fLoader->GetAcceptedFrame());
        fVars = fLoader->GetDetailedVars();
        std::cout << " Checking size of fVars = " << fVars.size() << std::endl;
        createHistograms();
        fillHistogramsRDF();
    }
}

void DVCSplotter::createHistograms()
{
    h_ep = new TH1D("h_ep", "Electron Momentum;p [GeV];Counts", 100, 0, 10);
    h_eth = new TH1D("h_eth", "Electron Theta;#theta [rad];Counts", 100, 0, 1);
    h_ephi = new TH1D("h_ephi", "Electron Phi;#phi [rad];Counts", 100, 0, 2 * M_PI);

    h_pp = new TH1D("h_pp", "Proton Momentum;p [GeV];Counts", 100, 0, 2);
    h_pth = new TH1D("h_pth", "Proton Theta;#theta [rad];Counts", 100, 0, 2);
    h_pphi = new TH1D("h_pphi", "Proton Phi;#phi [rad];Counts", 100, 0, 2 * M_PI);

    h_gp = new TH1D("h_gp", "Photon Momentum;p [GeV];Counts", 100, 0, 10);
    h_gth = new TH1D("h_gth", "Photon Theta;#theta [rad];Counts", 100, 0, 1);
    h_gphi = new TH1D("h_gphi", "Photon Phi;#phi [rad];Counts", 100, 0, 2 * M_PI);

    h_Q2 = new TH1D("h_Q2", "Q^{2};Q^{2} [GeV^{2}];Counts", 100, 0, 10);
    h_xB = new TH1D("h_xB", "Bjorken x_{B};x_{B};Counts", 100, 0, 1);
    h_t = new TH1D("h_t", "|t|;|t| [GeV^{2}];Counts", 100, 0, 2);
}

// I used this part of the codes from macparticle libs since its same functions
// I am needing here
void DVCSplotter::fillHistogramsRDF()
{
    /*names_t cachevars;
    std::cout<<" Checking file at = "<<fSaveDir+"all/predictions.root"<<std::endl;
    /// from the default Checking file at =
    /u/home/singh/Softwares/DISCAT/temp_test/pred_output//all/predictions.root
    for(const auto& var:fVars){
      //add fast resolution variable to dataframe
      auto fast_var =
    fLoader->CopyColumnFromTree("recon",fSaveDir+"all/predictions.root",var.rec_name);
      //define its difference column with truth variable
      std::string fastDiffName = var.rec_name+"Diff";
      fLoader->DefineColumn(fastDiffName,var.acc_name+"-"+ fast_var);
      //define original difference column with truth variable (from full
    simulation) std::string origDiffName = var.rec_name+"DiffReal";
      fLoader->DefineColumn(origDiffName,var.acc_name+"-"+ var.rec_name);

      fFullFastVars.push_back(fast_var);cachevars.push_back(fast_var);
      fFullOrigVars.push_back(var.rec_name);cachevars.push_back(var.rec_name);
      fDiffFastVars.push_back(fastDiffName);cachevars.push_back(fastDiffName);
      fDiffOrigVars.push_back(origDiffName);cachevars.push_back(origDiffName);
      std::cout<<"running for the pushing back some variables  ==== =======
    "<<std::endl;
    }

    fLoader->Cache(cachevars);
    auto faccEvents = fLoader->GetAcceptedFrame();
    fAccEvents =
    std::make_shared<ROOT::RDF::RNode>(fLoader->GetAcceptedFrame());*/

    names_t cachevars;
    std::cout << "Checking file at = " << fSaveDir + "all/predictions.root" << std::endl;

    // Load all fast variables
    for (const auto &var : fVars)
    {
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

        std::cout << "running for the pushing back some variables  ==== ======= " << std::endl;
    }

    // ADD YOUR DISCAT DEFINITIONS HERE
    fAccEvents = std::make_shared<ROOT::RDF::RNode>(fLoader->GetAcceptedFrame());

    // Now define DISCAT variables
    *fAccEvents = fAccEvents->Define("DISCAT_Q2",
                                     [](double recel_p, double recel_theta, double recel_phi, double recp1_p, double recp1_theta, double recp1_phi, double recp2_p, double recp2_theta, double recp2_phi)
                                     {
                                         TLorentzVector k_in(0, 0, 10.0, 10.0);  // incoming beam
                                         TLorentzVector p_in(0, 0, 0, 0.938);    // proton target

                                         TVector3 ele_vec;
                                         ele_vec.SetMagThetaPhi(recel_p, recel_theta, recel_phi);
                                         TLorentzVector k_out(ele_vec, sqrt(ele_vec.Mag2() + 0.000511 * 0.000511));

                                         TVector3 pro_vec;
                                         pro_vec.SetMagThetaPhi(recp1_p, recp1_theta, recp1_phi);
                                         TLorentzVector p_out(pro_vec, sqrt(pro_vec.Mag2() + 0.938 * 0.938));

                                         TVector3 pho_vec;
                                         pho_vec.SetMagThetaPhi(recp2_p, recp2_theta, recp2_phi);
                                         TLorentzVector q_prime(pho_vec, pho_vec.Mag());

                                         DISCATMath ev(k_in, k_out, p_in, p_out, q_prime);
                                         return ev.GetQ2();
                                     },
                                     {"recel_p", "recel_theta", "recel_phi", "recp1_p", "recp1_theta", "recp1_phi", "recp2_p", "recp2_theta", "recp2_phi"});

    // You can add more: DISCAT_xB, DISCAT_t, DISCAT_phi

    cachevars.push_back("DISCAT_Q2");  // Don't forget to cache new columns!
    //// cachevars.push_back("DISCAT_xB"); etc if you define more
    std::vector<std::string> existingVars;
    for (const auto &var : cachevars)
    {
        if (var.find("rec") == 0 || var.find("recon__") == 0)
        {
            existingVars.push_back(var);
        }
        else
        {
            std::cout << "Skipping unknown variable: " << var << std::endl;
        }
    }

    fLoader->Cache(existingVars);
    // THEN CONTINUE
    std::cout << "DISCAT_Q2 was cached\n";
    // accepted events
    auto faccEvents = fLoader->GetAcceptedFrame();
    fAccEvents = std::make_shared<ROOT::RDF::RNode>(faccEvents);

    std::vector<Double_t> valStdev;
    for (UInt_t i = 0; i < fVars.size(); ++i)
    {
        auto &var = fVars[i];
        auto stdev = fAccEvents->StdDev(fDiffFastVars[i].data());
        valStdev.push_back(*stdev);
        std::cout << "resolution  ==== ======= " << fDiffFastVars[i] << *stdev << std::endl;
    }

    for (UInt_t i = 0; i < fVars.size(); ++i)
    {
        auto &var = fVars[i];
        // plot histrograms via RDataFrame
        // 1D full distributions
        auto h1d = fAccEvents->Histo1D({fFullFastVars[i].data(), fVars[i].title.data(), 200, var.min, var.max}, fFullFastVars[i].data());
        fVhistFullFast.push_back(h1d);
        if (fFastOnly == kFALSE)
        {
            h1d = fAccEvents->Histo1D({fFullOrigVars[i].data(), fVars[i].title.data(), 200, var.min, var.max}, fFullOrigVars[i].data());
            fVhistFullOrig.push_back(h1d);
        }
        Double_t drange = 0.5 * valStdev[i];
        if (fDiffRanges.size() != 0)
        {
            drange = fDiffRanges[i];
        }

        // 1D resolutions
        // need to find max and min values
        h1d = fAccEvents->Histo1D({fDiffFastVars[i].data(), (string("Fast #Delta") + fVars[i].title).data(), 100, -drange, drange}, fDiffFastVars[i].data());
        fVhistDiffFast.push_back(h1d);
        if (fFastOnly == kFALSE)
        {
            h1d = fAccEvents->Histo1D({fDiffOrigVars[i].data(), (string("Original #Delta") + fVars[i].title).data(), 100, -drange, drange}, fDiffOrigVars[i].data());
            fVhistDiffOrig.push_back(h1d);
        }
        // 2D resolutions versus variable
        auto h2d = fAccEvents->Histo2D<Double_t, Double_t>({(fDiffFastVars[i] + "2D").data(), (string("Fast #Delta") + fVars[i].title + " v " + fVars[i].title).data(), 50, var.min, var.max, 50, -drange, drange}, fFullFastVars[i].data(), fDiffFastVars[i].data());
        fVhist2DDiffFast.push_back(h2d);
        std::cout << "After resolutions-3 " << std::endl;
        if (fFastOnly == kFALSE)
        {
            h2d = fAccEvents->Histo2D({(fDiffOrigVars[i] + "2D").data(), (string("Original #Delta") + fVars[i].title + " v " + fVars[i].title).data(), 50, var.min, var.max, 50, -drange, drange}, fFullOrigVars[i].data(), fDiffOrigVars[i].data());
            fVhist2DDiffOrig.push_back(h2d);
        }
    }
    TCanvas *c1 = new TCanvas("c1", "Q^2 Distribution", 800, 600);


    // TH1F *h_Q2 = fAccEvents->Histo1D({"DISCAT_Q2", (string("Original #Delta") + fVars[i].title).data(), 100, -drange, drange}, fDiffOrigVars[i].data());
    auto h_Q2 = fAccEvents->Histo1D({"hQ2", "Q^{2} Distribution;Q^{2} (GeV^{2});Events", 100, 0, 10}, "DISCAT_Q2");

    // Customize the histogram (optional)
    h_Q2->SetXTitle("Q^2 (GeV^2)");
    h_Q2->SetYTitle("Entries");
    h_Q2->SetLineColor(kBlue);
    h_Q2->SetLineWidth(2);

    // Draw the histogram
    h_Q2->Draw();

    // Save the plot as an image (optional)
    c1->SaveAs((fSaveDir + c1->GetName() + "Q2_test.png").data());
}

/// we might want to build an event to compute all the particle properties as
/// well as the DVCS variables
void DVCSplotter::buildDISEvent()
{
    TLorentzVector k_in(0.0, 0.0, 10.0, 10.0);      // incoming electron
    TLorentzVector p_in(0.0, 0.0, 0.0, 0.938);      // proton at rest
    TLorentzVector k_out(1.0, 0.0, 7.8, 8.0);       // scattered electron
    TLorentzVector q_prime(1.0, 0.5, 1.0, 1.5);     // real photon
    TLorentzVector p_out(-2.0, -0.5, -1.0, 0.438);  // scattered proton

    DISCATMath event(k_in, k_out, p_in, p_out, q_prime);

    std::cout << "Q^2 = " << event.GetQ2() << " GeV^2\n";
    std::cout << "xB = " << event.GetxB() << "\n";
    std::cout << "t = " << event.GetT() << " GeV^2\n";
    std::cout << "phi = " << event.GetPhi() << " degrees\n";
    std::cout << "W = " << event.GetW() << " GeV\n";
    std::cout << "nu = " << event.GetNu() << " GeV\n";
    std::cout << "y = " << event.Gety() << "\n";
}
//// this part may not be required at the end
/* ROOT::RDataFrame df("Events", "data.root");
 TLorentzVector p_in(0, 0, 0, m_p);
 TLorentzVector e_in(0, 0, E_beam, E_beam);

 auto df1 = df
     .Define("ve", "TVector3(); ve.SetMagThetaPhi(ep, eth, ephi); ve")
     .Define("vp", "TVector3(); vp.SetMagThetaPhi(pp, pth, pphi); vp")
     .Define("vg", "TVector3(); vg.SetMagThetaPhi(gp, gth, gphi); vg")
     .Define("e_out", [](const TVector3& ve) {
         double E = std::sqrt(ve.Mag2() + DVCSplotter::m_e * DVCSplotter::m_e);
         return TLorentzVector(ve, E);
     }, {"ve"})
     .Define("p_out", [](const TVector3& vp) {
         double E = std::sqrt(vp.Mag2() + DVCSplotter::m_p * DVCSplotter::m_p);
         return TLorentzVector(vp, E);
     }, {"vp"})
     .Define("q",    [](const TLorentzVector& e_out) {
         return TLorentzVector(0,0,E_beam,E_beam) - e_out;
     }, {"e_out"})
     .Define("Q2",   [](const TLorentzVector& q) { return -q.Mag2(); }, {"q"})
     .Define("xB",   [=](const TLorentzVector& q) { return -q.Mag2() / (2 *
 p_in.Dot(q)); }, {"q"}) .Define("t",    [](const TLorentzVector& p_out) {
         return std::fabs((TLorentzVector(0,0,0,m_p) - p_out).Mag2());
     }, {"p_out"});

 df1.Foreach([this](double ep, double eth, double ephi,
                    double pp, double pth, double pphi,
                    double gp, double gth, double gphi,
                    double Q2, double xB, double t) {
     h_ep->Fill(ep);    h_eth->Fill(eth);    h_ephi->Fill(ephi);
     h_pp->Fill(pp);    h_pth->Fill(pth);    h_pphi->Fill(pphi);
     h_gp->Fill(gp);    h_gth->Fill(gth);    h_gphi->Fill(gphi);
     h_Q2->Fill(Q2);    h_xB->Fill(xB);      h_t->Fill(t);
 }, {"ep", "eth", "ephi", "pp", "pth", "pphi", "gp", "gth", "gphi", "Q2", "xB",
 "t"});*/

void DVCSplotter::plotKinematics()
{
    fCanvas = new TCanvas(Form("FullVariables"), Form("FullVariables"), 100, 100, fCanvasWidth,
                          fCanvasHeight);  // ROOT will delete this
    std::cout << " Plotting the kinematics variable\n";
    auto ncols = std::ceil(((float) fVars.size()) / 3);  // max 3 columns
    fCanvas->Divide(3, ncols);
    std::cout << " Plotting the kinematics variable\n";
    // Plot the full variables
    for (UInt_t i = 0; i < fVars.size(); ++i)
    {
        fCanvas->cd(i + 1);
        if (fFastOnly == kFALSE)
        {
            fVhistFullOrig[i]->SetMinimum(0.1);
            fVhistFullOrig[i]->SetLineWidth(2);
            fVhistFullOrig[i]->DrawCopy();
            fVhistFullFast[i]->SetLineColor(2);
            fVhistFullFast[i]->SetLineWidth(2);
            fVhistFullFast[i]->DrawCopy("same");
        }
        else
        {
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
    for (UInt_t i = 0; i < fVars.size(); ++i)
    {
        fCanvas->cd(i + 1);
        if (fFastOnly == kFALSE)
        {
            fVhistDiffOrig[i]->SetMinimum(0.1);
            fVhistDiffOrig[i]->SetLineWidth(2);
            fVhistDiffOrig[i]->DrawCopy();
            fVhistDiffFast[i]->SetLineColor(2);
            fVhistDiffFast[i]->SetLineWidth(2);
            fVhistDiffFast[i]->DrawCopy("same");
        }
        else
        {
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
    for (UInt_t i = 0; i < fVars.size(); ++i)
    {
        fCanvas->cd(i + 1);
        if (fFastOnly == kFALSE)
        {
            fVhist2DDiffFast[i]->SetMinimum(0);
            fVhist2DDiffOrig[i]->SetMinimum(0);
            fVhist2DDiffOrig[i]->DrawCopy("col");
            fCanvas2->cd(i + 1);
            fVhist2DDiffFast[i]->DrawCopy("col");
        }
        else
        {
            fVhist2DDiffFast[i]->SetMinimum(0);
            fVhist2DDiffFast[i]->DrawCopy("col");
        }
    }

    fCanvas->SaveAs((fSaveDir + fCanvas->GetName() + "_test.png").data());
}

void DVCSplotter::plotDVCSVars()
{
    TCanvas *c = new TCanvas("cDVCS", "DVCS Variables", 1200, 400);
    c->Divide(3, 1);

    c->cd(1);
    h_Q2->SetLineColor(kRed);
    h_Q2->Draw();
    c->cd(2);
    h_xB->SetLineColor(kBlue);
    h_xB->Draw();
    c->cd(3);
    h_t->SetLineColor(kGreen + 2);
    h_t->Draw();

    c->SaveAs("DVCS_Variables.pdf");
}

void DVCSplotter::saveAll(const char *outFileName)
{
    TFile *fout = new TFile(outFileName, "RECREATE");

    h_ep->Write();
    h_eth->Write();
    h_ephi->Write();
    h_pp->Write();
    h_pth->Write();
    h_pphi->Write();
    h_gp->Write();
    h_gth->Write();
    h_gphi->Write();
    h_Q2->Write();
    h_xB->Write();
    h_t->Write();

    fout->Close();
    std::cout << "Histograms saved to " << outFileName << std::endl;
}
