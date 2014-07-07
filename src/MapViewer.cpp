#include "MapViewer.h"

MapViewer::MapViewer() {

};

MapViewer::~MapViewer() {

}

void MapViewer::render_map() {
  map->render_map();
}


void MapViewer::update_map(float dt ) {
  map->update_map(dt);
}

void MapViewer::set_map(Map* new_map) {
  map = new_map;
}


Map* MapViewer::get_map() {
  return map;
}
