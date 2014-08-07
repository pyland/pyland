#include "layer.hpp"
#include <exception>
#include <iostream>

Layer::Layer(int width_tiles, int height_tiles, std::string name) :
    width_tiles(width_tiles),
    height_tiles(height_tiles),
    name(name),
    layer(std::make_shared<std::vector<std::pair<std::string, int>>>()),
    packing(Packing::DENSE),
    location_texture_vbo_offset_map() {
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

void Layer::update_tile(int x_pos, int y_pos, int tile_id, std::string tileset) {
    LayerInvalidException layer_invalid_exception;
    if(x_pos < 0 || x_pos >= width_tiles || y_pos < 0 || y_pos >= height_tiles)
        throw layer_invalid_exception;
    //Make the pair

    //Set the new tile
    (*layer)[y_pos*width_tiles + x_pos] = std::make_pair(tileset, tile_id);;
}
