#ifndef MAPVIEWER_H
#define MAPVIEWER_H

#include "map.hpp"

class MapViewer {
    ///
    /// The Map we are currently rendering
    ///
    Map* map = nullptr;

    ///
    /// Object to focus the map on
    /// 
    int map_focus_object = 0;
    ///
    /// The reference to the window object in which we render the map.
    ///
    GameWindow *window = nullptr;

public:
    MapViewer(GameWindow* window);
    ~MapViewer();

    ///
    /// Refocus the map back onto the object it is focusses on
    /// 
    void refocus_map();

    ///
    /// Set the id of the object to focus on 
    /// @param object_id the id of the object to focus the map view on
    ///  
    void set_map_focus_object(int object_id);

    ///
    /// Get the id of the object to focus on
    /// @return the id of the the object to focs the map on
    ///
    int get_map_focus_object() { return map_focus_object; }

    ///
    /// Render the currently bound map
    ///
    void render_map();
  
    ///
    /// Update the map and any objects in it
    /// @param dt The time delta between the current frame and the previous frame
    ///
    void update_map(float dt);

    ///
    /// Set the map that the viewer is managing
    /// @param new_map The new map to manage
    ///
    void set_map(Map* new_map);

    ///
    /// Return the map instance the viewer is managing
    /// @return The map instance the viewer manages
    Map* get_map();


};
#endif
