#include <glm/vec2.hpp>

#include "challenge_helper.hpp"
#include "engine.hpp"
#include "map.hpp"
#include "map_viewer.hpp"
#include "object_manager.hpp"
#include "sprite.hpp"


std::shared_ptr<MapObject>
ChallengeHelper::make_object(std::string name, Walkability walkability) {

    auto *map = Engine::get_map_viewer()->get_map();
    auto id(map->locations.get<int>("Objects/" + name));
    auto properties(map->obj_from_id(id));

    auto object(std::make_shared<MapObject>(
        properties.location,
        name,
        walkability,
        properties.tileset_id,
        properties.atlas_name
    ));

    ObjectManager::get_instance().add_object(object);
    map->add_map_object(object->get_id());

    return object;
}

PositionDispatcher<int>::CallbackID
ChallengeHelper::make_interaction(std::string name,
                                  std::function<bool (int)> callback) {

    auto *map = Engine::get_map_viewer()->get_map();
    auto id(map->locations.get<int>("Interactions/" + name));

    return map->event_step_on.register_callback(
        map->obj_from_id(id).location,
        callback
    );
}

void ChallengeHelper::create_pickupable(glm::ivec2 start_tile,
                                        glm::ivec2 pickup_tile,
                                        glm::ivec2 finish_tile,
                                        glm::ivec2 dropoff_tile,
                                        std::shared_ptr<MapObject> object) {

    auto *map = Engine::get_map_viewer()->get_map();

    object->set_position(start_tile);

    // Pick-up marker
    Engine::change_tile(pickup_tile, 4, "circle_yellow");

    map->event_step_on.register_callback(
        pickup_tile,
        [object, pickup_tile, dropoff_tile] (int) {
            int id(Engine::get_sprites_at(pickup_tile).front());
            auto sprite(ObjectManager::get_instance().get_object<Sprite>(id));

            sprite->add_to_inventory(object->get_id());
            Engine::change_tile(pickup_tile,  4, "blank");
            Engine::change_tile(dropoff_tile, 4, "circle_yellow");

            // We only give out our item once
            return false;
        }
    );

    // Put-down marker
    Engine::change_tile(dropoff_tile, 4, "circle_yellow");

    map->event_step_on.register_callback(
        dropoff_tile,
        [object, dropoff_tile, finish_tile] (int) {
            int id(Engine::get_sprites_at(dropoff_tile).front());
            auto sprite(ObjectManager::get_instance().get_object<Sprite>(id));

            if (sprite->remove_from_inventory(object->get_id()) ) {
                object->set_position(finish_tile);
                Engine::change_tile(dropoff_tile, 4, "blank");

                // We're done waiting, so remove callback
                return false;
            }

            // Carry on listening because not yet returned
            return true;
        }
    );
}

void ChallengeHelper::create_pickupable(glm::ivec2 object_tile,
                                        glm::ivec2 pickup_tile,
                                        std::shared_ptr<MapObject> object) {

    auto *map = Engine::get_map_viewer()->get_map();

    object->set_position(object_tile);

    // Pick-up marker
    Engine::change_tile(pickup_tile, 4, "circle_yellow");

    map->event_step_on.register_callback(
        pickup_tile,
        [object, pickup_tile, object_tile] (int) {
            int id(Engine::get_sprites_at(pickup_tile).front());
            auto sprite(ObjectManager::get_instance().get_object<Sprite>(id));

            if (Engine::is_object_at(object_tile, object->get_id())) {
                sprite->add_to_inventory(object->get_id());
            }
            else if (sprite->remove_from_inventory(object->get_id())) {
                object->set_position(object_tile);
            }

            // We're able to give and accept the item
            // an arbitrary number of times
            return true;
        }
    );
}
