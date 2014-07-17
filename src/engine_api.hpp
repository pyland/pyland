///
/// Defines the API used for the C++/Python integration
///

#ifndef ENGINE_API_H
#define ENGINE_API_H
#include <array>
#include <utility>
#include <vector>

///
/// size of each tile in pixels
///
#define TILESET_ELEMENT_SIZE 16

///
/// Move sprite onscreen
///
/// @param id ID of sprite to move 
/// @param dx move in x by dx pixels
/// @param dy move in x by dy pixels
///
void move_object(const int id, const int tile_dx, int tile_dy);

///
/// Change the tile in the map in the given layer at the provided position
/// @param new_id the id of the new tile
/// @param x the x position of the tile to change
/// @param y the y position of the tile to change
/// @param layer the layer of the tile to change
/// @return indicates if the operation completed successfully
///
bool change_tile(int new_id, int x, int y, int layer);

///
/// Gets the ids of the tiles at this location. Layer 0 is the first
/// element in the vector. Then each layer follows. If the id is 0
/// then this  means that there is no tile in this layer
/// @param x the x location to fetch the tile list for
/// @param y the y location to fetch the tile list for
/// @return a vector of the tiles, stored in layer order with layer 0
/// as the first element. A 0 is stored for a layer if there is no
/// tile there
///
std::vector<int> get_tiles(int x, int y);

///
/// Get the objects at the given map position if any, empty if no objects
/// @param x the x position on the map
/// @param y the y position on the map
/// @return a vector of all the objects at that position on the map
///
std::vector<int> get_objects(int x, int y);
///
/// Get the position of the given object in the map
/// @param id the id of the object to find
/// @return a pair of the (x, y) coordinates
///  
std::pair<int, int> find_object(int id);

///
/// Load the map specified by the ap id
/// @param map_id the id of the map to load
/// @return indicate if the map was successfully loaded
///
bool load_map(int map_id);

/// Holds the overall map data
//0th tile is top left
extern std::array<std::array<int, 16>, 16> world_data;

#endif
