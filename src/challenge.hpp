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
#include "walkability.hpp"
#include "entity.hpp"

class ChallengeData;
class Entity;
class Map;
class LockableEntityThread;
class GameEngine;
class GUIMain;

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

    ///
    /// The instance of a GameEngine object, this is actually used as the interface betwixt C++ and python code.
    /// Used for functionality the entities cannot provide
    ///
    GameEngine *game_engine;
protected:
    Map* map;
    Lifeline esc_callback;

    std::vector<int> map_object_ids;
public:

    Challenge(ChallengeData* _challenge_data, GUIMain * _gui_main);

    virtual ~Challenge();

    ChallengeData* challenge_data;

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

    virtual boost::python::object read_message(int id) const;

    ///
    /// Create an entity and return a pointer to it. (Also correctly sets it up in the challenge, map etc.) Used by Challenge and GameEngine
    ///
    Entity *create_entity(std::string name, std::string object_file_location, std::string sprite_file_location, glm::ivec2 position);
};

#endif
