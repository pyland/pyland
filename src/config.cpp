#include <fstream>

#include "config.hpp"

extern "C" {
    #include "jsonnet/libjsonnet.h"
}

std::string Config::get_config_info(std::string key) {
    std::ifstream input_file("config.json");
    nlohmann::json j;
    input_file >> j;
    LOG(INFO) << key;
    std::string result = j["files"]["level_location"];
    return result;
}

nlohmann::json Config::get_instance() {
    //JsonnetVm* jvm = jsonnet_make();
    //int *error = nullptr;
    //const char *file_location = "/home/tom/Pyland/personal/pyland/src/config.jsonnet";
    //jsonnet_evaluate_file(jvm, file_location, error);
    std::ifstream input_file("config.json");
    nlohmann::json j;
    input_file >> j;
    return j;
}
