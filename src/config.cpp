#include "config.hpp"
#include <fstream>

std::string Config::get_config_info(std::string key) {
    std::ifstream input_file("config.json");
    nlohmann::json j;
    input_file >> j;
    LOG(INFO) << key;
    std::string result = j["files"]["level_location"];
    return result;
}

nlohmann::json Config::get_instance() {
    std::ifstream input_file("config.json");
    nlohmann::json j;
    input_file >> j;
    return j;
}
