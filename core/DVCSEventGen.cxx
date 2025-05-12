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

void DVCSEventGen::run2(const std::unordered_map<std::string, std::string>& config_) {
    std::string scriptDir = "/w/hallb-scshelf2102/clas12/singh/HighLevelAN/Generators/dvcs_simulation";
    std::string scriptName = "main";  // Refers to main.py
    std::string pythonScriptPath = scriptDir + "/main.py";

    if (!std::filesystem::exists(pythonScriptPath)) {
        std::cerr << "Error: Python script not found at " << pythonScriptPath << std::endl;
        return;
    }

    std::ostringstream py;
    py << "import sys\n";
    py << "sys.path.append('" << scriptDir << "')\n";
    py << "import " << scriptName << "\n";
    py << "args = {}\n";

    for (const auto& [key, value] : config_) {
        if (key == "radgen" || key == "fringe") {
            py << "args['" << key << "'] = " << (value == "true" ? "True" : "False") << "\n";
        } else if (key == "nentries" || key == "seed" || key == "bin") {
            py << "args['" << key << "'] = int(" << value << ")\n";
        } else if (key == "beam" || key == "xBmin" || key == "xBmax" || key == "Q2min" || key == "Q2max" ||
                   key == "tmin" || key == "tmax" || key == "ymin" || key == "ymax" || key == "w2min") {
            py << "args['" << key << "'] = float(" << value << ")\n";
        } else {
            py << "args['" << key << "'] = '" << value << "'\n";
        }
    }

    py << scriptName << ".main(**args)\n";
    TPython::Exec(py.str().c_str());
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
