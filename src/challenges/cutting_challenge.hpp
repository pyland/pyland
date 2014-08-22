#ifndef CUTTING_CHALLENGE_H
#define CUTTING_CHALLENGE_H

#include <random>
#include <vector>

#include <glm/vec2.hpp>

#include "challenge.hpp"
#include "engine.hpp"

class ChallengeData;


class CuttingChallenge : public Challenge {
private:
    int vine_count;
    std::vector<glm::ivec2> vine_spots;
    std::uniform_int_distribution<uint32_t> uniform_spot_gen;
    std::uniform_int_distribution<uint32_t> uniform_direction_gen;
    std::function<void()> regrow;

    void grow_out(int x, int y);

public:
    CuttingChallenge(ChallengeData* _challenge_data);

    virtual void start();
    virtual void finish();
};

#endif
