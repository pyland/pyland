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

}

NewChallenge::~NewChallenge() {

}

void NewChallenge::start() {
Engine::print_dialogue ( "Tom",
		"Welcome to my new level\n"
	);
}

void NewChallenge::finish() { 
   //Complete the challenge
   //TODO: Change this to use your challenge's id
   int challenge_id = 4;
   ChallengeHelper::set_completed_level(challenge_id);    

   //Return to the start screen
   event_finish.trigger(0);
}
