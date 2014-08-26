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
                    std::string start_frame,
                    bool cuttable=false,
                    bool findable=true);
    ///
    /// Create MapObject from named location
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
                    std::string name,
                    bool cuttable=false,
                    bool findable=true);

    ///
    /// Create Sprite from named location
    /// @param challenge a pointer to the challenge to make the object for
    /// @param markername the name of the object used to identify it in the TMX map file
    /// @param sprite_name the human readable name to give the character
    /// @param walkability the walkability status of the object
    /// @param start_frame which frame to start animating the object from
    ///
    int make_sprite(Challenge *challenge,
                    std::string marker_name,
                    std::string sprite_name,
                    Walkability walkability,
                    std::string start_frame);

    ///
    /// Create MapObjects from named locations
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
    /// Create MapObjects from named locations
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
                      std::string start_frame="",
                      bool cuttable=false,
                      bool findable=true);

    ///
    /// Attach callback to a position
    /// @param name the name given to the object on the map
    /// @param callback the lambda to be run when the dispatcher fires
    /// @return the id of the callback that is created by make_interaction 
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
    /// @param start_tile the tile that the object starts on
    /// @param pickup_tile the tile that the sprite steps onto to drop the tile off
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

#include "challenge_helper.hxx"

#endif
