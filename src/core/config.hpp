#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <glog/logging.h>

#include <json.hpp>

class Config : public nlohmann::json {
    private:
        static nlohmann::json j;
        static bool created;
    public:
        typedef nlohmann::json json;
        static json get_instance();
        static void refresh_config();
};

#endif
