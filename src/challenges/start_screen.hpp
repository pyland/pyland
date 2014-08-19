#ifndef START_SCREEN_H
#define START_SCREEN_H

#include "challenge.hpp"

class ChallengeData;


class StartScreen : public Challenge {
    public:
        StartScreen(ChallengeData* _challenge_data);

        virtual void start();
        virtual void finish();

};

#endif
