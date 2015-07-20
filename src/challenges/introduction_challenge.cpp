#include "python_embed_headers.hpp"

#include <boost/iterator/zip_iterator.hpp>
#include <boost/python.hpp>
#include <glog/logging.h>
#include <glm/vec2.hpp>
#include <memory>
#include <random>

#include "challenge_helper.hpp"
#include "challenge_data.hpp"
#include "dispatcher.hpp"
#include "engine.hpp"
#include "filters.hpp"
#include "input_manager.hpp"
#include "introduction_challenge.hpp"
#include "keyboard_input_event.hpp"
#include "map.hpp"
#include "map_object.hpp"
#include "object.hpp"
#include "object_manager.hpp"
#include "sprite.hpp"
#include "texture_atlas.hpp"

namespace py = boost::python;

IntroductionChallenge::IntroductionChallenge(ChallengeData *challenge_data): Challenge(challenge_data) {
    player = ChallengeHelper::make_sprite(this, "sprite/ben", "Ben", Walkability::BLOCKED, "south/still/1");

    auto player_obj(ObjectManager::get_instance().get_object<Sprite>(player));
    player_obj->set_instructions("Move left!");
    player_obj->set_challenge(this);

    Engine::get_map_viewer()->set_map_focus_object(player);

    treasure = ChallengeHelper::make_object(this, "treasure/at", Walkability::BLOCKED, "closed");
    pathblocker_top = ChallengeHelper::make_object(this, "pathblocker/top", Walkability::BLOCKED, "2");
    pathblockers_bottom.emplace_back(ChallengeHelper::make_object(this, "pathblocker/bottom/1", Walkability::BLOCKED, "2"));
    pathblockers_bottom.emplace_back(ChallengeHelper::make_object(this, "pathblocker/bottom/2", Walkability::BLOCKED, "2"));
    pathblockers_bottom.emplace_back(ChallengeHelper::make_object(this, "pathblocker/bottom/3", Walkability::BLOCKED, "2"));

    Engine::print_dialogue(
        "Ben",
        "Woa, this is a big forest.\n"
        "Let's go on an adventure!"
    );

    editor_lifeline = this->challenge_data->input_manager->register_keyboard_handler(filter(
        {KEY_PRESS, KEY("E")},
        [this] (KeyboardInputEvent) {

            auto player_obj(ObjectManager::get_instance().get_object<Sprite>(player));
            player_obj->set_instructions(
                "Change the script so that you move along the path, and hold Shift to go fast.\n"
                "\n"
                "You need to find the code in the editor that does this and remove the \"#\"s "
                "infront of it. Then press \"r\" to run it and \"h\" to make it stop."
            );

            editor_lifeline = this->challenge_data->input_manager->register_keyboard_handler(filter(
                {KEY_PRESS, KEY("R")},
                [this] (KeyboardInputEvent) {
                    auto pathblocker(ObjectManager::get_instance().get_object<MapObject>(pathblocker_top));

                    pathblocker->set_tile(TextureAtlas::from_name("test/blank"));
                    pathblocker->set_walkability(Walkability::WALKABLE);
                }
            ));
        }
    ));

    ChallengeHelper::make_interaction("howto/run_scripts", [this] (int) {
        Engine::print_dialogue(
            "Random Note",
            "Just walking there will take ages!\n"
            "Here's a better way. Press \"e\"."
        );

        auto player_obj(ObjectManager::get_instance().get_object<Sprite>(player));
        player_obj->set_instructions("Remember to press \"e\" to open the editor.");

        return false;
    });

    ChallengeHelper::make_interaction("row/complete/first", [this] (int) {
        Engine::print_dialogue(
            "Ben",
            "Programming's much faster than scripting...\n"
            "\n"
            "Well, I can't wait to see what's up ahead.\n"
            "I have a feeling it's ADVENTURE!\n"
        );

        auto player_obj(ObjectManager::get_instance().get_object<Sprite>(player));
        player_obj->set_instructions(
            "You should follow that path... I think there's treasure there."
        );

        return false;
    });

    ChallengeHelper::make_interaction("treasure/above", [this] (int) {
        Engine::print_dialogue(
            "Ben",
            "Damn! There are loads of locks on this!\n"
            "I'll remember to come back here if I find the keys...\n"
            "\n"
            "For now, let's go west."
        );

        auto player_obj(ObjectManager::get_instance().get_object<Sprite>(player));
        player_obj->set_instructions(
            "Change east to west in the script and try running it again."
        );

        return false;
    });

    ChallengeHelper::make_interaction("row/complete/second", [this] (int) {
        Engine::print_dialogue(
            "Ben",
            "There's a turn here. I wonder what it could be.\n"
            "\n"
            "It's best I don't rush..."
        );

        auto player_obj(ObjectManager::get_instance().get_object<Sprite>(player));
        player_obj->set_instructions(
            "Go downwards and see what's there."
        );

        return false;
    });

    ChallengeHelper::make_interaction("howto/search_logs", [this] (int) {
        Engine::print_dialogue(
            "Secret note:",
            "Arr, me pirates and I werr lookin' ferr some rum "
            "an' we LOST OUR TREASURE.\n"
            "\n"
            "Now nobody's sure wherre it be gone, so we hid ourr keyys "
            "amongst the logs.\n"
            "\n"
            "You'll need to go through them IN ORDER else ye' won't "
            "know what ta' look ferr, but each log has directions for finding "
            "the next one."
        );

        auto player_obj(ObjectManager::get_instance().get_object<Sprite>(player));
        player_obj->set_instructions(
            "When on a the location of a hidden message, run:\n"
            "\n"
            "    direction, steps = read_message()\n"
            "\n"
            "You can then use your previous code to move in the direction "
            "given, and by the number of steps given.\n"
            "\n"
            "Remember, \"r\" to run the script and Shift to increase your "
            "speed while running a script."
        );

        return false;
    });

    std::vector<std::string> locations({
        "log/1", "log/2", "log/3", "log/4", "log/5", "log/6", "log/7", "log/8", "log/9", "log/10",
        "entrance",
        "hidden_message/1", "hidden_message/2", "hidden_message/3", "hidden_message/4",
        "hidden_message/5", "hidden_message/6", "hidden_message/7", "hidden_message/8",
        "entrance"
    });

    keys_found = 0;
    int total_number_of_keys(int(locations.size()) - 1);

    auto *map = Engine::get_map_viewer()->get_map();
    auto zipped_locations_begin(boost::make_zip_iterator(boost::make_tuple(
        std::begin(locations), std::next(std::begin(locations))
    )));
    auto zipped_locations_end(boost::make_zip_iterator(boost::make_tuple(
        std::prev(std::end(locations)), std::end(locations)
    )));

    int i(0);
    for (auto pair=zipped_locations_begin; pair != zipped_locations_end; ++ pair) {
        ++i;
        std::string this_location(pair->get<0>());
        std::string next_location(pair->get<1>());

        auto this_properties(map->locations.at("Interactions/" + this_location));
        auto next_properties(map->locations.at("Interactions/" + next_location));

        auto diff(next_properties.location - this_properties.location);

        map_location_displacements[this_properties.location] = diff;

        int amount;
        std::string direction;
        if      (diff.x > 0) { amount = +diff.x; direction = "east";  }
        else if (diff.x < 0) { amount = -diff.x; direction = "west";  }
        else if (diff.y > 0) { amount = +diff.y; direction = "north"; }
        else                 { amount = -diff.y; direction = "south"; }

        ChallengeHelper::make_interaction(this_location, [this, total_number_of_keys, this_location, i, direction, amount] (int) {
            if (keys_found + 1 < i) {
                // If starts with "log", so is visible
                if (this_location.rfind("log/", 0) == 0 || this_location == "entrance") {
                    Engine::print_dialogue(
                        "Log message",
                        "You look but you don't find anything.\n"
                        "\n"
                        "You're probably on the wrong track; you should start again.\n"
                    );

                    auto player_obj(ObjectManager::get_instance().get_object<Sprite>(player));
                    player_obj->set_instructions(
                        "You're in the wrong place.\n"
                        "Consider going back to the start and trying again.\n"
                        "\n"
                        "When on a the location of a hidden message, run:\n"
                        "\n"
                        "    direction, steps = read_message()\n"
                        "\n"
                        "You can then use your previous code to move in the direction "
                        "given, and by the number of steps given.\n"
                    );
                }
            }
            else if (keys_found + 1 == i) {
                if (this_location == "entrance") {
                    Engine::print_dialogue(
                        "Log message",
                        "You found key number " + std::to_string(i) + ".\n"
                        "\n"
                        "Here's where the next one is:\n"
                        "        Direction: " + direction + "\n"
                        "        Amount: " + std::to_string(amount) + "\n"
                        "...\n"
                        "We arr tirrred of putting ye messages in logs,\n"
                        "so we're just going to HIDE YARR MESSAGES UNDERGROUND.\n"
                    );

                    auto player_obj(ObjectManager::get_instance().get_object<Sprite>(player));
                    player_obj->set_instructions(
                        "This is the same as before, but now the locations "
                        "of messages aren't marked by logs, so you have to trust "
                        "the program!\n"
                        "\n"
                        "When on a the location of a hidden message, run:\n"
                        "\n"
                        "    direction, steps = read_message()\n"
                        "\n"
                        "You can then use your previous code to move in the direction "
                        "given, and by the number of steps given.\n"
                        "\n"
                        "Remember, \"r\" to run the script and Shift to increase your "
                        "speed while running a script."
                    );
                }
                else {
                    Engine::print_dialogue(
                        "Log message",
                        "You found key number " + std::to_string(i) + ".\n"
                        "\n"
                        "Here's where the next one is:\n"
                        "        Direction: " + direction + "\n"
                        "        Amount: " + std::to_string(amount) + "\n"
                    );

                    auto player_obj(ObjectManager::get_instance().get_object<Sprite>(player));
                    player_obj->set_instructions(
                        "When on a the location of a hidden message, run:\n"
                        "\n"
                        "    direction, steps = read_message()\n"
                        "\n"
                        "You can then use your previous code to move in the direction "
                        "given, and by the number of steps given.\n"
                        "\n"
                        "Remember, \"r\" to run the script and Shift to increase your "
                        "speed while running a script."
                    );
                }

                ++keys_found;
            }
            else if (keys_found == total_number_of_keys && this_location == "entrance" && !pathblockers_bottom.empty()) {
                Engine::print_dialogue(
                    "Ben",
                    "I'm back here... so I'm probably done!\n"
                    "\n"
                    "...\n"
                    "\n"
                    "Wow! That entrance just OPENED!"
                    "I want to explore.\n"
                );

                for (auto pathblocker_bottom : pathblockers_bottom) {
                    auto pathblocker(ObjectManager::get_instance().get_object<MapObject>(pathblocker_bottom));

                    pathblocker->set_tile(TextureAtlas::from_name("test/blank"));
                    pathblocker->set_walkability(Walkability::WALKABLE);
                }

                pathblockers_bottom.clear();

                auto player_obj(ObjectManager::get_instance().get_object<Sprite>(player));
                player_obj->set_instructions("Go down!\n");
            }
            else {
                Engine::print_dialogue(
                    "Log message",
                    "You've already found key number " + std::to_string(i) + " here.\n"
                    "\n"
                    "Here's where the next one is:\n"
                    "        Direction: " + direction + "\n"
                    "        Amount: " + std::to_string(amount) + "\n"
                );

                auto player_obj(ObjectManager::get_instance().get_object<Sprite>(player));
                player_obj->set_instructions(
                    "You've already been here, but you can continue anyway.\n"
                    "\n"
                    "When on a the location of a hidden message, run:\n"
                    "\n"
                    "    direction, steps = read_message()\n"
                    "\n"
                    "You can then use your previous code to move in the direction "
                    "given, and by the number of steps given.\n"
                );
            }

            return true;
        });
    }

    ChallengeHelper::make_interaction("alert/goleft", [this] (int) {
        Engine::print_dialogue(
            "Secret note:",
            "Arr, the last key is up ahead.\n"
        );

        auto player_obj(ObjectManager::get_instance().get_object<Sprite>(player));
        player_obj->set_instructions("Go left!");

        return false;
    });

    ChallengeHelper::make_interaction("howto/find_key", [this] (int) {
        Engine::print_dialogue(
            "Secret note:",
            "Arr, the last key was up ahead but we lost it.\n"
            "\n"
            "Yarr going to need to search the whole path of land\n"
            "if ye want me booty.\n"
            "\n"
            "It's 100 steps across and almost 50 tall!\n"
            "Ye could get lost in it!\n"
        );

        auto player_obj(ObjectManager::get_instance().get_object<Sprite>(player));
        player_obj->set_instructions(
            "You need to put a loop for going sideways inside a loop for going down.\n"
            "\n"
            "Try something like this:\n"
            "\n"
            "    do this 20 times:\n"
            "        go down\n"
            "        go right 100 times\n"
            "        go down\n"
            "        go left 100 times\n"
            "\n"
        );

        return false;
    });

    ChallengeHelper::make_interaction("lost_key", [this] (int) {
        Engine::print_dialogue(
            "Ben:",
            "I FOUND IT! I FOUND THE KEY!\n"
            "\n"
            "Good thing I remember how to retrace my steps\n"
            "to the treasure chest!\n"
        );

        auto player_obj(ObjectManager::get_instance().get_object<Sprite>(player));
        player_obj->set_instructions(
            "You need to retrace your steps.\n"
            "\n"
            "You can get a list of steps to retrace by running:\n"
            "\n"
            "    steps = get_retrace_steps()\n"
            "\n"
            "Then you can go through them like this:\n"
            "\n"
            "    for direction in steps:\n"
            "        # move in direction of step\n"
            "\n"
            "You want to walk in the move of the step each time, such as with:\n"
            "\n"
            "        move(direction)\n"
        );

        ChallengeHelper::make_interaction("treasure/above", [this] (int) {
            auto treasure_obj(ObjectManager::get_instance().get_object<MapObject>(treasure));
            treasure_obj->set_tile(treasure_obj->frames.get_frame("open"));

            Engine::print_dialogue(
                "Ben",
                "...\n"
                "\n"
                "...\n"
                "\n"
                "...\n"
                "\n"
                "TREASURE!\n"
                "\n"
                "(exiting in 10 seconds)\n"
            );

            auto player_obj(ObjectManager::get_instance().get_object<Sprite>(player));
            player_obj->set_instructions("Not much to do now but wait...");

            ChallengeHelper::set_completed_level(1);

            EventManager::get_instance()->add_timed_event(GameTime::duration(10.0), [this] (float completion) {
                if (completion == 1.0) {
                    finish();
                }
                return true;
            });

            return false;
        });

        return false;
    });
}

void IntroductionChallenge::start() {
}

void IntroductionChallenge::finish() {
    ChallengeHelper::set_completed_level(1);
    event_finish.trigger(0);
}

py::object IntroductionChallenge::read_message(int id) const {
    auto object(ObjectManager::get_instance().get_object<MapObject>(id));
    if (map_location_displacements.find(object->get_position()) == std::end(map_location_displacements)) {
        return py::object();
    }

    auto displacement(map_location_displacements.at(object->get_position()));
    // Requires either x or y to be 0, and positions must be integral
    return py::make_tuple(
        py::make_tuple(
            (displacement.x > 0) - (0 > displacement.x),
            (displacement.y > 0) - (0 > displacement.y)
        ),
        py::object(abs(int(displacement.x + displacement.y)))
    );
}
