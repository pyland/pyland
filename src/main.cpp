#define GLM_FORCE_RADIANS

#include <glog/logging.h>
#include <iostream>

#include "game_main.hpp"

using namespace std;

int main(int argc, char *argv[]) {

    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    GameMain game(argc, argv);

    return 0;
}

