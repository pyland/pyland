#ifndef CHALLENGE_H
#define CHALLENGE_H

#include "python_embed_headers.hpp"

#include <boost/python.hpp>
#include <glm/vec2.hpp>
#include <string>
#include <vector>

#include "animation_frames.hpp"
#include "dispatcher.hpp"
#include "lifeline.hpp"
#include "sprite_switcher.hpp"
#include "walkability.hpp"
#include "entity.hpp"

//Constants to define the different types, used in id_type function
const int sprite_id_type = 1;
const int assistant_id_type = 2;
const int object_id_type = 3;

class ChallengeData;
class Map;
class LockableEntityThread;

class Challenge {
private:
    ///
    /// This is a list of entities in the challenge.
    /// An entity is basically the representation of a game object that is passable
    /// to python after wrapping.
    ///
    std::list<Entity> entity_list;

    ///
    /// The Python thread for running scripts in.
    ///
    std::unique_ptr<LockableEntityThread> daemon;
protected:
    Map* map;
    Lifeline esc_callback;
    SpriteSwitcher* sprite_switcher;

    std::vector<int> map_object_ids;
public:

    Challenge(ChallengeData* _challenge_data);

    virtual ~Challenge();

    ChallengeData* challenge_data;
    Dispatcher<int> event_finish;
    std::vector<int> sprite_ids;
    std::vector<int> assistant_ids;

    virtual void start();
    virtual void finish();

    ///
    /// Runs the challenge by calling all the needed subsystems
    ///
    void run();

    ///
    /// Makes an object and adds it to the object manager and the current map
    /// @return the sprite's id
    ///
    int make_object(glm::vec2 position,
                    std::string name,
                    Walkability walkability,
                    AnimationFrames frames);

    int id_type(int id);
    virtual boost::python::object read_message(int id) const;
};

#endif
