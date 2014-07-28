///
/// Defines the API used for the C++/Python integration
///

#ifndef ENGINE_API_H
#define ENGINE_API_H

#include <array>
#include "api.hpp"
#include <utility>
#include <vector>

class MapViewer;

///
/// size of each tile in pixels
///
#define TILESET_ELEMENT_SIZE 16
#define DEFAULT_PY_EDITOR "gedit"

//Wrap the api calls into a static public class
class Engine {

    static std::string editor;
    static MapViewer* map_viewer;
public:
    ///
    /// Set the map viewer attached to the engine
    /// @param _map_viewer the map viewer which is attached to the engine 
    ///
    static void set_map_viewer(MapViewer* _map_viewer) { map_viewer = _map_viewer; }


    ///
    /// Get the map viewer attached to the engine
    /// @return a pointer to the map viewer
    ///
    static MapViewer* get_map_viewer() { return map_viewer;    }

    ///
    /// Move sprite onscreen
    ///
    /// @param id ID of sprite to move 
    /// @param dx move in x by dx tiles
    /// @param dy move in x by dy tiles
    ///
    static bool move_object(int id, Vec2D move_by);

    ///
    /// Determine if a location can be walked on
    /// @param x_pos the x position to test
    /// @param y_pos the y position to test
    /// @return true if the position can be walked on, false if not
    ///
    static bool walkable(Vec2D location);

    ///
    /// Change the tile in the map in the given layer at the provided position
    /// @param new_id the id of the new tile
    /// @param x the x position of the tile to change
    /// @param y the y position of the tile to change
    /// @param layer the layer of the tile to change
    /// @return indicates if the operation completed successfully
    ///
    static bool change_tile(int new_id, Vec2D location, int layer);

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
    static std::vector<int> get_tiles(Vec2D location);

    ///
    /// Get the objects at the given map position if any, empty if no objects
    /// @param x the x position on the map
    /// @param y the y position on the map
    /// @return a vector of all the objects at that position on the map
    ///
    static std::vector<int> get_objects(Vec2D location);

    ///
    /// Load the map specified by the ap id
    /// @param map_id the id of the map to load
    /// @return indicate if the map was successfully loaded
    ///
    static bool load_map(int map_id);

    ///
    /// Get the locationof tte object in the map, throws exception if
    /// there is the object is not on the map
    /// @id the id of the object
    /// @return a pair which is the (x, y) tuple of the object position
    ///
    static Vec2D find_object(int id);
    
    ///
    /// Open a text editor for the user to edit a file
    ///
    static bool open_editor(std::string filename);

    ///
    /// Get the size of a tile in the current map
    /// @return the size of the tile in pixels - we only support square ones
    ///
    static int get_tile_size();

    ///
    /// Get a list of objects at this point
    ///
    ///
    static std::vector<int> get_objects_at(Vec2D location);

    ///
    /// Add an object to the map at the given location
    /// @param object_id the id of the object to add
    /// @param x_pos the x position of the object
    /// @param y_pos the y position of the object
    ///
    static void add_object(int object_id, Vec2D location);

    ///
    /// Remove an object from the map at a givene location
    /// @param object_id the id of the object to add
    /// @param x_pos the x position of the object 
    /// @param y_pos the y position of the object
    ///
    static void remove_object(int object_id, Vec2D location);

    static void set_editor(std::string editor);
};
#endif
