#include <glog/logging.h>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "api.hpp"
#include "challenge.hpp"
#include "challenge_helper.hpp"
#include "event_manager.hpp"
#include "filters.hpp"
#include "input_manager.hpp"
#include "long_walk_challenge.hpp"
#include "engine.hpp"
#include "map_viewer.hpp"
#include "object_manager.hpp"
#include "map_object.hpp"
#include "walkability.hpp"
#include "sprite.hpp"

//TODO: later this will be fetched from the map
std::map<std::string, std::vector<glm::ivec2>> targets = {
    {"start", {
        glm::ivec2(   4, 29-14)
    }},
    {"room:exit:first", {
        glm::ivec2(  11, 29-14),
        glm::ivec2(  11, 29-15)
    }},
    {"treasure:path:medium", {
        glm::ivec2(  24, 29-14)
    }},
    {"wall:path:medium", {
        glm::ivec2(  31, 29-13),
        glm::ivec2(  31, 29-14),
        glm::ivec2(  31, 29-15),
        glm::ivec2(  31, 29-16)
    }},
    {"treasure:path:long", {
        glm::ivec2( 114, 29-15)
    }},
    {"wall:path:long", {
        glm::ivec2( 116, 29-13),
        glm::ivec2( 116, 29-14),
        glm::ivec2( 116, 29-15),
        glm::ivec2( 116, 29-16)
    }},
    {"end", {
        glm::ivec2(1499, 29-14),
        glm::ivec2(1499, 29-15)
    }}
};

LongWalkChallenge::LongWalkChallenge(InputManager *input_manager): Challenge(input_manager) {
    auto *map = Engine::get_map_viewer()->get_map();

    // testing micro-objects
    std::shared_ptr<MapObject> test_chest = std::make_shared<MapObject>(glm::ivec2(10, 15), "test chest", 52);
    ObjectManager::get_instance().add_object(test_chest);
    auto chest_id = test_chest->get_id();
    LOG(INFO) << "created test_chest with id: " << chest_id;
    map->add_map_object(chest_id);
    // TODO: Refixificate
    // test_chest->set_walkability(Walkability::WALKABLE);

    ChallengeHelper::create_pickupable(
        glm::ivec2(10, 15),
        glm::ivec2(10, 14),
        glm::ivec2(15, 15),
        glm::ivec2(15, 14),
        test_chest
    );

    // testing lawn
    auto lawn_area = { glm::ivec2(12, 16), glm::ivec2(13, 16), glm::ivec2(14, 16) };
    for (glm::ivec2 lawn_tile : lawn_area) {
        Engine::change_tile(lawn_tile, 5, 20);
        map->event_step_on.register_callback(
            lawn_tile,
            [test_chest,lawn_tile] (int) {
                int id = Engine::get_sprites_at(lawn_tile).front();
                bool has_chest = ObjectManager::get_instance().get_object<Sprite>(id)->is_in_inventory(test_chest);
                if (has_chest) {
                    Engine::print_dialogue("Grass","You're mowing, keep on going");
                    Engine::change_tile(lawn_tile, 5, 10);
                    return false;
                } else {
                    Engine::print_dialogue ("Grass","Don't forget the lawn mower");
                    return true;
                }
            }
        );
    }

    // Set up blocking walls
    for (auto wall_location : targets.at("wall:path:medium")) {
        std::shared_ptr<MapObject> wall = std::make_shared<MapObject>(wall_location, "medium wall",5);
        ObjectManager::get_instance().add_object(wall);
        // TODO: Refixificate
        // wall->set_walkability(Walkability::BLOCKED);
        wall_path_medium_objects.push_back(wall);
        map->add_map_object(wall->get_id());
    }

    for (auto wall_location : targets.at("wall:path:long")) {
        std::shared_ptr<MapObject> wall = std::make_shared<MapObject>(wall_location, "medium wall",5);
        ObjectManager::get_instance().add_object(wall);
        // TODO: Refixificate
        // wall->set_walkability(Walkability::BLOCKED);
        wall_path_long_objects.push_back(wall);
        map->add_map_object(wall->get_id());
    }

    // Set up notifications about walls
    for (auto wall_location : targets.at("wall:path:medium")) {
        wall_path_medium_callbacks.push_back(map->event_step_on.register_callback(
            wall_location + glm::ivec2(-1, 0),
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
            wall_location + glm::ivec2(-1, 0),
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
                    for (auto wall_object : wall_path_medium_objects) {
                        // TODO: Refixificate
                        // wall_object->set_walkability(Walkability::WALKABLE);
                        wall_object->set_sheet_id(119);
                    }}
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

            for (auto wall_object : wall_path_long_objects) {
                // TODO: Refixificate
                // wall_object->set_walkability(Walkability::WALKABLE);
                wall_object->set_sheet_id(119);
            }
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
