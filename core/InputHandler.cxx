#include "InputHandler.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

InputHandler::InputHandler(const std::string& mapFilePath) {
    loadPathMap(mapFilePath);
}

void InputHandler::loadPathMap(const std::string& mapFilePath) {
    std::ifstream file(mapFilePath);
    if (!file) {
        throw std::runtime_error("Could not open path map file: " + mapFilePath);
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#' || line[0] == ';') continue;

        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            // Clean up extra whitespace
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);

            pathMap[key] = value;
        }
    }
}

// Setters
void InputHandler::setRunGroup(const std::string& rg) { runGroup = rg; }
void InputHandler::setYear(const std::string& y) { year = y; }
void InputHandler::setSeason(const std::string& s) { season = s; }
void InputHandler::setMagnetPolarity(const std::string& p) { magnetPolarity = p; }
void InputHandler::setBackgroundMerge(const std::string& b) { backgroundMerge = b; }

std::string InputHandler::getCompositeKey() const {
    return runGroup + "_" + year + "_" + season + "_" + magnetPolarity + "_" + backgroundMerge;
}

std::string InputHandler::getMappedInputPath() const {
    std::string key = getCompositeKey();
    auto it = pathMap.find(key);
    if (it == pathMap.end()) {
        throw std::runtime_error("Mapped input path not found for key: " + key);
    }
    return it->second;
}

