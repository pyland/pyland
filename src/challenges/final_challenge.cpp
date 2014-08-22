#include <glog/logging.h>
#include <memory>
#include <thread>
#include <chrono>
#include <iostream>
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

    // ,"banana","pineapple"
    std::vector<std::string> fruit_types = {"orange","pineapple"};
    glm::ivec2 hangover_location = ChallengeHelper::get_location_interaction("handover/1");
    glm::ivec2 hangover_pickup = ChallengeHelper::get_location_interaction("pickup/1");
    glm::ivec2 hangover_dropoff = ChallengeHelper::get_location_interaction("dropoff/1");

    for (std::string fruit_type : fruit_types) {

        // set of fruit collection part
        glm::ivec2 crate_location = ChallengeHelper::get_location_interaction("crate/"+fruit_type);
        glm::ivec2 dropoff_location = ChallengeHelper::get_location_interaction("dropoff/"+fruit_type);

        int num_of_fruit = 7;
        std::vector<int> fruit_ids;

        // adding fruits as pickupable objects
        for (int i = 1; i <= num_of_fruit; i++) {
            auto name = fruit_type+"/"+std::to_string(i);
            glm::ivec2 fruit_location = ChallengeHelper::get_location_object(name);

            int fruit_id = ChallengeHelper::make_object(this, name, Walkability::WALKABLE, fruit_type);
            ChallengeHelper::create_pickupable(fruit_location, fruit_location, crate_location, dropoff_location , fruit_id, true);
            ChallengeHelper::create_pickupable(hangover_location,hangover_pickup,hangover_location,hangover_dropoff,fruit_id, true);
            fruit_ids.push_back(fruit_id);
        }

        // checking if all fruits are in crate
        ChallengeHelper::make_interaction("dropoff/"+fruit_type, [dropoff_location, crate_location, fruit_ids,this] (int) {

            if (Engine::is_objects_at(crate_location, fruit_ids)) {
                Engine::print_dialogue ("Game","Well Done, all the oranges are in the crate");
                finish();
            } else {
                Engine::print_dialogue ("Game","Keep going");
            }
            return true;
        });

    }

    // challenge exit
    ChallengeHelper::make_interaction("exit", [this] (int) {
        finish();
        LOG(INFO) << "exiting";
        return false;
    });

    // creating croc
    for (int i = 1; i<=3; i++) {
        std::string croc_num = std::to_string(i);


        LOG(INFO) << "creating croc";
        int croc_id = ChallengeHelper::make_object(
            this,
            "sprite/crocodile/"+croc_num,
            Walkability::BLOCKED,
            "north/still/1"
        );

        auto croc = ObjectManager::get_instance().get_object<Object>(croc_id);
        if (!croc) { return; }

        // Register user controled sprite
        // Yes, this is a memory leak. Deal with it.
        auto properties(map->locations.at("Objects/sprite/crocodile/"+croc_num));
        auto *a_thing(new Entity(properties.location, "final_challenge_croc_"+croc_num, croc_id));

        LOG(INFO) << "Registering sprite";
        croc->daemon = std::make_unique<LockableEntityThread>(challenge_data->interpreter->register_entity(*a_thing));
        LOG(INFO) << "Done!";

        croc->daemon->value->halt_soft(EntityThread::Signal::RESTART);
    }
}

void FinalChallenge::start() {
}

void FinalChallenge::finish() {
    ChallengeHelper::set_completed_level(1);    
    event_finish.trigger(0);
}
