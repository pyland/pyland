#include "layer.hpp"
#include <iostream>

void Layer::add_tile(const std::string tileset, int tile_id) {

   //Add to the layer the tileset and tile id pair
    layer->push_back(std::make_pair(tileset, tile_id));

}

int Layer::get_tile(int x_pos, int y_pos) {
    //Fetch the tile at the required position
    int tile_id = (*layer).at(x_pos + (y_pos)*width).second;
    return tile_id;
}
