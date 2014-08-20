#ifndef FINAL_CHALLENGE_H
#define FINAL_CHALLENGE_H

#include "challenge.hpp"
#include "engine.hpp"

class ChallengeData;


class FinalChallenge : public Challenge {
    public:
        FinalChallenge(ChallengeData* _challenge_data);

        virtual void start();
        virtual void finish();

};

#endif
