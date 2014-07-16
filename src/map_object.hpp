#ifndef MAP_OBJECT_H
#define MAP_OBJECT_H

#include <string>

///
/// An object in the map, different from an Object in the engine
///
class MapObject {
    ///
    /// The name of the map object
    ///
    std::string name = "";

    ///
    /// The x position of the object
    ///
    int x_position = 0;

    ///
    /// The y position of the object
    /// 
    int y_position = 0;

public:
    ///
    /// Get the name of the object
    ///
    std::string get_name() { return name; }

    ///
    /// Set the name of the object
    ///
    void set_name(std::string _name) { name = _name; }

    ///
    /// Get the x position of the object
    ///
    int get_x_position() { return x_position; }

    ///
    /// Set the x position of the object
    ///
    void set_x_position(int x_pos) { x_position = x_pos; }

    ///
    /// Get the y position of the object
    ///
    int get_y_positioni() { return y_position; }

    ///
    /// Set the y position of the object
    ///
    void set_y_position(int y_pos) { y_position = y_pos; }
};

#endif
