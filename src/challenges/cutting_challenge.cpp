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
    ChallengeHelper::make_object(this, "sprite/farmer", Walkability::BLOCKED, "south/still/1");
    ChallengeHelper::make_object(this, "sprite/gardener", Walkability::BLOCKED, "west/still/1");
    ChallengeHelper::make_object(this, "sprite/monkey", Walkability::BLOCKED, "south/still/1");
    Engine::get_map_viewer()->set_map_focus_object(player);


    // int object_trigger = ChallengeHelper::make_object(this, "sprite/gardener", Walkability::BLOCKED, "west/still/1");
    // ObjectProperties properties(map->locations.at("Objects/sprite/crocodile"));
    glm::ivec2 trigger_objective_location = ChallengeHelper::get_location_interaction("trigger/objective/start");
    ChallengeHelper::make_interaction("trigger/objective/start", [trigger_objective_location] (int) {
            Engine::print_dialogue ("Gardener","We need more space for our farm, but these vines grow back faster than I can cut them down. Maybe you can do a better job? We will gladdly share some of our food with you if you cut down all of the vines.");
            return true;
        });

    vine_count = 0;
    int w(map->get_width());
    int h(map->get_height());
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (map->query_tile(x, y, "Interaction") == std::string("interactable/vines/cut/3")) {
                ++vine_count;
                vine_spots.push_back(glm::ivec2{x, y});
                map->event_step_on.register_callback(glm::ivec2({x, y}), [this, x, y] (int) {
                        map->update_tile(x, y, "Interaction", "test/blank");
                        --vine_count;
                        Engine::print_dialogue ("Vines remaining", std::to_string(vine_count));
                        return true;
                    });
            }
        }
    }
    uniform_spot_gen = std::uniform_int_distribution<uint32_t>(0, vine_count);

    // std::function<void()> regrow([this, regrow] () {
    //         int i;
    //         i = uniform_spot_gen(random_generator);
    //         glm::ivec2 spot(vine_spots[i]);
    //         if (map->query_tile(spot.x, spot.y, "Interaction") == std::string("test/blank.png")) {
    //             map->update_tile(spot.x, spot.y, "Interaction", "interactable/vines/cut/3");
    //         }
    //         if (vine_count > 0) {
    //             EventManager::get_instance().add_event_next_frame(regrow);
    //         }
    //     });
    // EventManager::get_instance().add_event_next_frame(regrow);
}

void CuttingChallenge::start() {
}

void CuttingChallenge::finish() {
}
