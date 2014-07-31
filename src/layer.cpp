#include "layer.hpp"
#include <iostream>

Layer::Layer(int _width_tiles, int _height_tiles, std::string _name) : 
    width_tiles(_width_tiles), height_tiles(_height_tiles),
    name(_name), packing(Packing::DENSE),
    layer(std::make_shared<std::vector<std::pair<std::string, int>>>()) {

}

void Layer::add_tile(const std::string tileset, int tile_id) {

   //Add to the layer the tileset and tile id pair
    layer->push_back(std::make_pair(tileset, tile_id));

}

int Layer::get_tile(int x_pos, int y_pos) {
    //Fetch the tile at the required position
    int tile_id = (*layer).at(x_pos + (y_pos)*width_tiles).second;
    return tile_id;
}
