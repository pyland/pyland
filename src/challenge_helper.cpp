#include <glm/vec2.hpp>
#include <memory>
#include <sstream>
#include <vector>
#include <fstream>
#include <iostream>

#include "entity.hpp"
#include "challenge.hpp"
#include "challenge_data.hpp"
#include "challenge_helper.hpp"
#include "engine.hpp"
#include "fml.hpp"
#include "make_unique.hpp"
#include "map.hpp"
#include "map_loader.hpp"
#include "map_object.hpp"
#include "map_viewer.hpp"
#include "interpreter.hpp"
#include "object_manager.hpp"

int ChallengeHelper::make_object(Challenge *challenge,
                                 std::string name,
                                 Walkability walkability,
                                 glm::ivec2 position) {
    VLOG(1) << "creating object at " << name << " (" << position.x << ", " << position.y << ")";
    return challenge->make_object(
        position,
        name,
        walkability
    );
}

PositionDispatcher<int>::CallbackID
ChallengeHelper::make_interaction(std::string name,
                                  std::function<bool (int)> callback) {

    LOG(INFO) << "creating interaction at " << name;
    auto *map = Engine::get_map_viewer()->get_map();
    auto properties(map->locations.at("Interactions/" + name));

    return map->event_step_on.register_callback(properties.position, callback);
}

PositionDispatcher<int>::CallbackID
ChallengeHelper::lose_interaction(std::string name,
                                  std::function<bool (int)> callback) {

    LOG(INFO) << "creating interaction at " << name;
    auto *map = Engine::get_map_viewer()->get_map();
    auto properties(map->locations.at("Interactions/" + name));

    return map->event_step_off.register_callback(properties.position, callback);
}
