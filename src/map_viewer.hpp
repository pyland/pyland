#ifndef MAPVIEWER_H
#define MAPVIEWER_H

#include "Map.h"

class MapViewer {
  ///
  /// The Map we are currently rendering
  ///
  Map* map = NULL;

  ///
  /// The reference to the window object in which we render the map.
  ///
  GameWindow *window = NULL;

public:
  MapViewer(GameWindow* window);
  ~MapViewer();
  
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
