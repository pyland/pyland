///
/// Defines the API used for the C++/Python integration
///

#ifndef ENGINE_API_H
#define ENGINE_API_H

#include <array>
#include <utility>
#include <vector>

class MapViewer;

///
/// size of each tile in pixels
///
#define TILESET_ELEMENT_SIZE 16

//Wrap the api calls into a static public class
class Engine {

    static MapViewer* map_viewer;
public:
    static void set_map_viewer(MapViewer* _map_viewer) { map_viewer = _map_viewer; }
    static MapViewer* get_map_viewer() { return map_viewer;    }

    ///
    /// Move sprite onscreen
    ///
    /// @param id ID of sprite to move 
    /// @param dx move in x by dx pixels
    /// @param dy move in x by dy pixels
    ///
    static bool move_object(int id, int tile_dx, int tile_dy);

    ///
    /// Determine if a location can be walked on
    /// @param x_pos the x position to test
    /// @param y_pos the y position to test
    /// @return true if the position can be walked on, false if not
    ///
    static bool walkable(int x_pos, int y_pos);

    ///
    /// Change the tile in the map in the given layer at the provided position
    /// @param new_id the id of the new tile
    /// @param x the x position of the tile to change
    /// @param y the y position of the tile to change
    /// @param layer the layer of the tile to change
    /// @return indicates if the operation completed successfully
    ///
    static bool change_tile(int new_id, int x, int y, int layer);

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
    static std::vector<int> get_tiles(int x, int y);

    ///
    /// Get the objects at the given map position if any, empty if no objects
    /// @param x the x position on the map
    /// @param y the y position on the map
    /// @return a vector of all the objects at that position on the map
    ///
    static std::vector<int> get_objects(int x, int y);

    ///
    /// Load the map specified by the ap id
    /// @param map_id the id of the map to load
    /// @return indicate if the map was successfully loaded
    ///
    static bool load_map(int map_id);

    ///
    /// Get the locationof hte object in the map, returns (-1, -1) if
    /// there is the object is not on the map
    /// @id the id of the object
    /// @return a pair which is the (x, y) tuple of the object position
    ///
    static std::pair<int, int> find_object(int id);
    
    ///
    /// Open a text editor for the user to edit a file
    ///
    static bool open_editor(std::string filename);

    ///
    /// Get the size of a tile in the current map
    /// @return the size of the tile in pixels - we only support square ones
    ///
    static int get_tile_size();
};
#endif
