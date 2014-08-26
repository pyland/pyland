#ifndef START_SCREEN_H
#define START_SCREEN_H

#include "challenge.hpp"

class ChallengeData;

/// ScreenScreen is the challenge class used on startup to select a challange to play
class StartScreen : public Challenge {
    public:
        StartScreen(ChallengeData* _challenge_data);

        virtual void start();
        virtual void finish();

};

#endif
