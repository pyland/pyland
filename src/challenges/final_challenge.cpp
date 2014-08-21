#include <glog/logging.h>
#include <memory>

#include "final_challenge.hpp"
#include "challenge_helper.hpp"
#include "challenge_data.hpp"
#include "engine.hpp"
#include "object_manager.hpp"
#include "entitythread.hpp"
#include "api.hpp"
#include "object.hpp"
#include "interpreter.hpp"
#include "make_unique.hpp"

FinalChallenge::FinalChallenge(ChallengeData *challenge_data): Challenge(challenge_data) {
    Engine::print_dialogue( "Game", "Welcome to the final challenge");
    ChallengeHelper::make_sprite(this, "sprite/1","Ben", Walkability::BLOCKED);

    // set of orange collection part
    glm::ivec2 crate_location = ChallengeHelper::get_location_interaction("crate/1");
    glm::ivec2 dropoff_location = ChallengeHelper::get_location_interaction("dropoff/1");

    int num_of_oranges = 1;
    std::vector<int> orange_ids;

    for (int i = 1; i <= num_of_oranges; i++) {
        auto name = "orange/"+std::to_string(i);
        glm::ivec2 orange_location = ChallengeHelper::get_location_object(name);

        int orange_id = ChallengeHelper::make_object(this, name, Walkability::WALKABLE);
        ChallengeHelper::create_pickupable(orange_location, orange_location, crate_location, dropoff_location , orange_id);
        orange_ids.push_back(orange_id);
    }
    // TODO: fix then when joshua pushs fixes
    // creating the crocodile
    // LOG(INFO) << "creating croc";
    // int croc_id = ChallengeHelper::make_object(this, "sprite/crocodile", Walkability::BLOCKED);
    // auto croc = ObjectManager::get_instance().get_object<Object>(croc_id);
    // if (!croc) { return; }

    // // setting up python for croc

    // // Register user controled sprite
    // // Yes, this is a memory leak. Deal with it.
    // auto properties(map->locations.at("Objects/sprite/crocodile"));
    // auto *a_thing(new Entity(properties.location, "croc", croc_id));

    // LOG(INFO) << "Registering sprite";
    // croc->daemon = std::make_unique<LockableEntityThread>(challenge_data->interpreter->register_entity(*a_thing));
    // LOG(INFO) << "Done!";

    // croc->daemon->value->halt_soft(EntityThread::Signal::RESTART);
    ChallengeHelper::make_sprite(this, "sprite/crocodile","final_challenge_croc", Walkability::BLOCKED);
}

void FinalChallenge::start() {
}

void FinalChallenge::finish() {
    // TODO: Somehow finish challenge...
}
