#include <glm/vec2.hpp>

#include "challenge_helper.hpp"
#include "engine.hpp"
#include "map_viewer.hpp"
#include "object_manager.hpp"
#include "sprite.hpp"

void ChallengeHelper::create_pickupable(glm::ivec2 start_tile,
                                        glm::ivec2 pickup_tile,
                                        glm::ivec2 finish_tile,
                                        glm::ivec2 dropoff_tile,
                                        std::shared_ptr<MapObject> object) {

    auto *map = Engine::get_map_viewer()->get_map();

    object->set_position(start_tile);
    Engine::change_tile(pickup_tile,5,18);
    map->event_step_on.register_callback(
        pickup_tile,
        [object,pickup_tile] (int) {
            int id = Engine::get_sprites_at(pickup_tile).front();
            ObjectManager::get_instance().get_object<Sprite>(id)->add_to_inventory(object);
            return false;
        }
    );

    Engine::change_tile(dropoff_tile,5,18);
    map->event_step_on.register_callback(
        dropoff_tile,
        [object,dropoff_tile,finish_tile] (int) {
            int id = Engine::get_sprites_at(dropoff_tile).front();
            bool was_returned = ObjectManager::get_instance().get_object<Sprite>(id)->remove_from_inventory(object);
            if (was_returned) {
                object->set_position(finish_tile);
            }
            return was_returned;
        }
    );

}