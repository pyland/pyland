#include <algorithm>
#include <functional>
#include <iterator>
#include <string>
#include <vector>

#include "event_manager.hpp"
#include "dispatcher.hpp"
#include "engine.hpp"
#include "map_viewer.hpp"
#include "object_manager.hpp"

template <class Container>
void ChallengeHelper::unregister_all(Container *callbacks) {
    EventManager::get_instance().add_event([callbacks] () {
        auto *map = Engine::get_map_viewer()->get_map();
        if (!map) { return; }

        for (auto callback : *callbacks) {
            map->event_step_on.unregister(callback);
        }

        callbacks->clear();
    });
}

template <class OutputIt>
void ChallengeHelper::make_objects(std::string name, Walkability walkability, OutputIt output) {

    auto *map = Engine::get_map_viewer()->get_map();
    auto begin(map->locations.begin("Objects/" + name));
    auto end  (map->locations.end  ("Objects/" + name));

    if (begin == end) {
        throw std::runtime_error("No such object: " + name);
    }

    std::transform(begin, end, output,
        [&] (std::pair<std::string, int> name_id) {
            auto properties(map->obj_from_id(name_id.second));

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
    );
}

template <class OutputIt>
void ChallengeHelper::make_interactions(std::string name,
                                        OutputIt output,
                                        std::function<bool (int)> callback) {

    auto *map = Engine::get_map_viewer()->get_map();
    auto begin(map->locations.begin("Interactions/" + name));
    auto end  (map->locations.end  ("Interactions/" + name));

    if (begin == end) {
        throw std::runtime_error("No such interaction: " + name);
    }

    std::transform(begin, end, output,
        [&] (std::pair<std::string, int> name_id) {
            return map->event_step_on.register_callback(
                map->obj_from_id(name_id.second).location,
                callback
            );
        }
    );
}
