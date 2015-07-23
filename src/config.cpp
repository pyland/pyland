#include "config.hpp"
#include "json-parser/json.hpp"
#include <fstream>
using namespace nlohmann;

std::string Config::get_config_info(std::string key) {
    std::ifstream input_file("config.json");
    json j;
    j << input_file;
    LOG(INFO) << key;
    std::string result = j["files"][key];
    return result;
}
