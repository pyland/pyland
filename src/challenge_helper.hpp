#ifndef CHALLENGE_HELPER_H
#define CHALLENGE_HELPER_H

///
/// Challenge Helper is collection of static methods to aid/abstract with challenge writing
///
#include <functional>
#include <glm/vec2.hpp>
#include <string>

#include "dispatcher.hpp"
#include "walkability.hpp"
class Challenge;


namespace ChallengeHelper {
    ///
    /// Unregister a container of many location callbacks
    ///
    template <class Container>
    void unregister_all(Container *callbacks);

    ///
    /// Create MapObject from named location
    ///
    int make_object(Challenge *challenge, std::string name, Walkability walkability);

    ///
    /// Create Sprite from named location
    ///
    int make_sprite(Challenge *challenge, std::string marker_name, std::string sprite_name, Walkability walkability);

    ///
    /// Create MapObjects from named locations
    ///
    template <class OutputIt>
    void make_objects(Challenge *challenge, std::string name, Walkability walkability, OutputIt output);

    ///
    /// Attach callback to a position
    ///
    PositionDispatcher<int>::CallbackID
    make_interaction(std::string name,
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
    ///
    void create_pickupable(glm::ivec2 start_tile,
                           glm::ivec2 pickup_tile,
                           glm::ivec2 finish_tile,
                           glm::ivec2 dropoff_tile,
                           int object_id);

    ///
    /// Allow an object to picked up / dropped off at the same location
    ///
    void create_pickupable(glm::ivec2 start_tile,
                           glm::ivec2 pickup_tile,
                           int object_id);

    ///
    /// saves the fact that this challenge has been completed
    ///
    void set_completed_level(int challenge_id);

    int get_current_level ();
}

#include "challenge_helper.hxx"

#endif
