#ifndef BASECHALLENGE_H
#define BASECHALLENGE_H

#include "challenge.hpp"

class ChallengeData;

///
/// This is the base challenge from which you can build further challenges.
/// You need to provide implementations for start and finish 
/// To finish the challenge, run;
/// 
/// event_finish.trigger(0);
///
/// This will return you to the introduction screen.
///
/// NOTE: This file is NOT in the makefile for a reason!
///
class NewChallenge : public Challenge {

public:

    NewChallenge(ChallengeData* _challenge_data);
    ~NewChallenge();
    virtual void start();
    virtual void finish();

};

#endif

