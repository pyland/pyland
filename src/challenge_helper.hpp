///
/// Challenge Helper is collection of static methods to aid/abstract with challenge writing
///
#include <glm/vec2.hpp>

#include "map_object.hpp"

class ChallengeHelper {
public:
    ///
    /// Given an object, allow it to picked up and dropped off at specific locations
    /// Only one pick up and drop off is permitted
    ///
    static void create_pickupable(glm::ivec2 start_tile,
                                  glm::ivec2 pickup_tile,
                                  glm::ivec2 finish_tile,
                                  glm::ivec2 dropoff_tile,
                                  int object_id);

    ///
    /// Allow an object to picked up / dropped off at the same location
    ///
    static void create_pickupable(glm::ivec2 start_tile,
                                  glm::ivec2 pickup_tile,
                                  int object_id);
};
