#ifndef MAPVIEWER_H
#define MAPVIEWER_H

#include "Map.h"

class MapViewer {
  Map* map;

 public:
  MapViewer();
  ~MapViewer();

  void render_map();
  void update_map(float dt);
  void set_map(Map* new_map);
  Map* get_map();


};
#endif
