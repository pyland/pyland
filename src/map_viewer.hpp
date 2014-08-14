#ifndef MAPVIEWER_H
#define MAPVIEWER_H

#include "gui/gui_manager.hpp"

class GameWindow;
class Map;

class MapViewer {

    ///
    /// The Map we are currently rendering
    ///
    Map* map = nullptr;

    ///
    /// The GUI manager this viewer is using
    ///
    GUIManager* gui_manager;

    ///
    /// Object to focus the map on
    ///
    int map_focus_object = 0;
    ///
    /// The reference to the window object in which we render the map.
    ///
    GameWindow *window = nullptr;

    ///
    /// The far left  x position where we display the map
    ///
    float map_display_x = 0.0f;

    ///
    /// The lower y position we are currently displaying the map at.
    ///
    float map_display_y = 0.0f;

    ///
    /// The width of the map to be displayed on screen.
    ///
    float map_display_width;

    ///
    /// The height of the map to be displayed on screen
    ///
    float map_display_height;

    ///
    /// Render the GUI
    ///
    void render_gui();

    ///
    /// Render the map
    ///
    void render_map();

    ///
    /// Render objects on the map
    /// @param above_sprite if the object is to be rendered above the sprites
    ///
    void render_objects(bool above_sprite);

    ///
    /// Render sprites on the map
    ///
    void render_sprites();

public:
    MapViewer(GameWindow* window, GUIManager* manager);
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
    /// Render the currently bound map, objects and GUI
    ///
    void render();

    ///
    /// Set the map that the viewer is managing
    /// @param new_map The new map to manage
    ///
    void set_map(Map* new_map);

    ///
    /// Return the map instance the viewer is managing
    /// @return The map instance the viewer manages
    Map* get_map();

    ///
    /// Get the game window associated with this map viewer
    /// @return the game window used to view the map
    ///
    GameWindow* get_window() { return window; }

    GUIManager* get_gui_manager() { return gui_manager; }  

    ///
    /// Rejigg the map in response to the viewport size changing.
    ///
    void resize();

    ///
    /// Get the display width of the map
    /// @return get the display width of the map
    ///
    float get_display_width() { return map_display_width; }

    ///
    /// Set the display width of the map
    /// @param display_width the new display width of the map
    ///
    void set_display_width(float display_width) { map_display_width = display_width; }

    ///
    /// Get the display height of the map
    /// @return get the display height of the map
    ///
    float get_display_height() { return map_display_height; }

    ///
    /// Set the display height of the map
    /// @param display_width the new display height of the map
    ///
    void set_display_height(float display_height) { map_display_height = display_height; }


    ///
    /// Get the map display lower left x position
    /// @return the map display far left x position
    ///
    float get_display_x() { return map_display_x; }

    ///
    /// Set the x display position of the map
    /// @param new_display_x the new display position
    ///
    void set_display_x(float new_display_x) { map_display_x = new_display_x; }

    ///
    /// Get the map display bottom y position
    /// @return the map display bottom y  position
    ///
    float get_display_y() { return map_display_y; }

    ///
    /// Set the y display position of the map
    /// @param new_display_y the new display position
    ///
    void set_display_y(float new_display_y) { map_display_y = new_display_y; }

    ///
    /// converts pixel location inside window to a map tile
    ///
    glm::ivec2 pixel_to_tile (glm::ivec2 pixel_location);

    ///
    /// converts a tile in map to a pixel location in window of the bottem left corner of tile
    ///
    glm::ivec2 tile_to_pixel(glm::vec2 tile_location) ;

};

#endif
