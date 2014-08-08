///
/// Challenge Helper is collection of static methods to aid/abstract with challenge writing
///
#include "api.hpp"
#include "map_object.hpp"

class ChallengeHelper {
public:

    ///
    /// given an object, allow it to picked up and dropped off at specific locations
    /// only one pick up and drop off is permitted
    ///
    static void create_pickupable(Vec2D start_tile, Vec2D pickup_tile, 
                           Vec2D finish_tile, Vec2D dropoff_tile, 
                           std::shared_ptr<MapObject> object);

    /// allowing an object to picked up / dropped off at the same location
    static void create_pickupable(Vec2D start_tile, Vec2D pickup_tile, std::shared_ptr<MapObject> object );
};