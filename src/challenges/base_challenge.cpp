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
#include "notification_bar.hpp"
#include "object_manager.hpp"
#include "sprite.hpp"

namespace py = boost::python;

BaseChallenge::BaseChallenge(ChallengeData* _challenge_data) : Challenge(_challenge_data) {

}

BaseChallenge::~BaseChallenge() {

}

void BaseChallenge::start() {

}

void BaseChallenge::finish() { 
   //Complete the challenge
   //TODO: Change this to use your challenge's id
   int challenge_id = 1;
   ChallengeHelper::set_completed_level(challenge_id);    

   //Return to the start screen
   event_finish.trigger(0);
}


