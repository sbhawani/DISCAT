#ifndef DATA_CONVERTER_H
#define DATA_CONVERTER_H

#include <string>
#include <TTree.h>

struct Particle {
    float id, t, flag, pid, ch, status;
    float px, py, pz, E, mass;
    float vx, vy, vz;

    float true_p, reco_p;
    float true_theta, reco_theta;
    float true_phi, reco_phi;

    void computeDerived();
};

class DataConverter {
public:
    DataConverter(const std::string& inputFile, const std::string& outputFile);
    void processFile();

private:
    void setupBranches();
    bool readParticleLine(const std::string& line, int particleIndex);
    
    std::string inputFileName;
    std::string outputFileName;

    Particle particles[4];
    int eventAccepted;
    TTree* tree;
};

#endif // DATA_CONVERTER_H
