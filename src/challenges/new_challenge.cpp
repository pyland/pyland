#include "api.hpp"
#include "base_challenge.hpp"
#include "challenge.hpp"
#include "challenge_data.hpp"
#include "challenge_helper.hpp"
#include "engine.hpp"
#include "entitythread.hpp"
#include "interpreter.hpp"
#include "make_unique.hpp"
#include "map.hpp"
#include "map_object.hpp"
#include "map_viewer.hpp"
#include "new_challenge.hpp"
#include "notification_bar.hpp"
#include "object_manager.hpp"
#include "sprite.hpp"

namespace py = boost::python;

NewChallenge::NewChallenge(ChallengeData* _challenge_data) : Challenge(_challenge_data)
{

    //add the monkey to the game
    int monkey_id = ChallengeHelper::make_assistant(
                        this,
                        "sprite/monkey",
                        "Alex",
                        Walkability::BLOCKED,
                        "east/still/1"
                    );

    int player_id = ChallengeHelper::make_sprite(
                        this,
                        "sprite/1",
                        "Ben",
                        Walkability::BLOCKED,
                        "east/still/1"
                    );

    ChallengeHelper::make_interaction("trigger/objective/button1", [monkey_id,this] (int)
    {
        map->update_tile(9, 8, "Scenery2", "test/blank");
        map->update_tile(9, 8, "Collisions", "test/blank");
        //End Level if one gets trapped in door
        //Must remove object from id lists if remove

//        Engine::print_dialogue("Villager","test");
//        auto map_objects = map->get_map_objects();
//        for(auto object_id: map_objects){
//            map->remove_map_object(object_id);
//            ObjectManager::get_instance().remove_object(object_id);
//        }
//
//        auto map_sprites = map->get_sprites();
//        for(auto sprite_id: map_sprites){
//            if (sprite_id == monkey_id){
//                map->remove_sprite(sprite_id);
//                ObjectManager::get_instance().remove_object(sprite_id);
//            }
//        }

        return true;
    });

    ChallengeHelper::lose_interaction("trigger/objective/button1", [player_id,monkey_id,this] (int)
    {
        map->update_tile(9, 8, "Scenery2", "walls/4");
        map->update_tile(9, 8, "Collisions", "test/solid");
        map->remove_map_object(monkey_id);

        auto player = ObjectManager::get_instance().get_object<Sprite>(player_id);
        auto monkey = ObjectManager::get_instance().get_object<Sprite>(monkey_id);

        if((player->get_position()) == (glm::vec2 (9, 8)))
        {
            //End level if one gets trapped in door
        }


        if((monkey->get_position()) == (glm::vec2 (9, 8)))
        {
            //End level if one gets trapped in door
        }
        return true;
    });

    ChallengeHelper::make_interaction("trigger/objective/button2", [this] (int)
    {
        map->update_tile(17, 7, "Scenery2", "test/blank");
        map->update_tile(17, 7, "Collisions", "test/blank");
        return true;
    });

    ChallengeHelper::lose_interaction("trigger/objective/button2", [player_id,monkey_id,this] (int)
    {
        map->update_tile(17, 7, "Scenery2", "walls/4");
        map->update_tile(17, 7, "Collisions", "test/solid");

        auto player = ObjectManager::get_instance().get_object<Sprite>(player_id);
        auto monkey = ObjectManager::get_instance().get_object<Sprite>(monkey_id);

        if((player->get_position()) == (glm::vec2 (17, 7)))
        {
            map->remove_sprite((player_id));
        }


        if((monkey->get_position()) == (glm::vec2 (17, 7)))
        {
            map->remove_sprite((monkey_id));
        }
        return true;
    });


    ChallengeHelper::make_interaction("trigger/objective/button3", [this] (int)
    {
        map->update_tile(19, 9, "Scenery2", "test/blank");
        map->update_tile(19, 9, "Collisions", "test/blank");
        return true;
    });

    ChallengeHelper::lose_interaction("trigger/objective/button3", [player_id,monkey_id,this] (int)
    {
        map->update_tile(19, 9, "Scenery2", "walls/3");
        map->update_tile(19, 9, "Collisions", "test/solid");

        auto player = ObjectManager::get_instance().get_object<Sprite>(player_id);
        auto monkey = ObjectManager::get_instance().get_object<Sprite>(monkey_id);

        if((player->get_position()) == (glm::vec2 (19, 9)))
        {
            map->remove_sprite((player_id));
        }


        if((monkey->get_position()) == (glm::vec2 (19, 9)))
        {
            map->remove_sprite((monkey_id));
        }

        return true;
    });

    ChallengeHelper::make_interaction("trigger/objective/button4", [this] (int)
    {
        map->update_tile(17, 7, "Scenery2", "test/blank");
        map->update_tile(17, 7, "Collisions", "test/blank");
        return true;
    });

    ChallengeHelper::lose_interaction("trigger/objective/button4", [player_id,monkey_id,this] (int)
    {
        map->update_tile(17, 7, "Scenery2", "walls/4");
        map->update_tile(17, 7, "Collisions", "test/solid");

        auto player = ObjectManager::get_instance().get_object<Sprite>(player_id);
        auto monkey = ObjectManager::get_instance().get_object<Sprite>(monkey_id);

        if((player->get_position()) == (glm::vec2 (17, 7)))
        {
            map->remove_sprite((player_id));
        }


        if((monkey->get_position()) == (glm::vec2 (17, 7)))
        {
            map->remove_sprite((monkey_id));
        }

        return true;
    });

    ChallengeHelper::make_interaction("trigger/objective/button5", [this] (int)
    {
        map->update_tile(9, 8, "Scenery2", "test/blank");
        map->update_tile(9, 8, "Collisions", "test/blank");
        return true;
    });

    ChallengeHelper::lose_interaction("trigger/objective/button5", [player_id,monkey_id,this] (int)
    {
        map->update_tile(9, 8, "Scenery2", "walls/4");
        map->update_tile(9, 8, "Collisions", "test/solid");

        auto player = ObjectManager::get_instance().get_object<Sprite>(player_id);
        auto monkey = ObjectManager::get_instance().get_object<Sprite>(monkey_id);

        if((player->get_position()) == (glm::vec2 (9, 8)))
        {
            map->remove_sprite((player_id));
        }


        if((monkey->get_position()) == (glm::vec2 (9, 8)))
        {
            map->remove_sprite((monkey_id));
        }

        return true;
    });


    ChallengeHelper::make_interaction("trigger/objective/finish", [this] (int)
    {
        ChallengeHelper::set_completed_level(4);
        finish();
        return false;
    });

}


NewChallenge::~NewChallenge()
{

}

void NewChallenge::start()
{
}

void NewChallenge::finish()
{
    int challenge_id = 4;
    ChallengeHelper::set_completed_level(challenge_id);

    //Return to the start screen
    event_finish.trigger(0);
}
