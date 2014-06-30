/**
 * The MapViewer class is used to hold all the map information that we
 * need to render the map. It holds the map data and is responsible
 * for rendering the map.
 */

class MapViewer {

 private:
  int map_width, map_height;
  int tileset_width, tileset_height;
  void generate_tileset_coords();
  void generate_map_tex_coords();
  void generate_map_vert_coords();

 public:
  MapViewer();
  ~MapViewer();
  bool load_map();
  void draw_map();
  int get_map_width() { return map_width; }
  int get_map_height() { return map_height; }
  int get_tileset_width() { return tileset_width; }
  int get_tileset_height() { return tileset_height; }

}
