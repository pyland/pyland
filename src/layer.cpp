#include "layer.hpp"
#include <iostream>
void Layer::add_tile(const std::string tileset, int tile_id) {

   //Add to the layer the tileset and tile id pair
    layer->push_back(std::make_pair(tileset, tile_id));

}
