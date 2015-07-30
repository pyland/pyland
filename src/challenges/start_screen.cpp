#include <string>
#include <fstream>
#include <iostream>

#include "challenge.hpp"
#include "challenge_data.hpp"
#include "challenge_helper.hpp"
#include "engine.hpp"
#include "start_screen.hpp"
#include "map_object.hpp"
#include "object_manager.hpp"
#include "sprite.hpp"
#include "walkability.hpp"

// this int refers to the number of levels which have been developed, after this
// level number we just direct the user to github
// CHANGE THIS NUMBER WHEN YOU INTRODUCE A NEW LEVEL
int encoded_levels = 4;


StartScreen::StartScreen(ChallengeData *challenge_data): Challenge(challenge_data) {
    //create main character
    ChallengeHelper::make_sprite(this, "sprite/1", "Ben", Walkability::BLOCKED, "east/still/1");

    //set up level selection for the 5 level selction points on the map
    for (int i=1; i<=4; i++) {
        std::string name = "level/"+std::to_string(i);
        ChallengeHelper::make_interaction(name,
            [i, this] (int) {
                int current_level = ChallengeHelper::get_current_level();
                if (i<=current_level) {
                    VLOG(2) << "Playable level has been selected";
                    Engine::print_dialogue("Game", "loading challenge " + std::to_string(i));
                    event_finish.trigger(i);
                }
                else if (i<=encoded_levels) {
                    //Remove the comments, put this code back in to restrict level access
                    /*VLOG(2) << "Unplayable level has been selected";
                    Engine::print_dialogue(
                        "Game","Sorry this level is not avaliable to you yet. \n"
                        "Please finish level " + std::to_string(current_level) + " first."
                    );*/
                    //Temporary code to provide access to all levels for testing
                    VLOG(2) << "Playable level has been selected";
                    Engine::print_dialogue("Game", "loading challenge " + std::to_string(i));
                    event_finish.trigger(i);
                }
                else {
                    VLOG(2) << "Unimplemented level has been selected";
                    Engine::print_dialogue(
                        "Game","Help us build a new game level to go here, \n"
                        "See github.com/pyland/pyland for more information."
                    );
                }
                return true;
            }
        );
    }
}

void StartScreen::start() {
    Engine::print_dialogue ( "Game",
        "Welcome to Pyland, walk to one of the orange icons to select a level.\n"
        "You can walk using the arrow keys or the WASD keys.\n"
    );
}

void StartScreen::finish() {
}
