#ifndef INTRODUCTION_CHALLENGE_H
#define INTRODUCTION_CHALLENGE_H

#include "python_embed_headers.hpp"

#include <boost/python.hpp>
#include <glm/vec2.hpp>
#include <unordered_map>
#include <vector>

#include "challenge.hpp"
#include "lifeline.hpp"

class ChallengeData;

#ifndef KEYHASH
#define KEYHASH
struct KeyHash {
    std::size_t operator()(const glm::vec2 &key) const {
        return boost::hash<float>()(key.x * 2048.0f + key.y);
    }
};
#endif

class IntroductionChallenge : public Challenge {
private:
    int player;
    int treasure;
    int pathblocker_top;
    std::vector<int> pathblockers_bottom;

    int keys_found;

    Lifeline editor_lifeline;

    std::unordered_map<glm::vec2, glm::vec2, KeyHash> map_location_displacements;

public:
    IntroductionChallenge(ChallengeData *challenge_data);

    virtual void start();
    virtual void finish();

    virtual boost::python::api::object read_message(int id) const;
};

#endif
