#include <glm/vec2.hpp>
#include <memory>
#include <sstream>
#include <vector>
#include <fstream>
#include <iostream>

#include "api.hpp"
#include "challenge.hpp"
#include "challenge_data.hpp"
#include "challenge_helper.hpp"
#include "engine.hpp"
#include "fml.hpp"
#include "make_unique.hpp"
#include "map.hpp"
#include "map_loader.hpp"
#include "map_object.hpp"
#include "map_viewer.hpp"
#include "interpreter.hpp"
#include "object_manager.hpp"
#include "sprite.hpp"

glm::vec2 ChallengeHelper::get_location_interaction(std::string name) {
    LOG(INFO) << "getting location of " << name;
    auto properties(Engine::get_map_viewer()->get_map()->locations.at("Interactions/" + name));
    return properties.location;
}

glm::vec2 ChallengeHelper::get_location_object(std::string name) {
    LOG(INFO) << "getting location of " << name;
    auto properties(Engine::get_map_viewer()->get_map()->locations.at("Objects/" + name));
    return properties.location;
}

int ChallengeHelper::make_object(Challenge *challenge,
                                 std::string marker_name,
                                 Walkability walkability,
                                 std::string start_frame,
                                 bool cuttable,
                                 bool findable) {

    return make_object(challenge, marker_name, walkability, marker_name, start_frame, cuttable, findable);
}
int ChallengeHelper::make_object(Challenge *challenge,
                                 std::string marker_name,
                                 Walkability walkability,
                                 std::string name,
                                 std::string start_frame,
                                 bool cuttable,
                                 bool findable) {

    auto *map = Engine::get_map_viewer()->get_map();
    LOG(INFO) << "checking map for object called " << marker_name;
    auto properties(map->locations.at("Objects/" + marker_name));
    LOG(INFO) << "creating object at " << marker_name << " (" << properties.tileset << ")";

    return challenge->make_map_object(
        properties.location,
        name,
        walkability,
        AnimationFrames(properties.tileset.substr(0, properties.tileset.length() - start_frame.length() - 1)),
        start_frame,
        cuttable,
        findable
    );
}

int ChallengeHelper::make_sprite(Challenge *challenge,
                                 std::string marker_name,
                                 std::string sprite_name,
                                 Walkability walkability,
                                 std::string start_frame) {

    auto *map = Engine::get_map_viewer()->get_map();
    LOG(INFO) << marker_name;
    auto properties(map->locations.at("Objects/" + marker_name));

    LOG(INFO) << properties.tileset;

    auto new_sprite(std::make_shared<Sprite>(
        properties.location,
        sprite_name,
        walkability,
        AnimationFrames(properties.tileset.substr(0, properties.tileset.length() - start_frame.length() - 1)),
        start_frame
    ));
    ObjectManager::get_instance().add_object(new_sprite);

    auto sprite_id(new_sprite->get_id());

    LOG(INFO) << "Adding sprite";

    challenge->sprite_ids.push_back(sprite_id);
    map->add_sprite(sprite_id);
    Engine::get_map_viewer()->set_map_focus_object(sprite_id);
    LOG(INFO) << "Creating sprite wrapper";
    LOG(INFO) << "ID " << sprite_id;

    // Register user controled sprite
    // Yes, this is a memory leak. Deal with it.
    auto *a_thing(new Entity(properties.location, sprite_name, sprite_id));

    LOG(INFO) << "Registering sprite";
    new_sprite->daemon = std::make_unique<LockableEntityThread>(challenge->challenge_data->interpreter->register_entity(*a_thing));
    LOG(INFO) << "Done!";

    return sprite_id;
}

void ChallengeHelper::kill_sprite(Challenge *challenge,
                                  int sprite_id,
                                  glm::vec2 location) {
    auto *map = Engine::get_map_viewer()->get_map();
    auto player = ObjectManager::get_instance().get_object<Sprite>(sprite_id);
    
    if((player->get_position()) == (location)) {
        map->remove_sprite(sprite_id); //at the moment only the sprite is removed
    }

    return;
}

