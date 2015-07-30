#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <glog/logging.h>

#include "json-parser/json.hpp"

class Config : public nlohmann::json {
    private:
        static nlohmann::json j;
        static bool created;
    public:
        static nlohmann::json get_instance();
};

#endif
