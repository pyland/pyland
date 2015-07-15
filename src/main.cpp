#include <glog/logging.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <chrono>
#include <functional>
//#include <glm/vec2.hpp>
#include <iostream>
#include <memory>
#include <random>
#include <ratio>
#include <string>
#include <sstream>
#include <utility>
#include <vector>

#include "game_init.hpp"

#include "challenge_data.hpp"

#include "engine.hpp"


using namespace std;

static std::mt19937 random_generator;
Challenge* pick_challenge(ChallengeData* challenge_data);
int main(int argc, char *argv[]) {

    game_init(argc, argv);
    switch (argc) {
        default:
            std::cout << "Usage: " << argv[0] << " [EDITOR] [MAP]" << std::endl;
            return 1;

        // The lack of break statements is not an error!!!
        case 3:
            //map_path = std::string(argv[2]);
        case 2:
            Engine::set_editor(argv[1]);
        case 1:
            break;
    }

    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    /// CREATE GLOBAL OBJECTS

    return 0;
}
