#ifndef GAME_MAIN_H
#define GAME_MAIN_H

class Challenge;
class ChallengeData;

class GameMain{
private:


public:
    GameMain(int argc, char *argv[]);
    void game_loop();
    Challenge* pick_challenge(ChallengeData* challenge_data);

};

#endif // GAME_MAIN_H
