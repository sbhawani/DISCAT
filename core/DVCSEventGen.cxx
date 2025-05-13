#include "DVCSEventGen.h"
#include <TPython.h>
#include <iostream>
#include <sstream>
#include <filesystem>

DVCSEventGen::DVCSEventGen() {
    // No need to manage interpreter when using TPython
}
DVCSEventGen::~DVCSEventGen() {
    // No need to manage interpreter when using TPython
}
void DVCSEventGen::run(const std::unordered_map<std::string, std::string>& config_) {
    std::string pythonScriptPath = "/w/hallb-scshelf2102/clas12/singh/HighLevelAN/Generators/dvcs_simulation/main.py";
    if (!std::filesystem::exists(pythonScriptPath)) {
        std::cerr << "Error: Python script not found at " << pythonScriptPath << std::endl;
        return;
    }

    std::ostringstream cmd;
    cmd << "python3 " << pythonScriptPath;

    for (const auto& [key, value] : config_) {
        if (key == "radgen" || key == "fringe") {
            if (value == "true") {
                cmd << " --" << key;
            }
        } else {
            cmd << " --" << key << " " << value;
        }
    }
    
    std::string commandStr = cmd.str();
    std::cout << "Executing Python command: " << commandStr << std::endl;
    int ret = std::system(commandStr.c_str());
    if (ret != 0) {
        std::cerr << "Python script execution failed with return code " << ret << std::endl;
    }
}
