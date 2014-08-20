#include <string>

#include "challenge.hpp"
#include "challenge_data.hpp"
#include "challenge_helper.hpp"
#include "engine.hpp"
#include "start_screen.hpp"
#include "map_object.hpp"
#include "object_manager.hpp"
#include "sprite.hpp"
#include "walkability.hpp"


#include <iostream>
StartScreen::StartScreen(ChallengeData *challenge_data): Challenge(challenge_data) {
    ChallengeHelper::make_sprite(this, "sprite/1","Ben", Walkability::BLOCKED);
    for (int i=1; i<=5; i++) {
        std::string name = "level/"+std::to_string(i);
        
        ChallengeHelper::make_interaction(name,
            [i] (int) {
                Engine::print_dialogue("Game","loading challenge "+std::to_string(i));
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