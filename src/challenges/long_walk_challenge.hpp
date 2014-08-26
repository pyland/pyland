#ifndef LONG_WALK_CHALLENGE_H
#define LONG_WALK_CHALLENGE_H

#include <vector>

#include "challenge.hpp"
#include "dispatcher.hpp"
#include "lifeline.hpp"

class ChallengeData;


class LongWalkChallenge : public Challenge {
    public:
        LongWalkChallenge(ChallengeData* _challenge_data);

        virtual void start();
        virtual void finish();

    private:
        std::vector<int> wall_path_medium_objects;
        std::vector<int> wall_path_long_objects;

        std::vector<PositionDispatcher<int>::CallbackID> blocked_alert_path_long_callbacks;
        std::vector<PositionDispatcher<int>::CallbackID> blocked_alert_path_medium_callbacks;
        std::vector<PositionDispatcher<int>::CallbackID> end_callbacks;
        std::vector<PositionDispatcher<int>::CallbackID> grass_path_short_callbacks;
        std::vector<PositionDispatcher<int>::CallbackID> room_first_exit_callbacks;


        Lifeline editor_lifeline;
};

#endif
