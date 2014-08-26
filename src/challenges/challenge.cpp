#include "python_embed_headers.hpp"

#include <boost/python.hpp>
#include <glog/logging.h>
#include <glm/vec2.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <stdlib.h>
#include <string>

#include "api.hpp"
#include "challenge.hpp"
#include "challenge_data.hpp"
#include "engine.hpp"
#include "entitythread.hpp"
#include "interpreter.hpp"
#include "make_unique.hpp"
#include "map.hpp"
#include "map_object.hpp"
#include "map_viewer.hpp"
#include "notification_bar.hpp"
#include "object_manager.hpp"
#include "sprite.hpp"

namespace py = boost::python;

Challenge::Challenge(ChallengeData* _challenge_data) :
    map(nullptr), sprite_switcher(nullptr), challenge_data(_challenge_data) {
        map = new Map(challenge_data->map_name);
        MapViewer* map_viewer = Engine::get_map_viewer();
        if(map_viewer == nullptr) {
            throw std::logic_error("MapViewer is not intialised in Engine. In Challenge()");
        }
        map_viewer->set_map(map);

        sprite_switcher = new SpriteSwitcher();

        //Register a dispatcher to shut the challenge down
        event_finish.register_callback([&] (int next_challenge) {
            challenge_data->run_challenge = false;
            challenge_data->next_challenge = next_challenge;
            return false;
        });
}

Challenge::~Challenge() {
    // destruct sprite switch
    delete sprite_switcher;

    //Remove all sprites
    for(int sprite_id : sprite_ids) {
        ObjectManager::get_instance().remove_object(sprite_id);
    }

    //Remove all map objects
    for(int map_object_id : map_object_ids) {
        ObjectManager::get_instance().remove_object(map_object_id);
    }

    Engine::get_notification_bar()->clear_text();
    Engine::get_map_viewer()->set_map(nullptr);

    //Delete the map
    delete map;
    //All threads created for the challenge should have terminated now
    LOG(INFO) << " CHALLENGE DESTROYED ";
}

int Challenge::make_map_object(glm::vec2 position,
                               std::string name,
                               Walkability walkability,
                               AnimationFrames frames,
                               std::string start_frame,
                               bool cuttable,
                               bool findable) {

    auto new_object(std::shared_ptr<MapObject>(new MapObject(position, name, walkability, frames, start_frame)));
    new_object->set_cuttable(cuttable);
    new_object->set_findable(findable);
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

