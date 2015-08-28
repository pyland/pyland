#include <fstream>
#include <stdio.h>
#include <stdlib.h>

#include "config.hpp"

extern "C" {
    #include "jsonnet/libjsonnet.h"
}

// Hacky workaround to use jsonnet, calls the commandline application instead of using the C library :(
std::string exec(const char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}

Config::json Config::j;
bool Config::created = false;

int bob(int argc, const char *fn)
{
    int error;
    char *output;
    struct JsonnetVm *vm;
    if (argc != 2) {
        fprintf(stderr, "libjsonnet_test_file <file>\n");
        return EXIT_FAILURE;
    }
    vm = jsonnet_make();
    output = jsonnet_evaluate_file(vm, fn, &error);
    if (error) {
        fprintf(stderr, "%s", output);
        jsonnet_realloc(vm, output, 0);
        jsonnet_destroy(vm);
        return EXIT_FAILURE;
    } 
    printf("%s", output);
    jsonnet_realloc(vm, output, 0);
    jsonnet_destroy(vm);
    return EXIT_SUCCESS;
}

Config::json Config::get_instance() {
    /*
    int error;
    char *output;
    struct JsonnetVm *vm;
    vm = jsonnet_make();
    output = jsonnet_evaluate_file(vm, "config.jsonnet", &error);
    if (error) {
        fprintf(stderr, "%s", output);
        jsonnet_realloc(vm, output, 0);
        jsonnet_destroy(vm);
        //TODO: handle error here in some way > terminate game and give helpful hint as to cause of problem?
    }
    LOG(INFO) << output; 
    //print("%s", output);
    jsonnet_realloc(vm, output, 0);
    jsonnet_destroy(vm);
    output = jsonnet_to_json("config.jsonnet");
    */
    if(!created) {
        std::string output = exec("jsonnet/jsonnet config.jsonnet");
        Config::j = Config::json::parse(output);
        Config::created = true;
    }

    //bob(2, "config.jsonnet");
  
    /*if(!created) {
        //std::string output = exec("jsonnet/jsonnet config.jsonnet");
        int error;
        char *output;
        struct JsonnetVm *vm;
        vm = jsonnet_make();
        output = jsonnet_evaluate_file(vm, "config.jsonnet", &error);
        Config::j = nlohmann::json::parse(output);
        Config::created = true;
    }
    */
    return Config::j;
}


void Config::refresh_config() {
    created = false;
}
