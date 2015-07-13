#define GLM_FORCE_RADIANS

#include <glog/logging.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <chrono>
#include <functional>
#include <glm/vec2.hpp>
#include <iostream>
#include <memory>
#include <random>
#include <ratio>
#include <string>
#include <sstream>
#include <utility>
#include <vector>

#include "game_init.hpp"
#include "button.hpp"
#include "callback_state.hpp"
#include "challenge_data.hpp"
#include "cutting_challenge.hpp"
#include "engine.hpp"
#include "event_manager.hpp"
#include "filters.hpp"
#include "final_challenge.hpp"
#include "game_window.hpp"
#include "gui_manager.hpp"
#include "gui_window.hpp"
#include "input_manager.hpp"
#include "interpreter.hpp"
#include "introduction_challenge.hpp"
#include "keyboard_input_event.hpp"
#include "lifeline.hpp"
#include "map_viewer.hpp"
#include "mouse_cursor.hpp"
#include "mouse_input_event.hpp"
#include "mouse_state.hpp"
#include "new_challenge.hpp"
#include "notification_bar.hpp"
#include "sprite.hpp"
#include "start_screen.hpp"

#ifdef USE_GLES
#include "typeface.hpp"
#include "text_font.hpp"
#include "text.hpp"
#endif

using namespace std;

static std::mt19937 random_generator;
Challenge* pick_challenge(ChallengeData* challenge_data);
int main(int argc, char *argv[]) {

    std::string map_path("../maps/start_screen.tmx");

    game_init(argc, argv);
    switch (argc) {
        default:
            std::cout << "Usage: " << argv[0] << " [EDITOR] [MAP]" << std::endl;
            return 1;

        // The lack of break statements is not an error!!!
        case 3:
            map_path = std::string(argv[2]);
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
