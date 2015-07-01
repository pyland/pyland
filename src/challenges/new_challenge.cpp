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

NewChallenge::NewChallenge(ChallengeData* _challenge_data) : Challenge(_challenge_data) {

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

    int door3_id = ChallengeHelper::make_object(
		this,
		"trigger/objective/door3",
		Walkability::WALKABLE,
		"3"
	);

	//ChallengeHelper::create_pickupable(f)
	//auto magicdoor3 = (ObjectManager::get_instance()).getobject<MapObject>(door3_id);

	//glm::ivec2 door3_location = ChallengeHelper::get_location_interaction("trigger/objective/door3");

	//ChallengeHelper::create_pickupable()

	ChallengeHelper::make_interaction("trigger/objective/button3", [this] (int) {
		Engine::print_dialogue ("Ben","Yay!");
		return true;
	});







	ChallengeHelper::make_interaction("trigger/objective/finish", [this] (int) {
		ChallengeHelper::set_completed_level(4);
		finish();
		return false;
	});


	//trigger/objective/button1
	//trigger/objective/button4
	//trigger/objective/door1
	//The following code is for door 1 and the buttons that activate it.
	int door1_id = ChallengeHelper::make_object(this, "trigger/objective/door1", Walkability::BLOCKED, "4");

	ChallengeHelper::make_interaction("trigger/objective/button1", [this] (int) {

		return true;
	});

	ChallengeHelper::make_interaction("trigger/objective/button4", [this] (int) {

		return true;
	});

}


NewChallenge::~NewChallenge() {

}

void NewChallenge::start() {
	Engine::print_dialogue ("Ben","Look at those buttons, there may be a way out");
}

void NewChallenge::finish() {
   int challenge_id = 4;
   ChallengeHelper::set_completed_level(challenge_id);

   //Return to the start screen
   event_finish.trigger(0);
}
