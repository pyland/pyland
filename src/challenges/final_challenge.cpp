#include <glog/logging.h>
#include <memory>
#include <thread>
#include <chrono>

#include "final_challenge.hpp"
#include "challenge_helper.hpp"
#include "challenge_data.hpp"
#include "challenge.hpp"
#include "engine.hpp"
#include "object_manager.hpp"
#include "entitythread.hpp"
#include "api.hpp"
#include "object.hpp"
#include "interpreter.hpp"
#include "make_unique.hpp"

FinalChallenge::FinalChallenge(ChallengeData *challenge_data): Challenge(challenge_data) {
    Engine::print_dialogue( "Game", "Welcome to the final challenge");

    //creating monkey helper
    ChallengeHelper::make_sprite(
        this,
        "sprite/monkey",
        "Milo",
        Walkability::BLOCKED,
        "south/still/1"
    );


    //create the main game character
    ChallengeHelper::make_sprite(
        this,
        "sprite/1",
        "Ben",
        Walkability::BLOCKED,
        "south/still/1"
    );

    // set of orange collection part
    glm::ivec2 crate_location = ChallengeHelper::get_location_interaction("crate/orange");
    glm::ivec2 dropoff_location = ChallengeHelper::get_location_interaction("dropoff/orange");

    int num_of_oranges = 5;
    std::vector<int> orange_ids;

    // adding oranges as pickupable objects
    for (int i = 1; i <= num_of_oranges; i++) {
        auto name = "orange/"+std::to_string(i);
        glm::ivec2 orange_location = ChallengeHelper::get_location_object(name);

        int orange_id = ChallengeHelper::make_object(this, name, Walkability::WALKABLE, "orange");
        ChallengeHelper::create_pickupable(orange_location, orange_location, crate_location, dropoff_location , orange_id);
        orange_ids.push_back(orange_id);
    }

    // checking if all oranges are in crate
    ChallengeHelper::make_interaction("dropoff/orange", [dropoff_location, crate_location, orange_ids,this] (int) {

        if (Engine::is_objects_at(crate_location, orange_ids)) {
            Engine::print_dialogue ("Game","Well Done, all the oranges are in the crate");
            finish();
        } else {
            Engine::print_dialogue ("Game","Keep going");
        }
        return true;
    });

    // challenge exit
    ChallengeHelper::make_interaction("exit", [this] (int) {
        finish();
        LOG(INFO) << "exiting";
        return false;
    });

    // creating croc
    LOG(INFO) << "creating croc";
    int croc_id = ChallengeHelper::make_object(
        this,
        "sprite/crocodile",
        Walkability::BLOCKED,
        "south/still/1"
    );

    auto croc = ObjectManager::get_instance().get_object<Object>(croc_id);
    if (!croc) { return; }

    // setting up python for croc

    // Register user controled sprite
    // Yes, this is a memory leak. Deal with it.
    auto properties(map->locations.at("Objects/sprite/crocodile"));
    auto *a_thing(new Entity(properties.location, "final_challenge_croc", croc_id));

    LOG(INFO) << "Registering sprite";
    croc->daemon = std::make_unique<LockableEntityThread>(challenge_data->interpreter->register_entity(*a_thing));
    LOG(INFO) << "Done!";

    croc->daemon->value->halt_soft(EntityThread::Signal::RESTART);

}

void FinalChallenge::start() {
}

void FinalChallenge::finish() {
    ChallengeHelper::set_completed_level(1);    
    event_finish.trigger(0);
}
