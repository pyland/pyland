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
    int monkey_id = ChallengeHelper::make_sprite(
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

    auto player = ObjectManager::get_instance().get_object<Sprite>(player_id);
    auto monkey = ObjectManager::get_instance().get_object<Sprite>(monkey_id);

    ChallengeHelper::make_interaction("trigger/objective/button1", [this] (int)
    {
        map->update_tile(9, 8, "Scenery2", "test/blank");
        map->update_tile(9, 8, "Collisions", "test/blank");
        return true;
    });

    ChallengeHelper::lose_interaction("trigger/objective/button1", [this] (int)
    {
        map->update_tile(9, 8, "Scenery2", "walls/4");
        map->update_tile(9, 8, "Collisions", "test/solid");


        if(player->get_position() == (new glm::vec2(9, 8)))
        {
            map->remove_sprite((player_id));
        }


        if(monkey->get_position() == (new glm::vec2(9, 8)))
        {
            map->remove_sprite((monkey_id));
        }
        return true;
    });

    ChallengeHelper::make_interaction("trigger/objective/button2", [this] (int)
    {
        map->update_tile(17, 7, "Scenery2", "test/blank");
        map->update_tile(17, 7, "Collisions", "test/blank");
        return true;
    });

    ChallengeHelper::lose_interaction("trigger/objective/button2", [this] (int)
    {
        map->update_tile(17, 7, "Scenery2", "walls/4");
        map->update_tile(17, 7, "Collisions", "test/solid");
        return true;
    });


    ChallengeHelper::make_interaction("trigger/objective/button3", [this] (int)
    {
        map->update_tile(19, 9, "Scenery2", "test/blank");
        map->update_tile(19, 9, "Collisions", "test/blank");
        return true;
    });

    ChallengeHelper::lose_interaction("trigger/objective/button3", [this] (int)
    {
        map->update_tile(19, 9, "Scenery2", "walls/3");
        map->update_tile(19, 9, "Collisions", "test/solid");
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