PositionDispatcher<int>::CallbackID
ChallengeHelper::make_interaction(std::string name,
                                  std::function<bool (int)> callback) {

    LOG(INFO) << "creating interaction at " << name;
    auto *map = Engine::get_map_viewer()->get_map();
    auto properties(map->locations.at("Interactions/" + name));

    return map->event_step_on.register_callback(properties.location, callback);
}

PositionDispatcher<int>::CallbackID
ChallengeHelper::lose_interaction(std::string name,
                                  std::function<bool (int)> callback) {

    LOG(INFO) << "creating interaction at " << name;
    auto *map = Engine::get_map_viewer()->get_map();
    auto properties(map->locations.at("Interactions/" + name));

    return map->event_step_off.register_callback(properties.location, callback);
}

void ChallengeHelper::create_pickupable(glm::ivec2 start_tile,
                                        glm::ivec2 pickup_tile,
                                        glm::ivec2 finish_tile,
                                        glm::ivec2 dropoff_tile,
                                        int object_id, bool repeat) {

    auto *map(Engine::get_map_viewer()->get_map());
    auto object(ObjectManager::get_instance().get_object<MapObject>(object_id));
    //object->set_position(start_tile);

    // Pick-up marker
   // Engine::change_tile(pickup_tile, 4, "circle_yellow");

    map->event_step_on.register_callback(
        pickup_tile,
        [object_id, pickup_tile, dropoff_tile, start_tile,repeat] (int) {
            if (Engine::is_object_at(start_tile, object_id)) {
                int id(Engine::get_sprites_at(pickup_tile).front());
                auto sprite(ObjectManager::get_instance().get_object<Sprite>(id));

                sprite->add_to_inventory(object_id);
                //Engine::change_tile(pickup_tile,  5, "blank");
                //Engine::change_tile(dropoff_tile, 5, "circle_yellow");

                // We only give out our item once
                return repeat;
            } else {
                return true;
            }
        }
    );

    // Put-down marker
    //    Engine::change_tile(dropoff_tile, 4, "circle_yellow");

    map->event_step_on.register_callback(
        dropoff_tile,
        [object_id, dropoff_tile, finish_tile,repeat] (int) {
            int id(Engine::get_sprites_at(dropoff_tile).front());
            auto sprite(ObjectManager::get_instance().get_object<Sprite>(id));

            if (sprite->remove_from_inventory(object_id) ) {
                auto object(ObjectManager::get_instance().get_object<MapObject>(object_id));

                object->set_position(finish_tile);
                //                Engine::change_tile(dropoff_tile, 4, "blank");

                // We're done waiting, so remove callback
                return repeat;
            }

            // Carry on listening because not yet returned
            return true;
        }
    );
}

void ChallengeHelper::create_pickupable(glm::ivec2 object_tile,
                                        glm::ivec2 pickup_tile,
                                        int object_id) {

    auto *map = Engine::get_map_viewer()->get_map();
    auto object(ObjectManager::get_instance().get_object<MapObject>(object_id));
    //object->set_position(object_tile);

    // Pick-up marker
    //    Engine::change_tile(pickup_tile, 4, "circle_yellow");
    map->event_step_on.register_callback(
        pickup_tile,
        [object_id, pickup_tile, object_tile] (int) {
            int id(Engine::get_sprites_at(pickup_tile).front());
            auto sprite(ObjectManager::get_instance().get_object<Sprite>(id));

            if (Engine::is_object_at(object_tile, object_id)) {
                sprite->add_to_inventory(object_id);
            }
            else if (sprite->remove_from_inventory(object_id)) {
                auto object(ObjectManager::get_instance().get_object<MapObject>(object_id));

                object->set_position(object_tile);
            }

            // We're able to give and accept the item
            // an arbitrary number of times
            return true;
        }
    );
}

void ChallengeHelper::set_completed_level(int challenge_id) {
  std::ofstream myfile;
  myfile.open ("game_progress.txt");
  myfile << (challenge_id+1);
  myfile.close();
}

// return the level the player such play next
int ChallengeHelper::get_current_level () {
  std::string line;
  std::ifstream myfile ("game_progress.txt");
  if (myfile.is_open()) {
    getline (myfile,line);
    myfile.close();
    return std::stoi(line);
  } else {
    LOG(ERROR) << "Unable to open game progress file";
    return 1;
  }
}
