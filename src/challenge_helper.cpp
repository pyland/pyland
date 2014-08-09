#include "challenge_helper.hpp"
#include "engine_api.hpp"
#include "map_viewer.hpp"
#include "object_manager.hpp"
#include "sprite.hpp"

void ChallengeHelper::create_pickupable(Vec2D start_tile, Vec2D pickup_tile, Vec2D finish_tile, 
    Vec2D dropoff_tile, std::shared_ptr<MapObject> object) {
    auto *map = Engine::get_map_viewer()->get_map();

    // placing object & pickup marker
    object->set_x_position(start_tile.x);
    object->set_y_position(start_tile.y);
    Engine::change_tile(pickup_tile,5,18);
    map->event_step_on.register_callback(
            pickup_tile,
            [object,pickup_tile,dropoff_tile] (int) {
                //adding object to sprite
                int id = Engine::get_sprites_at(pickup_tile).front();
                ObjectManager::get_instance().get_object<Sprite>(id)->add_to_inventory(object);
                //moving marker
                Engine::change_tile(pickup_tile,5,119);
                Engine::change_tile(dropoff_tile,5,18);
                return false;
    });
    
    map->event_step_on.register_callback(
            dropoff_tile,
            [object,dropoff_tile,finish_tile] (int) {
                int id = Engine::get_sprites_at(dropoff_tile).front();
                bool was_returned = ObjectManager::get_instance().get_object<Sprite>(id)->remove_from_inventory(object);
                if (was_returned) {
                    //reset object location
                    object->set_x_position(finish_tile.x);
                    object->set_y_position(finish_tile.y);
                    Engine::change_tile(dropoff_tile,5,119);
                    return false;
                } else {
                    return true;
                }
                
    });

}

void ChallengeHelper::create_pickupable(Vec2D object_tile, Vec2D pickup_tile, std::shared_ptr<MapObject> object ) {
    auto *map = Engine::get_map_viewer()->get_map();

    object->set_x_position(object_tile.x);
    object->set_y_position(object_tile.y);
    Engine::change_tile(pickup_tile,5,18);

    map->event_step_on.register_callback(
            pickup_tile,
            [object,pickup_tile,object_tile] (int) {
                int id = Engine::get_sprites_at(pickup_tile).front();
                if (Engine::is_object_at(object_tile,object->get_id())) {
                    // add object to inventory
                    ObjectManager::get_instance().get_object<Sprite>(id)->add_to_inventory(object);
                } else {
                    // try to remove object from inventory
                    bool was_returned = ObjectManager::get_instance().get_object<Sprite>(id)->remove_from_inventory(object);
                    if (was_returned) {
                        //reset object location
                        object->set_x_position(object_tile.x);
                        object->set_y_position(object_tile.y);
                    }
                }
                return true;
    });
}