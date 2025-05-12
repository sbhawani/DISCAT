#include "DataConverter.h"

#include <TFile.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
constexpr double pi = 3.14159265358979323846;
void Particle::computeDerived() {
  true_p = reco_p = std::sqrt(px * px + py * py + pz * pz);
  true_theta = reco_theta = (true_p != 0) ? std::acos(pz / true_p) : 0.0f;
  double phi = std::atan2(py, px) + pi;
  true_phi = reco_phi = phi;
}

DataConverter::DataConverter(const std::string& inputFile, const std::string& outputFile) : inputFileName(inputFile + ".dat"), outputFileName(outputFile + ".root"), tree(nullptr), eventAccepted(0) {}

void DataConverter::setupBranches() {
  const char* prefix[4] = {"beam", "el", "p1", "p2"};
  for (int i = 0; i < 4; ++i) {
    std::string pre = prefix[i];
    /*tree->Branch((pre + "_id").c_str(), &particles[i].id);
    tree->Branch((pre + "_t").c_str(), &particles[i].t);
    tree->Branch((pre + "_flag").c_str(), &particles[i].flag);
    tree->Branch((pre + "_pid").c_str(), &particles[i].pid);
    tree->Branch((pre + "_ch").c_str(), &particles[i].ch);
    tree->Branch((pre + "_status").c_str(), &particles[i].status);
    tree->Branch((pre + "_px").c_str(), &particles[i].px);
    tree->Branch((pre + "_py").c_str(), &particles[i].py);
    tree->Branch((pre + "_pz").c_str(), &particles[i].pz);
    tree->Branch((pre + "_E").c_str(), &particles[i].E);
    tree->Branch((pre + "_mass").c_str(), &particles[i].mass);
    tree->Branch((pre + "_vx").c_str(), &particles[i].vx);
    tree->Branch((pre + "_vy").c_str(), &particles[i].vy);
    tree->Branch((pre + "_vz").c_str(), &particles[i].vz);*/
    tree->Branch(("tru" + pre + "_p").c_str(), &particles[i].true_p);
    tree->Branch(("rec" + pre + "_p").c_str(), &particles[i].reco_p);
    tree->Branch(("tru" + pre + "_theta").c_str(), &particles[i].true_theta);
    tree->Branch(("rec" + pre + "_theta").c_str(), &particles[i].reco_theta);
    tree->Branch(("tru" + pre + "_phi").c_str(), &particles[i].true_phi);
    tree->Branch(("rec" + pre + "_phi").c_str(), &particles[i].reco_phi);
  }
  tree->Branch("accepted", &eventAccepted);
}

bool DataConverter::readParticleLine(const std::string& line, int particleIndex) {
  std::istringstream iss(line);
  Particle& p = particles[particleIndex];

  if (particleIndex == 0) {
    if (!(iss >> p.id >> p.t >> p.flag >> p.pid >> p.ch >> p.status >> p.px >> p.py >> p.pz >> p.E)) return false;
    p.mass = 0;
    p.vx = p.vy = p.vz = 0;
  } else {
    if (!(iss >> p.id >> p.t >> p.flag >> p.pid >> p.ch >> p.status >> p.px >> p.py >> p.pz >> p.E >> p.mass >> p.vx >> p.vy >> p.vz)) return false;
  }

  p.computeDerived();
  return true;
}

void DataConverter::processFile() {
  std::ifstream infile(inputFileName);
  if (!infile.is_open()) {
    std::cerr << "Failed to open input file: " << inputFileName << "\n";
    return;
  }

  TFile* outfile = new TFile(outputFileName.c_str(), "RECREATE");
  tree = new TTree("tree", "tree");
  setupBranches();

  std::string line;
  int lineCount = 0;
  int eventCount = 0;

  while (std::getline(infile, line)) {
    if (line.empty()) continue;
    int index = lineCount % 4;

    if (!readParticleLine(line, index)) {
      std::cerr << "Error parsing line: " << line << "\n";
      continue;
    }

    lineCount++;
    if (lineCount % 4 == 0) {
      eventAccepted = 0;
      //if (particles[1].reco_p > 2.0) {
        tree->Fill();
        eventCount++;
      //}
    }
  }

  tree->Write();
  outfile->Close();
  std::cout << "Converted " << eventCount << " events to " << outputFileName << "\n";
}
