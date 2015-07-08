#ifndef CHALLENGE_HELPER_H
#define CHALLENGE_HELPER_H

///
/// Challenge Helper is collection of static methods to aid/abstract with challenge writing
///

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

class Challenge;

namespace ChallengeHelper {
    ///
    /// Unregister a container of many location callbacks
    ///
    template <class Container>
    void unregister_all(Container *callbacks);

    ///
    /// Create MapMapObject from named location
    /// @param challenge a pointer to the challenge to make the object for
    /// @param markername the name of the object used to identify it in the TMX map file
    /// @param walkability the walkability status of the object
    /// @param start_frame which frame to start animating the object from
    /// @param cuttable if the object can be cut down using the cut API call
    /// @param findable if the object can be found using the loop API call
    ///
    int make_object(Challenge *challenge,
                    std::string markername,
                    Walkability walkability,
                    std::string start_frame);
    ///
    /// Create MapMapObject from named location
    /// @param challenge a pointer to the challenge to make the object for
    /// @param markername the name of the object used to identify it in the TMX map file
    /// @param walkability the walkability status of the object
    /// @param start_frame which frame to start animating the object from
    /// @param name the human friendly name to give the object
    /// @param cuttable if the object can be cut down using the cut API call
    /// @param findable if the object can be found using the loop API call
    ///
    ///
    int make_object(Challenge *challenge,
                    std::string markername,
                    Walkability walkability,
                    std::string start_frame,
                    std::string name);

    ///
    /// Create MapMapObjects from named locations
    /// @param challenge a pointer to the challenge to make the object for
    /// @param markername the name of the object used to identify it in the TMX map file
    /// @param walkability the walkability status of the object
    /// @param output
    /// @param start_frame which frame to start animating the object from
    /// @param name the human friendly name to give the object
    /// @param cuttable if the object can be cut down using the cut API call
    /// @param findable if the object can be found using the loop API call
    ///
    template <class OutputIt>
    void make_objects(Challenge *challenge,
                      std::string marker_name,
                      Walkability walkability,
                      OutputIt output,
                      std::string start_frame="",
                      bool cuttable=false,
                      bool findable=true);
    ///
    /// Create MapMapObjects from named locations
    /// @param challenge a pointer to the challenge to make the object for
    /// @param markername the name of the object used to identify it in the TMX map file
    /// @param walkability the walkability status of the object
    /// @param output a vector of the objects to be inserted
    /// @param start_frame which frame to start animating the object from
    /// @param name the human friendly name to give the object
    /// @param cuttable if the object can be cut down using the cut API call
    /// @param findable if the object can be found using the loop API call
    ///
    template <class OutputIt>
    void make_objects(Challenge *challenge,
                      std::string marker_name,
                      Walkability walkability,
                      OutputIt output,
                      std::string name,
                      std::string start_frame="");

    ///
    /// Attach callback to a position
    /// @param name the name given to the object on the map
    /// @param callback the lambda to be run when the dispatcher fires
    /// @return the id of the callback that is created by make_interaction
    ///
    PositionDispatcher<int>::CallbackID
    make_interaction(std::string name,
                     std::function<bool (int)> callback);

    PositionDispatcher<int>::CallbackID
    lose_interaction(std::string name,
                     std::function<bool (int)> callback);

    ///
    /// Attach callbacks to a set of positions
    ///
    template <class OutputIt>
    void make_interactions(std::string name,
                           OutputIt output,
                           std::function<bool (int)> callback);

    ///
    /// Given an object, allow it to picked up and dropped off at specific locations
    /// Only one pick up and drop off is permitted
    /// @param start_tile the tile that the object starts on
    /// @param pickup_tile the tile that the sprite steps onto to pick up the object
    /// @param finish_tile the tile that the object is dropped onto
    /// @param dropoff_tile the tile that the player steps onto to drop off the object
    /// @param object_id the id of the object to do this to
    /// @param repeat is whether you can do this multiple times e.g. keep picking it up
    ///               putting it down
    ///
    void create_pickupable(glm::ivec2 start_tile,
                           glm::ivec2 pickup_tile,
                           glm::ivec2 finish_tile,
                           glm::ivec2 dropoff_tile,
                           int object_id,
                           bool repeat);

    ///
    /// Allow an object to picked up / dropped off at the same location
    ///
    void create_pickupable(glm::ivec2 start_tile,
                           glm::ivec2 pickup_tile,
                           int object_id);

    ///
    /// saves the fact that this challenge has been completed, call at end of challenge
    ///
    void set_completed_level(int challenge_id);

    ///
    /// returns the next challenge that is to be completed
    int get_current_level ();

    ///
    /// returns the tile location of a named object in the interaction layer
    ///
    glm::vec2 get_location_interaction (std::string name);

    ///
    /// returns the tile location of a named object in the object layer
    ///
    glm::vec2 get_location_object (std::string name);
}

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
                                   std::string, //marker_name
                                   Walkability walkability,
                                   OutputIt output,
                                   std::string start_frame,
                                   std::string name,
                                   bool cuttable,
                                   bool findable) {
    auto *map(Engine::get_map_viewer()->get_map());
    auto begin(maptools::start_of(map->locations, "MapObjects/" + name));
    auto end  (maptools::end_of  (map->locations, "MapObjects/" + name));

    if (begin == end) {
        throw std::runtime_error("No such object: " + name);
    }

    std::transform(begin, end, output,
        [&] (std::pair<std::string, MapObjectProperties> name_properties) {
            return challenge->make_object(
                name_properties.second.position, //set the position fo the object
                name_properties.first, //set the name of the objext
                walkability, //set wether it is walkable or not
                AnimationFrames(name_properties.second.sprite_file_location), //TODO: work out animation stuff!
                name_properties.second.sprite_file_location
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
        [&] (std::pair<std::string, MapObjectProperties> name_properties) {
            return map->event_step_on.register_callback(
                name_properties.second.position,
                callback
            );
        }
    );
}

#endif
