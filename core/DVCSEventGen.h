#pragma once
#include <string>
#include <unordered_map>

class DVCSEventGen {
 public:
  DVCSEventGen();  // Constructor
  ~DVCSEventGen(); // Destructor

  // Run the Python script with the provided configuration
  void run2(const std::unordered_map<std::string, std::string>& config_);
  void run(const std::unordered_map<std::string, std::string>& config_);

 private:
  //void initializePython();  // Optional initialization (not used with system calls)

};

