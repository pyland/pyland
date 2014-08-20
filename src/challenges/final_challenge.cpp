#include "final_challenge.hpp"
#include "challenge_helper.hpp"


FinalChallenge::FinalChallenge(ChallengeData *challenge_data): Challenge(challenge_data) {
    Engine::print_dialogue( "Game", "Welcome to the final challenge");
    ChallengeHelper::make_sprite(this, "sprite/1","Ben", Walkability::BLOCKED);
    ChallengeHelper::make_sprite(this, "sprite/crocadile","", Walkability::BLOCKED);
}

void FinalChallenge::start() {
}

void FinalChallenge::finish() {
    // TODO: Somehow finish challenge...
}