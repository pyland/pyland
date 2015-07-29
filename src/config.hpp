#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <glog/logging.h>

#include "json-parser/json.hpp"

class Config : public nlohmann::json {
    public:
        static std::string get_config_info(std::string key);
        static nlohmann::json get_instance();
};

#endif
