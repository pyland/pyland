// Fixes bug with lambdas for transform_iterator on older compilers
#define BOOST_RESULT_OF_USE_DECLTYPE

#include <algorithm>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <functional>
#include <glm/vec2.hpp>
#include <stdexcept>
#include <string>
#include <utility>

#include "animation_frames.hpp"
#include "challenge.hpp"
#include "dispatcher.hpp"
#include "engine.hpp"
#include "event_manager.hpp"
#include "fml.hpp"
#include "map.hpp"
#include "maptools.hpp"
#include "map_loader.hpp"
#include "map_viewer.hpp"
#include "walkability.hpp"


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
void ChallengeHelper::make_objects(Challenge *challenge,
                                   std::string marker_name,
                                   Walkability walkability,
                                   OutputIt output,
                                   std::string start_frame,
                                   std::string name,
                                   bool cuttable,
                                   bool findable) {
    auto *map(Engine::get_map_viewer()->get_map());
    auto begin(maptools::start_of(map->locations, "Objects/" + name));
    auto end  (maptools::end_of  (map->locations, "Objects/" + name));

    if (begin == end) {
        throw std::runtime_error("No such object: " + name);
    }

    std::transform(begin, end, output,
        [&] (std::pair<std::string, ObjectProperties> name_properties) {
            return challenge->make_map_object(
                name_properties.second.location,
                name_properties.first,
                walkability,
                AnimationFrames(name_properties.second.tileset.substr(
                    0, name_properties.second.tileset.length() - start_frame.length() - 1
                )),
                start_frame,
                cuttable,
                findable
            );
        }
    );

}
template <class OutputIt>
void ChallengeHelper::make_objects(Challenge *challenge,
                                   std::string marker_name,
                                   Walkability walkability,
                                   OutputIt output,
                                   std::string start_frame,
                                   bool cuttable,
                                   bool findable) {
    make_object(challenge, marker_name, walkability, output, start_frame, marker_name, cuttable, findable);
}

template <class OutputIt>
void ChallengeHelper::make_interactions(std::string name,
                                        OutputIt output,
                                        std::function<bool (int)> callback) {

    auto *map = Engine::get_map_viewer()->get_map();
    auto begin(maptools::start_of(map->locations, "Interactions/" + name));
    auto end  (maptools::end_of  (map->locations, "Interactions/" + name));


    if (begin == end) {
        throw std::runtime_error("No such interaction: " + name);
    }

    std::transform(begin, end, output,
        [&] (std::pair<std::string, ObjectProperties> name_properties) {
            return map->event_step_on.register_callback(
                name_properties.second.location,
                callback
            );
        }
    );
}
