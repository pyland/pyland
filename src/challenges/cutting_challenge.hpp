#ifndef CUTTING_CHALLENGE_H
#define CUTTING_CHALLENGE_H

#include "challenge.hpp"
#include "engine.hpp"

class ChallengeData;


class CuttingChallenge : public Challenge {
private:
    int vine_count = 0;

public:
    CuttingChallenge(ChallengeData* _challenge_data);

    virtual void start();
    virtual void finish();
};

#endif
