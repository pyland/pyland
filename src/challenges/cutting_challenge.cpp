#include <glog/logging.h>
#include <memory>
#include <random>

#include <glm/vec2.hpp>

#include "api.hpp"
#include "cutting_challenge.hpp"
#include "challenge_helper.hpp"
#include "challenge_data.hpp"
#include "dispatcher.hpp"
#include "engine.hpp"
#include "map.hpp"
#include "object.hpp"
#include "object_manager.hpp"



static std::mt19937 random_generator;



CuttingChallenge::CuttingChallenge(ChallengeData *challenge_data): Challenge(challenge_data) {
    Engine::print_dialogue("Ben", "I think I see a farm up ahead...");
    int player = ChallengeHelper::make_sprite(this, "sprite/1","Ben", Walkability::BLOCKED, "south/still/1");
    ChallengeHelper::make_object(this, "sprite/advisor", Walkability::BLOCKED, "south/still/1");
    ChallengeHelper::make_object(this, "sprite/farmer", Walkability::BLOCKED, "south/still/1");
    ChallengeHelper::make_object(this, "sprite/gardener", Walkability::BLOCKED, "west/still/1");
    int monkey = ChallengeHelper::make_object(this, "sprite/monkey", Walkability::BLOCKED, "south/still/1");
    Engine::get_map_viewer()->set_map_focus_object(player);


    ChallengeHelper::make_interaction("trigger/objective/finish", [this] (int) {
            ChallengeHelper::set_completed_level(2);
            finish();
            return false;
        });

    ChallengeHelper::make_interaction("trigger/advice/monkey", [this] (int) {
            if (has_bananas) {
                Engine::print_dialogue ("Villager","Hey, you've got some bananas! Maybe you can get use them to persuade that monkey to move...");
            }
            else {
                Engine::print_dialogue ("Villager","That monkey up north has been blocking that path for ages now. He usually goes away when we give him a banana or two, but we've been rather short on food recently. If you're lucky, someone around the farm might have some to spare.");
                has_bananas = true;
            }
            return true;
        });

    ChallengeHelper::make_interaction("trigger/objective/gardener", [this] (int) {
            if (vine_count > 0) {
                Engine::print_dialogue ("Gardener","We need more space for our farm, but these vines grow back faster than I can cut them down. Maybe you can do a better job? We will gladdly share some of our bananas with you if you cut down all of the vines.");
            }
            else {
                Engine::print_dialogue ("Gardener","Thanks for cutting those vines! Now we have more farmland. Go see the banana farmer, I'm sure he can now spare you some of our produce.");
            }
            return true;
        });

    ChallengeHelper::make_interaction("trigger/objective/farmer", [this] (int) {
            if (vine_count > 0) {
                Engine::print_dialogue ("Farmer","Sorry, I can't spare any bananas. Unless we expand our farmland soon, we'll run out food supplies.");
            }
            else {
                Engine::print_dialogue ("Farmer","Alright! With all those vines gone, we'll be able to grow more bananas! Mmmm... banaaanaaas... Do you like bananas? Here, take a bunch of them - we can grow more now.");
                has_bananas = true;
            }
            return true;
        });

    ChallengeHelper::make_interaction("trigger/objective/monkey", [this, monkey] (int) {
            if (has_bananas) {
                Engine::print_dialogue ("Monkey","Oop-a-...... Bananas? Now you're speaking my language!");
                Engine::move_object(monkey, glm::vec2({1, 0}));
                return false;
            }
            else {
                Engine::print_dialogue ("Monkey", monkey_say());
                return true;
            }
        });

    has_bananas = false;
    vine_count = 0;
    int w(map->get_width());
    int h(map->get_height());
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (map->query_tile(x, y, "Interaction") == std::string("interactable/vines/cut/3")) {
                vine_spots.push_back(glm::ivec2{x, y});
                ++vine_count;
                map->event_step_on.register_callback(glm::ivec2({x, y}), [this, x, y] (int) {
                        map->update_tile(x, y, "Interaction", "test/blank");
                        --vine_count;
                        Engine::print_dialogue ("Vines remaining", std::to_string(vine_count));
                        return false;
                    });
            }
        }
    }
    uniform_spot_gen = std::uniform_int_distribution<uint32_t>(0, vine_count);
    uniform_direction_gen = std::uniform_int_distribution<uint32_t>(0, 4);

    regrow = std::function<void()>([this] () {
            if (vine_count > 0) {
                // Find a vine to grow from.
                int i = uniform_spot_gen(random_generator);
                glm::ivec2 spot(vine_spots[i]);
                VLOG(3) << spot.x << " REGEN! " << spot.y;
                if (map->query_tile(spot.x, spot.y, "Interaction") == std::string("interactable/vines/cut/3")) {
                    map->update_tile(spot.x, spot.y, "Interaction", "interactable/vines/cut/4");
                }
                else if (map->query_tile(spot.x, spot.y, "Interaction") == std::string("interactable/vines/cut/4")) {
                    map->update_tile(spot.x, spot.y, "Interaction", "interactable/vines/grown/4");
                }
                else if (map->query_tile(spot.x, spot.y, "Interaction") == std::string("interactable/vines/grown/4")) {
                    map->update_tile(spot.x, spot.y, "Interaction", "interactable/vines/grown/3");
                    grow_out(spot.x, spot.y);
                }
                else if (map->query_tile(spot.x, spot.y, "Interaction") == std::string("interactable/vines/grown/3")) {
                    grow_out(spot.x, spot.y);
                }
                // Wait one second before triggering another regrowth.
                EventManager::get_instance().add_timed_event(GameTime::duration(0.025), [this] (float completion) {
                        if (completion == 1.0) {
                            EventManager::get_instance().add_event_next_frame(regrow);
                        }
                        return true;
                    });
            } else {
                Engine::print_dialogue ("Gardener", "Hey, you did it! Meet me back here to talk...");
            }
        });
    EventManager::get_instance().add_event_next_frame(regrow);
}

