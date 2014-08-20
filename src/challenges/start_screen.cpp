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

int encoded_levels = 2;


StartScreen::StartScreen(ChallengeData *challenge_data): Challenge(challenge_data) {
    ChallengeHelper::make_sprite(this, "sprite/1","Ben", Walkability::BLOCKED);
    for (int i=1; i<=5; i++) {
        std::string name = "level/"+std::to_string(i);

        ChallengeHelper::make_interaction(name,
            [i,this] (int) {
                int current_level = ChallengeHelper::get_current_level();
                if (i==current_level) {
                    Engine::print_dialogue(
                        "Game","loading challenge "+std::to_string(i));
                    event_finish.trigger(i);

                } else if (i<current_level) {
                    Engine::print_dialogue(
                        "Game","You've finish this level. but feel free to have another go");

                } else if (i<=encoded_levels) {
                    Engine::print_dialogue(
                        "Game","Sorry this level is not avaliable to you yet. \n"
                        "Please finish level "+std::to_string(current_level)+ " first.");

                } else {
                    Engine::print_dialogue(
                        "Game","Help us build a new game level to go here,"
                        "See github.com/pyland/pyland for more information.");
                }
                return true;
            }
        );
    }
}

void StartScreen::start() {
    Engine::print_dialogue ( "Game",
        "Welcome to Pyland, walk to one of the orange icons to select a level\n"
    );
}

void StartScreen::finish() {
    // TODO: Somehow finish challenge...
}
