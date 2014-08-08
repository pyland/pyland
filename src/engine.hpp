///
/// Defines the API used for the C++/Python integration
///

#ifndef ENGINE_H
#define ENGINE_H

#include <array>
#include <future>
#include <glm/vec2.hpp>
#include <memory>
#include <utility>
#include <vector>

#include "gil_safe_future.hpp"
#include "notification_stack.hpp"
#include "typeface.hpp"
#include "text_font.hpp"
#include "text.hpp"

class MapViewer;

enum class Status {RUNNING, STOPPED, FAILED};
enum class Direction {NEXT, PREVIOUS};

///
/// default python editor, used as long as another isn't passed as command line arg
#define DEFAULT_PY_EDITOR "gedit"

// Class wrapping the API calls into a static public class
class Engine {
private:

    ///
    /// name of editor used for python editing
    ///
    static std::string editor;

    static MapViewer *map_viewer;

    ///
    /// pointer for text box
    ///
    static Text* dialogue_box;

    ///
    /// cache for hold past notification
    ///
    static Notification notification_stack;

    ///
    /// The size of a tile
    ///
    static int tile_size;

    ///
    /// The global tile and object scaling factor, how much to multiply widths and heights
    /// by
    ///
    static float global_scale;

public:
    ///
    /// Get the global scale
    /// @return the global scale
    ///
    static float get_global_scale() { return global_scale; }

    ///
    /// Set the global scaling factor
    /// @param _global_scale the scaling factor
    ///
    static void set_global_scale(float global_scale) { Engine::global_scale = global_scale; }

    ///
    /// Set the tile size to be used by the engine
    /// @param _tile_size the tile size
    ///
    static void set_tile_size(int tile_size) { Engine::tile_size = tile_size; }

    ///
    /// Get the tile size used by the engine  in pixels - we only support square ones
    /// @return the tile size
    ///
    static int get_tile_size() { return tile_size; }

    static float get_actual_tile_size() { return float(tile_size) * global_scale; }
    ///
    /// Set the map viewer attached to the engine
    /// @param _map_viewer the map viewer which is attached to the engine
    ///
    static void set_map_viewer(MapViewer *map_viewer) { Engine::map_viewer = map_viewer; }


    ///
    /// Get the map viewer attached to the engine
    /// @return a pointer to the map viewer
    ///
    static MapViewer *get_map_viewer() { return map_viewer; }

    ///
    /// Move sprite onscreen
    ///
    /// @param id ID of sprite to move
    /// @param dx move in x by dx tiles
    /// @param dy move in x by dy tiles
    ///
    static void move_sprite(int id, glm::ivec2 move_by);
    static void move_sprite(int id, glm::ivec2 move_by, GilSafeFuture<bool> walk_succeeded_return);

    ///
    /// Determine if a location can be walked on
    /// @param x_pos the x position to test
    /// @param y_pos the y position to test
    /// @return true if the position can be walked on, false if not
    ///
    static bool walkable(glm::ivec2 location);

    ///
    /// Change the tile in the map in the given layer at the provided position
    /// @param new_id the id of the new tile
    /// @param x the x position of the tile to change
    /// @param y the y position of the tile to change
    /// @param layer the layer of the tile to change
    /// @return indicates if the operation completed successfully
    ///
    static void change_tile(glm::ivec2 tile, int layer_num, int tile_id);

    ///
    /// Get the location of the map object or sprite in the map, throws exception if
    /// there is the object is not on the map
    /// @id the id of the object
    /// @return a pair which is the (x, y) tuple of the object position
    ///
    static glm::vec2 find_object(int id);

    ///
    /// Open a text editor for the user to edit a file
    /// @param filename name of file in scripts directory
    ///
    static void open_editor(std::string filename);

    ///
    /// Get a list of objects at this point
    /// @return a vector of object ids
    ///
    static std::vector<int> get_objects_at(glm::vec2 location);

    ///
    /// Get a list of sprites at this point
    /// @return a vector of object ids
    ///
    static std::vector<int> get_sprites_at(glm::vec2 location);

    ///
    /// set the text editor, opened by the challenges
    ///
    static void set_editor(std::string editor) { Engine::editor = editor; }

    static void set_dialogue_box(Text *dialogue_box) { Engine::dialogue_box = dialogue_box; }
    static Text *get_dialogue_box(){ return dialogue_box; }

    static void print_dialogue(std::string name, std::string text);
    static void move_notification(Direction direction);

    static void text_displayer();
    static void text_updater();
    static void update_status(int id, std::string status);

};

#endif
