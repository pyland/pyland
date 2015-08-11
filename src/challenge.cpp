#include "python_embed_headers.hpp"

#include <boost/python.hpp>
#include <glog/logging.h>
#include <glm/vec2.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <list>

#include "challenge.hpp"
#include "challenge_data.hpp"
#include "engine.hpp"
#include "entitythread.hpp"
#include "interpreter.hpp"
#include "make_unique.hpp"
#include "map.hpp"
#include "map_object.hpp"
#include "map_viewer.hpp"
#include "text_box.hpp"
#include "object_manager.hpp"
#include "challenge_helper.hpp"
#include "game_engine.hpp"

class GUIMain;

namespace py = boost::python;

Challenge::Challenge(ChallengeData* _challenge_data, GUIMain * _gui_main) :
    game_engine(nullptr), map(nullptr), challenge_data(_challenge_data) {
        //Load the correct map into the game.
        map = new Map(challenge_data->map_name);
        MapViewer* map_viewer = Engine::get_map_viewer();
        if(map_viewer == nullptr) {
            throw std::logic_error("MapViewer is not intialised in Engine. In Challenge()");
        }
        map_viewer->set_map(map);

        //Add all the objects to the map
        //TODO: The names of variables need to be refactored to make sense, and this code needs to be cleaned up as there is a bit too much inderection atm!
        for(auto properties : map->locations) { //look at map_loader.hpp for the format of this struct (MapObjectProperties)
            //Create the entity, it automatically gets added to the entity_list
            create_entity(properties.first, properties.second.object_file_location, properties.second.sprite_file_location, properties.second.position);
        }

        //create a new GameEngine instance for the python api, we need to review if this is the best place to create it
        game_engine = new GameEngine(_gui_main, this);

        //The intepreter creates a new python thread which is the main thread for the running level, the list of all objects in the level are passed to it which are then exposed to the python code
        daemon = std::make_unique<LockableEntityThread>(challenge_data->interpreter->register_entities(entity_list, *game_engine));
        daemon->value->halt_soft(EntityThread::Signal::RESTART);
}

Challenge::~Challenge() {
    //Remove all map objects
    for(int map_object_id : map_object_ids) {
        ObjectManager::get_instance().remove_object(map_object_id);
    }

    Engine::get_map_viewer()->set_map(nullptr);

    //Delete the map
    delete map;

    //Delete the game engine wrapper
    delete game_engine;
    //All threads created for the challenge should have terminated now
    LOG(INFO) << " CHALLENGE DESTROYED ";
}

int Challenge::make_object(glm::vec2 position,
                           std::string name,
                           Walkability walkability,
                           AnimationFrames frames) {

    auto new_object(std::shared_ptr<MapObject>(new MapObject(position, name, walkability, frames)));
    ObjectManager::get_instance().add_object(new_object);

    auto new_object_id(new_object->get_id());

    LOG(INFO) << "created new_object with id: " << new_object_id;
    map_object_ids.push_back(new_object_id);
    map->add_map_object(new_object_id);

    return new_object_id;
}

py::object Challenge::read_message(int) const {
    return py::object();
}

Entity *Challenge::create_entity(std::string object_name, std::string object_file_location, std::string sprite_file_location, glm::ivec2 position) {
    int map_object_id = ChallengeHelper::make_object(
        this,
        object_name, //the tmx name of the object being reconstructed from it's parts. TODO: Handle this more neatly  (in game_engine.cpp as well)
        Walkability::WALKABLE,	//wether the object can be walked through
        object_file_location,
        sprite_file_location,
        position
    );
    //Push the map_object on the Challenge's internal list of map_objects
    map_object_ids.push_back(map_object_id);
    //Create the entity which will be passed to python for each map_object
    auto *entity(new Entity(position, object_name, object_file_location, map_object_id));

    entity_list.push_front(*entity); //put all the entities in the entity list!!!
    return entity;
}




