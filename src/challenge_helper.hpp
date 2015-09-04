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

#include "challenge.hpp"
#include "dispatcher.hpp"
#include "engine.hpp"
#include "event_manager.hpp"
#include "fml.hpp"
#include "map.hpp"
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
    /// @param name the human friendly name to give the object
    ///
    ///
    int make_object(Challenge *challenge,
                    std::string name,
                    Walkability walkability,
                    glm::ivec2 position);

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
    /// saves the fact that this challenge has been completed, call at end of challenge
    ///
    void set_completed_level(int challenge_id);

    ///
    /// returns the next challenge that is to be completed
    int get_current_level ();
}

template <class Container>
void ChallengeHelper::unregister_all(Container *callbacks) {
    EventManager::get_instance()->add_event([callbacks] () {
        auto *map = Engine::get_map_viewer()->get_map();
        if (!map) { return; }

        for (auto callback : *callbacks) {
            map->event_step_on.unregister(callback);
        }

        callbacks->clear();
    });
}
#endif
