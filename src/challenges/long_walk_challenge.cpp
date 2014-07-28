#include <glog/logging.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "api.hpp"
#include "challenge.hpp"
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
        Vec2D(  30, 29-13),
        Vec2D(  30, 29-14),
        Vec2D(  30, 29-15),
        Vec2D(  30, 29-16)
    }},
    {"treasure:path:long", {
        Vec2D( 114, 29-15)
    }},
    {"wall:path:long", {
        Vec2D( 115, 29-13),
        Vec2D( 115, 29-14),
        Vec2D( 115, 29-15),
        Vec2D( 115, 29-16)
    }},
    {"end", {
        Vec2D(1499, 29-14),
        Vec2D(1499, 29-15)
    }}
};

LongWalkChallenge::LongWalkChallenge(std::string editor, Text* text_displayer): Challenge(editor,text_displayer) {
    auto *map = Engine::get_map_viewer()->get_map();

    // Set up blocking walls
    for (auto wall_location : targets.at("wall:path:medium")) {
        wall_path_medium_blockers.push_back(map->block_tile(wall_location));
    }

    for (auto wall_location : targets.at("wall:path:long")) {
        wall_path_long_blockers.push_back(map->block_tile(wall_location));
    }

    // Set up treasure chest triggers
    map->event_step_on.register_callback(
        targets.at("treasure:path:medium").front(),
        [&] (int) {
            dialogue ("TREASURE",
                "▞▀▖      ▗       ▐     ▛▀▖                   ▐    \n"
                "▙▄▌▛▀▖▞▀▖▄ ▞▀▖▛▀▖▜▀    ▌ ▌▞▀▖▞▀▖▌ ▌▛▚▀▖▞▀▖▛▀▖▜▀ ▐▌\n"
                "▌ ▌▌ ▌▌ ▖▐ ▛▀ ▌ ▌▐ ▖   ▌ ▌▌ ▌▌ ▖▌ ▌▌▐ ▌▛▀ ▌ ▌▐ ▖▗▖\n"
                "▘ ▘▘ ▘▝▀ ▀▘▝▀▘▘ ▘ ▀    ▀▀ ▝▀ ▝▀ ▝▀▘▘▝ ▘▝▀▘▘ ▘ ▀ ▝▘\n"
                "                                                  \n"
                "▛▀▖                     ▐                ▌        \n"
                "▙▄▘▙▀▖▞▀▖▞▀▘▞▀▘   ▞▀▖   ▜▀ ▞▀▖   ▞▀▖▞▀▖▞▀▌▞▀▖     \n"
                "▌  ▌  ▛▀ ▝▀▖▝▀▖   ▛▀    ▐ ▖▌ ▌   ▌ ▖▌ ▌▌ ▌▛▀      \n"
                "▘  ▘  ▝▀▘▀▀ ▀▀    ▝▀▘    ▀ ▝▀    ▝▀ ▝▀ ▝▀▘▝▀▘     \n"
            );

            // TODO: Add wall-lowering callback to "e"

            // TODO: Hook into keybinding and enable here
            // ... somehow!
            Engine::open_editor(editor, "John_1.py");

            wall_path_medium_blockers.clear();
            return false;
        }
    );

    map->event_step_on.register_callback(
        targets.at("treasure:path:long").front(),
        [&] (int) {
            // TODO: Fancy output

            wall_path_long_blockers.clear();
            return false;
        }
    );

    // Set up interaction triggers
    auto room_exit_first_lambda = [&] (int) {
        dialogue ("Tom",
            "Well, hello!\n"
            "I heard there was treasure to the right...\n"
            "You should look for it! It's probably really close!\n"
        );

        // Erase the callbacks
        auto *map = Engine::get_map_viewer()->get_map();
        for (auto callback : room_exit_first_callback) {
            map->event_step_on.unregister(callback);
        }
        room_exit_first_callback.clear();

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
                dialogue("Game",
                    "▄▄▄    ▄▄▄                              ▄▄      ▄▄  ▄▄▄▄▄▄   ▄▄▄   ▄▄     ▄▄\n" 
                    " ██▄  ▄██                               ██      ██  ▀▀██▀▀   ███   ██     ██\n" 
                    "  ██▄▄██    ▄████▄   ██    ██           ▀█▄ ██ ▄█▀    ██     ██▀█  ██     ██\n" 
                    "   ▀██▀    ██▀  ▀██  ██    ██            ██ ██ ██     ██     ██ ██ ██     ██\n" 
                    "    ██     ██    ██  ██    ██            ███▀▀███     ██     ██  █▄██     ▀▀\n" 
                    "    ██     ▀██▄▄██▀  ██▄▄▄███            ███  ███   ▄▄██▄▄   ██   ███     ▄▄\n" 
                    "    ▀▀       ▀▀▀▀     ▀▀▀▀ ▀▀            ▀▀▀  ▀▀▀   ▀▀▀▀▀▀   ▀▀   ▀▀▀     ▀▀\n" 
                );
                return true;
            }
                                                                                   
            // TODO: FINISH SOMEHOW!!
        );
    }
}

void LongWalkChallenge::start() {
    dialogue ( "Tom",
        "Welcome to Project Zgyote \n"
        "My name is Tom and I am here to help you learn to move around \n"
        "You can move Adam using arrow keys.\n"
        "\n"
        "Hey... why don't you come out here?\n"
    );
}

void LongWalkChallenge::finish() {
    // TODO: Somehow finish challenge...
}