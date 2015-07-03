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

    //add the player to the game
    int player_id = ChallengeHelper::make_sprite(
                        this,
                        "sprite/1",
                        "Ben",
                        Walkability::BLOCKED,
                        "east/still/1"
                    );

    //define what happens when a character ("sprite") stands on button1

    ChallengeHelper::make_interaction("trigger/objective/button1", [this] (int)
    {
        map->update_tile(9, 8, "Scenery2", "test/blank"); //make the wall dissapear
        map->update_tile(9, 8, "Collisions", "test/blank"); //remove the collision with the wall

        return true;
    });

    //define what happens when a character moves off button1
    ChallengeHelper::lose_interaction("trigger/objective/button1", [player_id,monkey_id,this] (int)
    {
        map->update_tile(9, 8, "Scenery2", "walls/4"); //add the wall
        map->update_tile(9, 8, "Collisions", "test/solid"); //add the collision tile (To make it impossible to walk through the wall)

        ChallengeHelper::kill_sprite(this, player_id, (glm::vec2 (9, 8)), "Game", "Oh no! The Wall got you!");
        ChallengeHelper::kill_sprite(this, monkey_id, (glm::vec2 (9, 8)), "Game", "Oh no! The Wall got you!");

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

        ChallengeHelper::kill_sprite(this, player_id, (glm::vec2 (17, 7)), "Game", "Oh no! The Wall got you!");
        ChallengeHelper::kill_sprite(this, monkey_id, (glm::vec2 (17, 7)), "Game", "Oh no! The Wall got you!");

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

        ChallengeHelper::kill_sprite(this, player_id, (glm::vec2 (19, 9)), "Game", "Oh no! The Wall got you!");
        ChallengeHelper::kill_sprite(this, monkey_id, (glm::vec2 (19, 9)), "Game", "Oh no! The Wall got you!");

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

        ChallengeHelper::kill_sprite(this, player_id, (glm::vec2 (17, 7)), "Game", "Oh no! The Wall got you!");
        ChallengeHelper::kill_sprite(this, monkey_id, (glm::vec2 (17, 7)), "Game", "Oh no! The Wall got you!");

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

        ChallengeHelper::kill_sprite(this, player_id, (glm::vec2 (9, 8)), "Game", "Oh no! The Wall got you!");
        ChallengeHelper::kill_sprite(this, monkey_id, (glm::vec2 (9, 8)), "Game", "Oh no! The Wall got you!");

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
