#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <string>
#include <unordered_map>

class InputHandler {
public:
    explicit InputHandler(const std::string& mapFilePath);

    // Setters for path components
    void setRunGroup(const std::string& rg);
    void setYear(const std::string& year);
    void setSeason(const std::string& season);
    void setMagnetPolarity(const std::string& polarity);
    void setBackgroundMerge(const std::string& background);
    // Final resolved path
    std::string getMappedInputPath() const;

private:
    void loadPathMap(const std::string& mapFilePath);
    std::string getCompositeKey() const;

    std::unordered_map<std::string, std::string> pathMap;

    std::string runGroup, year, season, magnetPolarity, backgroundMerge;
};

#endif
