/** 
 * Defines the API used for the C++/Python integration
 */

#ifndef API_H
#define API_H
#include <array>

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
void move_object(const int id, const float dx, const float dy);

/// Holds the overall map data
//0th tile is top left
extern std::array<std::array<int, 16>, 16> world_data;

#endif
