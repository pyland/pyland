#include "engine_api.hpp"
#include <utility>
#include <vector>


bool change_tile(int new_id, int x, int y, int layer) {
    return false;
}

std::vector<int> get_tiles(int x, int y) {
    std::vector<int> tiles;
    return tiles;
}

std::vector<int> get_objects(int x, int y) {
    std::vector<int> objects;
    return objects;
}

std::pair<int, int> find_object(int id) {
    std::pair<int, int> position;
    return position;
}

bool load_map(int map_id) {
    return false;
}
