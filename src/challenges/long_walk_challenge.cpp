#include <glog/logging.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "api.hpp"
#include "challenge.hpp"
#include "event_manager.hpp"
#include "filters.hpp"
#include "input_manager.hpp"
#include "long_walk_challenge.hpp"
#include "engine_api.hpp"
#include "map_viewer.hpp"

//TODO: later this will be fetched from the map
std::map<std::string, std::vector<Vec2D>> targets = {
    {"start", {
        Vec2D(   4, 29-14)
    }},
    {"room:exit:first", {
        Vec2D(  11, 29-14),
        Vec2D(  11, 29-15)
    }},
    {"treasure:path:medium", {
        Vec2D(  24, 29-14)
    }},
    {"wall:path:medium", {
        Vec2D(  31, 29-13),
        Vec2D(  31, 29-14),
        Vec2D(  31, 29-15),
        Vec2D(  31, 29-16)
    }},
    {"treasure:path:long", {
        Vec2D( 114, 29-15)
    }},
    {"wall:path:long", {
        Vec2D( 116, 29-13),
        Vec2D( 116, 29-14),
        Vec2D( 116, 29-15),
        Vec2D( 116, 29-16)
    }},
    {"end", {
        Vec2D(1499, 29-14),
        Vec2D(1499, 29-15)
    }}
};

LongWalkChallenge::LongWalkChallenge(InputManager *input_manager): Challenge(input_manager) {
    auto *map = Engine::get_map_viewer()->get_map();

    // Set up blocking walls
    for (auto wall_location : targets.at("wall:path:medium")) {
        wall_path_medium_blockers.push_back(map->block_tile(wall_location));
    }

    for (auto wall_location : targets.at("wall:path:long")) {
        wall_path_long_blockers.push_back(map->block_tile(wall_location));
    }

    // Set up notifications about walls
    for (auto wall_location : targets.at("wall:path:medium")) {
        wall_path_medium_callbacks.push_back(map->event_step_on.register_callback(
            wall_location + Vec2D(-1, 0),
            [&] (int) {
                Engine::print_dialogue("Tom", "Get the treasure and press \"e\" to view it!\n");

                EventManager::get_instance().add_event([&] () {
                    auto *map = Engine::get_map_viewer()->get_map();
                    for (auto callback : wall_path_medium_callbacks) {
                        map->event_step_on.unregister(callback);
                    }
                });

                return false;
            }
        ));
    }

    for (auto wall_location : targets.at("wall:path:long")) {
        wall_path_long_callbacks.push_back(map->event_step_on.register_callback(
            wall_location + Vec2D(-1, 0),
            [&] (int) {
                Engine::print_dialogue("Tom", "Hey... you missed the treasure!\n");

                EventManager::get_instance().add_event([&] () {
                    auto *map = Engine::get_map_viewer()->get_map();
                    for (auto callback : wall_path_long_callbacks) {
                        map->event_step_on.unregister(callback);
                    }
                });

                return false;
            }
        ));
    }

    // Set up treasure chest triggers
    map->event_step_on.register_callback(
        targets.at("treasure:path:medium").front(),
        [&] (int) {
            Engine::print_dialogue ("Tom",
                "Who new treasure was so close?\n"
                "\n"
                "To open it up and see what's inside, press \"e\".\n"
            );

            editor_lifeline = this->input_manager->register_keyboard_handler(filter(
                {ANY_OF({KEY_HELD}), KEY({"E"})},
                [&] (KeyboardInputEvent) {

                    std::string id = std::to_string(Engine::get_map_viewer()->get_map_focus_object());
                    std::string filename = "John_" + id + ".py";
                    Engine::open_editor(filename);
                    wall_path_medium_blockers.clear();
                }
            ));

            return false;
        }
    );

    map->event_step_on.register_callback(
        targets.at("treasure:path:long").front(),
        [&] (int) {
            // TODO: Fancy output
            Engine::print_dialogue("Game",
                "Nice, more treasure!\n"
                "I doubt the next bit is going to be short enough to just walk...\n"
                "You should try to program it.\n"
            );

            wall_path_long_blockers.clear();
            return false;
        }
    );

    // Set up interaction triggers
    auto room_exit_first_lambda = [&] (int) {
        Engine::print_dialogue ("Tom",
            "Well, hello!\n"
            "I heard there was treasure to the right...\n"
            "You should look for it! It's probably really close!\n"
        );

        EventManager::get_instance().add_event([&] () {
            // Erase the callbacks
            auto *map = Engine::get_map_viewer()->get_map();
            for (auto callback : room_exit_first_callback) {
                map->event_step_on.unregister(callback);
            }
            room_exit_first_callback.clear();
        });

        // Don't want to repeat, but it
        // doesn't really matter what is returned
        return false;
    };

    for (auto interaction_location : targets.at("room:exit:first")) {
        room_exit_first_callback.push_back(
            // Register the event
            map->event_step_on.register_callback(
                interaction_location,
                room_exit_first_lambda
            )
        );
    }

    // Set up finish
    for (auto finish_location : targets.at("end")) {
        map->event_step_on.register_callback(
            finish_location,
            [&] (int) {
                Engine::print_dialogue("Game", "You Win, Well Done !");
                return true;
            }

            // TODO: FINISH SOMEHOW!!
        );
    }
}

void LongWalkChallenge::start() {
    Engine::print_dialogue ( "Tom",
        "Welcome to Level 1. The aim of this level to reach the green markers. You win when you reach the last one.\n"
        "My name is Tom and I am here to help you learn to move around \n"
        "You can move your character using arrow keys. Take a walk and look for the first green marker\n"
    );
}

void LongWalkChallenge::finish() {
    // TODO: Somehow finish challenge...
}