#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <glog/logging.h>

#include "json-parser/json.hpp"

class Config : public nlohmann::json {
    public:
        static nlohmann::json get_instance();
};

#endif
