#include "layer.hpp"
#include "tileset.hpp"

Layer::Layer(int width_tiles, int height_tiles, std::string name) :
    width_tiles(width_tiles),
    height_tiles(height_tiles),
    name(name),
    layer(std::make_shared<std::vector<std::pair<std::shared_ptr<TileSet>, int>>>()),
    packing(Packing::DENSE),
    location_texture_vbo_offset_map() {

    renderable_component = std::make_shared<RenderableComponent>();
}

std::shared_ptr<RenderableComponent> Layer::get_renderable_component(){
	return renderable_component;
}

void Layer::add_tile(std::shared_ptr<TileSet> tileset, int tile_id) {

    //Add to the layer the tileset and tile id pair
    layer->push_back(std::make_pair(tileset, tile_id));
    // Temporary single-tileset hack.
    if (tileset) {
        renderable_component->set_texture(tileset->get_atlas());
    }
}

std::pair<std::shared_ptr<TileSet>, int> Layer::get_tile(int x_pos, int y_pos) {
    //Fetch the tile at the required position
    return layer->at(x_pos + y_pos * width_tiles);
}

void Layer::update_tile(int x_pos, int y_pos, int tile_id, std::shared_ptr<TileSet> tileset) {
    LayerInvalidException layer_invalid_exception;
    if(x_pos < 0 || x_pos >= width_tiles || y_pos < 0 || y_pos >= height_tiles)
        throw layer_invalid_exception;
    //Make the pair

    //Set the new tile
    (*layer)[y_pos*width_tiles + x_pos] = std::make_pair(tileset, tile_id);;
}