void CuttingChallenge::start() {
}

void CuttingChallenge::finish() {
    ChallengeHelper::set_completed_level(2); 
    event_finish.trigger(0);
}



void CuttingChallenge::grow_out(int x, int y) {
    // Grow out in all directions.
    for (int i2 = 0; i2 < 4; ++i2) {
        // Target positions
        int tx = x;
        int ty = y;
        switch (i2) {
        case 0:
            --tx;
            break;
        case 1:
            --ty;
            break;
        case 2:
            ++tx;
            break;
        case 3:
            ++ty;
            break;
        }
        if (tx > 0 && ty > 0 && tx < map->get_width() && ty < map->get_height()) {
            if (map->query_tile(tx, ty, "Interaction") == std::string("test/blank")) {
                map->update_tile(tx, ty, "Interaction", "interactable/vines/cut/3");
                map->event_step_on.register_callback(glm::ivec2({tx, ty}), [this, tx, ty] (int) {
                        map->update_tile(tx, ty, "Interaction", "test/blank");
                        --vine_count;
                        Engine::print_dialogue ("Vines remaining", std::to_string(vine_count));
                        return false;
                    });
                ++vine_count;
                Engine::print_dialogue ("Vines remaining", std::to_string(vine_count));
            }
        }
    }
}


std::string CuttingChallenge::monkey_say() {
    std::uniform_int_distribution<uint32_t> uniform_monkey_sentence(1,4);
    std::uniform_int_distribution<uint32_t> uniform_monkey_phrases(1,6);
    std::uniform_int_distribution<uint32_t> uniform_monkey_sounds(1,4);
    std::uniform_int_distribution<uint32_t> uniform_monkey_syllables(0,4);
    std::uniform_int_distribution<uint32_t> uniform_monkey_pauses(0,1);
    std::uniform_int_distribution<uint32_t> uniform_monkey_ends(0,2);
    std::stringstream message;
    // Sentence limit.
    int sentences = uniform_monkey_sentence(random_generator);
    for (int sentence = 0; sentence < sentences; ++sentence) {
        // Phrase limit.
        int phrases = uniform_monkey_phrases(random_generator);
        for (int phrase = 0; phrase < phrases; ++phrase) {
            // Sound limit.
            int sounds = uniform_monkey_sounds(random_generator);
            for (int sound = 0; sound < sounds; ++sound) {
                if (phrase == 0 && sound == 0) {
                    switch (uniform_monkey_syllables(random_generator)) {
                    case 0:
                        message << "Oop";
                        break;
                    case 1:
                        message << "Ack";
                        break;
                    case 2:
                        message << "Eek";
                        break;
                    case 3:
                        message << "Aa";
                        break;
                    case 4:
                        message << "Oo";
                        break;
                    }
                }
                else {
                    switch (uniform_monkey_syllables(random_generator)) {
                    case 0:
                        message << "oop";
                        break;
                    case 1:
                        message << "ack";
                        break;
                    case 2:
                        message << "eek";
                        break;
                    case 3:
                        message << "aa";
                        break;
                    case 4:
                        message << "oo";
                        break;
                    }
                }
                if (sound != sounds - 1) {
                    message << "-";
                }
            }
            if (phrase != phrases - 1) {
                switch (uniform_monkey_pauses(random_generator)) {
                case 0:
                    message << " ";
                    break;
                case 1:
                    message << ", ";
                    break;
                }
            }
        }

        switch (uniform_monkey_ends(random_generator)) {
        case 0:
            message << ".";
            break;
        case 1:
            message << "!";
            break;
        case 2:
            message << "?";
            break;
        }
        if (sentence != sentences - 1) {
            message << " ";
        }
    }
    return message.str();
}
