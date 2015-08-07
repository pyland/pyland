#include <algorithm>
#include <exception>
#include <fstream>
#include <glm/vec2.hpp>
#include <glog/logging.h>
#include <iterator>
#include <memory>
#include <new>
#include <string>
#include <tuple>
#include <utility>

#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif

#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

#include "cacheable_resource.hpp"
#include "config.hpp"
#include "dispatcher.hpp"
#include "engine.hpp"
#include "fml.hpp"
#include "layer.hpp"
#include "map.hpp"
#include "map_loader.hpp"
#include "map_object.hpp"
#include "object_manager.hpp"
#include "renderable_component.hpp"
#include "shader.hpp"
#include "texture_atlas.hpp"
#include "tileset.hpp"

Map::Map(const std::string map_src):
    event_step_on(glm::ivec2(0, 0)),
    event_step_off(glm::ivec2(0, 0))
    {
        //Load the map
        MapLoader map_loader;
        bool result = map_loader.load_map(map_src);
        if(!result)  {

            LOG(ERROR) << "Couldn't load map";
            return;
        }

        locations = map_loader.get_object_mapping(); //returns a vector of MapObjectProperties (see map_loader.hpp for structure)

        //Get the loaded map data
        map_width = map_loader.get_map_width();
        map_height = map_loader.get_map_height();

        // hack to construct postion dispatcher as we need map diametions
        event_step_on  = PositionDispatcher<int>(glm::ivec2(map_width, map_height));
        event_step_off = PositionDispatcher<int>(glm::ivec2(map_width, map_height));

        LOG(INFO) << "Map width: " << map_width << " Map height: " << map_height;
        std::vector<std::shared_ptr<Layer>> layers = map_loader.get_layers();
        for(auto layer : layers) {
            layer_ids.push_back(layer->get_id());
            ObjectManager::get_instance().add_object(layer);
        }

        tilesets = map_loader.get_tilesets();

        //Get the tilesets
        //TODO: We'll only support one tileset at the moment
        //Get an object list
        blocker = std::vector<std::vector<int>>(map_width, std::vector<int>(map_height, 0));

        //Generate the geometry needed for this map
        init_shaders();
        init_textures();
        // generate_tileset_coords(texture_atlases[0]);
        generate_data();
}

Map::~Map() {
    for(int layer_id : layer_ids) {
        ObjectManager::get_instance().remove_object(layer_id);
    }

    // release buffers
    LOG(INFO) << "Map destructed";
}

bool Map::is_walkable(int x_pos, int y_pos) {
    for(auto it = std::begin(layer_ids); it != std::end(layer_ids); ++it) {
        std::shared_ptr<Layer> layer = ObjectManager::get_instance().get_object<Layer>(*it);
        // Block only in the case where we're on the special layer and the tile is set
        if(layer->get_name() == Config::get_instance()["layers"]["special_layer_name"] && (layer->get_tile(x_pos, y_pos).second == 1)) {
            return false;
        }
    }
    return true;
}

int Map::get_tile_type(int x, int y) {
    for(auto it = layer_ids.begin(); it != layer_ids.end(); ++it) {
        std::shared_ptr<Layer> layer = ObjectManager::get_instance().get_object<Layer>(*it);
        if(layer->get_name() == Config::get_instance()["layers"]["special_layer_name"]) {
            return layer->get_tile(x, y).second;
        }
    }
    return -1; 
}


void Map::add_map_object(int object_id) {
    if(ObjectManager::is_valid_object_id(object_id))
        object_ids.push_back(object_id);
}

void Map::remove_map_object(int object_id) {
    if(ObjectManager::is_valid_object_id(object_id)){
        for(auto it = object_ids.begin(); it != object_ids.end(); ++it) {
            //If a valid object
            if(*it != 0) {
                //remove it if its the object
                if(*it == object_id) {
                    object_ids.erase(it);
                    return;
                }
            }
        }
    }
}

/**
 * The function used to generate the cache of tile texture coordinates.
 */
void Map::generate_tileset_coords(std::shared_ptr<TextureAtlas> atlas) {
    int texture_count = atlas->get_texture_count();

    LOG(INFO) << "Generating tileset texture coords";

    if(Engine::get_tile_size() == 0) {
        LOG(ERROR) << "Tile size is 0 in Map::generate_tileset_coords";
        return;
    }

    LOG(INFO) << "Tileset texture count: " << texture_count;

    //Each tile needs 8 floats to describe its position in the image
    try {
        tileset_tex_coords = new GLfloat[texture_count * 4 * 2];
    }
    catch (std::bad_alloc& ba) {
        LOG(ERROR) << "Out of Memory in Map::generate_tileset_coords";
        return;
    }

    int data_i = 0;
    for (int i = 0; i < texture_count; ++i) {
        std::tuple<float,float,float,float> bounds = atlas->index_to_coords(i);

        // bottom left
        tileset_tex_coords[data_i++] = std::get<0>(bounds);
        tileset_tex_coords[data_i++] = std::get<2>(bounds);

        // top left
        tileset_tex_coords[data_i++] = std::get<0>(bounds);
        tileset_tex_coords[data_i++] = std::get<3>(bounds);

        // bottom right
        tileset_tex_coords[data_i++] = std::get<1>(bounds);
        tileset_tex_coords[data_i++] = std::get<2>(bounds);

        // top right
        tileset_tex_coords[data_i++] = std::get<1>(bounds);
        tileset_tex_coords[data_i++] = std::get<3>(bounds);
    }
}

void Map::generate_data() {
    LOG(INFO) << "Generating map data";

    // Get each layer of the map
    // Start at layer 0
    int layer_num = 0;
    for (int layer_id : layer_ids) {
        std::shared_ptr<Layer> layer = ObjectManager::get_instance().get_object<Layer>(layer_id);

        auto layer_data = layer->get_layer_data();


        // Build the mapping from (x, y) to offsets in the vertex and texture buffers
        auto buffer_map = std::make_shared<std::map<int, int>>();
        layer_mappings[layer_num] = buffer_map;


        // Don't generate data for the collisions layer
        // TODO: handle this not being in layer_mappings
        // TODO: let python have some control over which layers are special
        if (layer->get_name() == Config::get_instance()["layers"]["special_layer_name"]) {
            layer_num++;
            continue;
        }


        // Work out if we need a dense or a sparse buffer
        int total_tiles = 0;
        int num_blank_tiles = 0;

        // Get all the tiles in the layer, moving from left to right and down
        for (auto &tile_data : *layer_data) {
            std::shared_ptr<TileSet> tileset = tile_data.first;
            if (!tileset) { num_blank_tiles++; }

            total_tiles++;
        }

        // Spare packing by default
        auto layer_packing(Layer::Packing::SPARSE);

        // If more than 50% of layer has tiles, make it dense
        if (num_blank_tiles < total_tiles / 2) {
            layer_packing = Layer::Packing::DENSE;
        }

        layer->set_packing(layer_packing);

        // Create the buffer for the layer
        int num_tiles(layer_packing == Layer::Packing::DENSE ? total_tiles
                                                             : total_tiles - num_blank_tiles);

        // The number of bytes needed - *6 for the GLTRIANGLES
        // WTF: num_tile_vertices??
        size_t tex_data_size (sizeof(GLfloat) * num_tiles * num_tile_vertices * num_tile_dimensions);
        size_t vert_data_size(sizeof(GLfloat) * num_tiles * num_tile_vertices * num_tile_dimensions);
        GLfloat* layer_tex_coords(nullptr);
        GLfloat* layer_vert_coords(nullptr);

        try {
            layer_tex_coords = new GLfloat[tex_data_size];
            layer_vert_coords = new GLfloat[vert_data_size];
        }
        catch(std::bad_alloc& ba) {
            LOG(ERROR) << "Out of memory in Map::generate_data";
            return;
        }

        // Build the layer data based on the
        if (layer_packing == Layer::Packing::DENSE) {
            generate_dense_layer_tex_coords(layer_tex_coords, layer);
            generate_dense_layer_vert_coords(layer_vert_coords, layer);
        }
        else {
            generate_sparse_layer_tex_coords(layer_tex_coords, layer);
            generate_sparse_layer_vert_coords(layer_vert_coords, layer);

            //Generate the mappings
            //ONLY NEEDED FOR SPARSE
            int x(0);
            int y(0);
            int idx(0);

            for(auto tile_data = layer_data->begin(); tile_data != layer_data->end(); ++tile_data) {
                std::shared_ptr<TileSet> tileset = tile_data->first;

                //Set the index into the buffer
                buffer_map->insert(std::make_pair(y*map_width + x, idx));

                //Calculate the next index
                //If we're not looking at a blank tile
                if(tileset) {
                    //Calculate the new offset
                    idx += num_tile_dimensions*num_tile_vertices;

                }

                //ELSE: Offset unchanged

                //Next tile
                x++;

                //if we're on a new layer
                if(x >= map_width) {
                    x = 0;
                    y++;
                }
            }
        }

        // Set this data in the renderable component for the layer
        RenderableComponent* renderable_component = layer->get_renderable_component();
        renderable_component->set_texture_coords_data(layer_tex_coords, tex_data_size, false);
        renderable_component->set_vertex_data(layer_vert_coords, vert_data_size, false);
        renderable_component->set_num_vertices_render(num_tiles*num_tile_vertices);

        layer_num++;
    }
}

void Map::generate_layer_tex_coords(GLfloat* data, std::shared_ptr<Layer> layer, bool dense) {
    auto layer_data = layer->get_layer_data();

    //Get all the tiles in the layer, moving from left to right and down
    int offset(0);
    const int num_floats(12);
    for (auto &tile_data : *layer_data) {
        std::shared_ptr<TileSet> tileset(tile_data.first);
        int tile_id(tile_data.second);

        //IF WE ARE GENERATING A SPARSE LAYER
        //Skip out blank tiles:
        //This get's us our sparse data structure
        if (!dense && !tileset) {
            continue;
        }

        // If this is a dense layer and the tile is blank, then we don't
        // actually care about the texture coordinates. Only create data
        // when we have a tileset.
        if (tileset) {
            //Get the texture coordinates for this tile
            // GLfloat *tileset_ptr = &tileset_tex_coords[(tile_id)*8]; //*8 as 8 coordinates per tile
            std::tuple<float,float,float,float> coords(tileset->get_atlas()->index_to_coords(tile_id));

            //bottom left
            data[offset+0]  = std::get<0>(coords);
            data[offset+1]  = std::get<2>(coords);

            //top left
            data[offset+2]  = std::get<0>(coords);
            data[offset+3]  = std::get<3>(coords);

            //bottom right
            data[offset+4]  = std::get<1>(coords);
            data[offset+5]  = std::get<2>(coords);

            //top left
            data[offset+6]  = std::get<0>(coords);
            data[offset+7]  = std::get<3>(coords);

            //top right
            data[offset+8]  = std::get<1>(coords);
            data[offset+9]  = std::get<3>(coords);

            //bottom right
            data[offset+10] = std::get<1>(coords);
            data[offset+11] = std::get<2>(coords);
        }

        offset += num_floats;
    }
}

void Map::generate_sparse_layer_tex_coords(GLfloat* data, std::shared_ptr<Layer> layer) {
    generate_layer_tex_coords(data, layer, false);
}


void Map::generate_dense_layer_tex_coords(GLfloat* data, std::shared_ptr<Layer> layer) {
    generate_layer_tex_coords(data, layer, true);
}

void Map::generate_layer_vert_coords(GLfloat* data, std::shared_ptr<Layer> layer, bool dense) {
    LOG(INFO) << "Generating map coordinate data";

    int num_floats(12);
    int offset(0);
    ///
    /// Vertex winding order:
    /// 1, 3   4
    ///  * --- *
    ///  |     |
    ///  |     |
    ///  * --- *
    /// 0       2,5
    ///

    // The current tile's data
    auto tile_data = layer->get_layer_data()->begin();

    // Generate one layer's worth of data
    for (int y = 0; y < map_height; y++) {
        for (int x = 0; x < map_width; x++) {
            // If we exhaust the layer's data
            if (tile_data == layer->get_layer_data()->end()) {
                LOG(ERROR) << "Layer had less data than map dimensions in Map::generate_layer_vert_coords";
                return;
            }

            std::shared_ptr<TileSet> tileset(tile_data->first);
            // int tile_id(tile_data->second);

            // IF GENERATING A SPARSE LAYER
            // Skip empty tiles
            if (!dense && !tileset) {
                ++tile_data;
                continue;
            }

            // Default to invisible.
            float vx1(-1.0f), vy1(-1.0f);
            float vx2(-1.0f), vy2(-1.0f);

            if (tileset) {
                // The tile is not blank, so set its x, y.
                vx1 = float(x);
                vy1 = float(y);
                vx2 = float(x + 1.001);
                vy2 = float(y + 1.001);
            } else if (dense) {
                LOG(INFO) << x << ", " << y;
            }

            //bottom left
            data[offset + 0] = vx1;
            data[offset + 1] = vy1;

            //top left
            data[offset + 2] = vx1;
            data[offset + 3] = vy2;

            //bottom right
            data[offset + 4] = vx2;
            data[offset + 5] = vy1;

            //top left
            data[offset + 6] = vx1;
            data[offset + 7] = vy2;

            //top right
            data[offset + 8] = vx2;
            data[offset + 9] = vy2;

            //bottom right
            data[offset + 10] = vx2;
            data[offset + 11] = vy1;

            offset += num_floats;
            ++tile_data;
        }
    }
}

void Map::generate_dense_layer_vert_coords(GLfloat* data, std::shared_ptr<Layer> layer) {
    generate_layer_vert_coords(data, layer, true);
}

void Map::generate_sparse_layer_vert_coords(GLfloat* data, std::shared_ptr<Layer> layer) {
    generate_layer_vert_coords(data, layer, false);
}

void Map::init_textures() {
    // texture_atlases[0] = TextureAtlas::get_shared("../resources/basictiles_2.png");

    // WTF is going on?
    // Set the texture data in the rederable component for each layer
    for (int layer_id : layer_ids) {
        std::shared_ptr<Layer> layer = ObjectManager::get_instance().get_object<Layer>(layer_id);
        // layer->get_renderable_component()->set_texture((*layer->get_layer_data())[0].first->get_atlas());
        layer->get_renderable_component()->set_texture(tilesets[0]->get_atlas());
    }
}

/**
 * This function initialises the shader, creating and loading them.
 */
bool Map::init_shaders() {
    std::shared_ptr<Shader> shader;
    try {
        shader = Shader::get_shared("tile_shader");
    }
    catch (std::exception e) {
        LOG(ERROR) << "Failed to create the shader";
        return false;
    }

    //Set the shader for each layer
    for (int layer_id : layer_ids) {
        std::shared_ptr<Layer> layer = ObjectManager::get_instance().get_object<Layer>(layer_id);
        layer->get_renderable_component()->set_shader(shader);
    }

    return true;
}

Map::Blocker::Blocker(glm::ivec2 tile, std::vector <std::vector<int>>* blocker):
    tile(tile), blocker(blocker) {
        (*blocker)[tile.x][tile.y]++;

        VLOG(2) << "Block level at tile " << tile.x << " " <<tile.y
                << " increased from " << (*blocker)[tile.x][tile.y] - 1
                << " to " << (*blocker)[tile.x][tile.y] << ".";
}

Map::Blocker::Blocker(const Map::Blocker &other):
    tile(other.tile), blocker(other.blocker) {
        (*blocker)[tile.x][tile.y]++;

        VLOG(2) << "Block level at tile " << tile.x << " " <<tile.y
                << " increased from " << (*blocker)[tile.x][tile.y] - 1
                << " to " << (*blocker)[tile.x][tile.y] << ".";
}

Map::Blocker::~Blocker() {
    VLOG(2) << "Unblocking tile at " << tile.x << ", " << tile.y << ".";

    blocker->at(tile.x).at(tile.y) -= 1;

    VLOG(2) << "Block level at tile " << tile.x << " " <<tile.y
            << " decreased from " << (*blocker)[tile.x][tile.y] + 1
            << " to " << (*blocker)[tile.x][tile.y] << ".";
}

Map::Blocker Map::block_tile(glm::ivec2 tile) {
    return Blocker(tile, &blocker);
}

bool Map::recalculate_layer_mappings(int x_pos, int y_pos, int layer_num) {
    if (y_pos < 0 || x_pos < 0) {
        return false;
    }

    auto layer(layer_mappings[layer_num]);
    int index(y_pos * map_width + x_pos);
    int size(int(layer->size()));

    // Handle last element and those outside of range
    // Note: index ≥ 0 by y_pos, map_width, x_pos > 0
    if (index + 1 >= size) {
        return false;
    }

    // Do we need to increment mappings?
    // Compare offsets
    if ((*layer)[index + 1] == (*layer)[index]) {
        // The next element is the same so we need to increment
        // further offsets.

        // Shift all of the offsets down as we're putting a tile into
        // this position
        for (auto i = index + 1; i < size; i++) {
            (*layer)[i] += num_tile_dimensions * num_tile_vertices;;
        }

        return false;
    }
    else {
        // There is already a tile here, so don't mutate

        // You can directly insert the tile
        return true;
    }
}

void Map::update_tile(int x_pos, int y_pos, const std::string layer_name, const std::string tile_name) {
    int tile_id = -1;
    std::shared_ptr<TileSet> tileset;
    for (std::shared_ptr<TileSet> tileset_i : tilesets) {
        try {
            tile_id = tileset_i->get_atlas()->get_name_index(tile_name);
            tileset = tileset_i;
            break;
        } catch (std::exception) {
            continue;
        }
    }
    if (tile_id == -1) {
        // Tile not found.
        throw std::runtime_error("Tile not found: " + tile_name);
    }

    // Build the data for the update
    GLfloat *data;
    size_t data_size(sizeof(GLfloat) * num_tile_dimensions * num_tile_vertices);

    // Get a buffer for the data
    try {
        data = new GLfloat[data_size];
    }
    catch(std::bad_alloc& ba) {
        LOG(ERROR) << "Couldn't allocate memory in Map::update_tile";
        return;
    }

    // Get the texture coordinates for this tile
    // GLfloat *tileset_ptr = &tileset_tex_coords[(tile_id)*8]; //*8 as 8 coordinates per tile
    std::tuple<float,float,float,float> coords(tileset->get_atlas()->index_to_coords(tile_id));

    //bottom left
    data[0]  = std::get<0>(coords);
    data[1]  = std::get<2>(coords);

    //top left
    data[2]  = std::get<0>(coords);
    data[3]  = std::get<3>(coords);

    //bottom right
    data[4]  = std::get<1>(coords);
    data[5]  = std::get<2>(coords);

    //top left
    data[6]  = std::get<0>(coords);
    data[7]  = std::get<3>(coords);

    //top right
    data[8]  = std::get<1>(coords);
    data[9]  = std::get<3>(coords);

    //bottom right
    data[10] = std::get<1>(coords);
    data[11] = std::get<2>(coords);


    // Find the layer from the layer name name.
    std::shared_ptr<Layer> layer;
    int layer_num;
    for (unsigned int i = 0; i < layer_ids.size(); i++) {
        std::shared_ptr<Layer> layer_test(ObjectManager::get_instance().get_object<Layer>(layer_ids[i]));
        if (layer_test->get_name() == layer_name) {
            layer = layer_test;
            layer_num = i;
            break;
        }
    }

    if (!layer) {
        throw std::runtime_error("Layer not found: " + layer_name);
    }

    // Put it into the buffers

    Layer::Packing packing(layer->get_packing());

    // Add this tile to the layer data structure
    layer->update_tile(x_pos, y_pos, tile_id, tileset);

    int tile_offset;

    // Perform O(1) update. no need to do mapping changes
    if (packing == Layer::Packing::DENSE) {
        tile_offset = y_pos * map_width + x_pos;

        // Update the buffer
        // Just update that tile directly
        RenderableComponent *renderable_component(layer->get_renderable_component());
        renderable_component->update_texture_buffer(
            sizeof(GLfloat) * tile_offset * num_tile_vertices * num_tile_dimensions,
            data_size,
            data
        );
    }
    else {
        // TODO: create a small buffer to hold these updates rather than rebuilding the entire buffer

        // Generate the new tile's data
        GLfloat* vertex_data = nullptr;

        // Get a buffer for the data
        try {
            vertex_data = new GLfloat[data_size];
        }
        catch(std::bad_alloc& ba) {
            LOG(ERROR) << "Couldn't allocate memory in Map::update_tile";
            data = nullptr;
            return;
        }

        // Set the needed data

        // bottom left
        vertex_data[0]  = float(x_pos);
        vertex_data[1]  = float(y_pos);

        // top left
        vertex_data[2]  = float(x_pos);
        vertex_data[3]  = float(y_pos + 1);

        // bottom right
        vertex_data[4]  = float(x_pos + 1);
        vertex_data[5]  = float(y_pos);

        // top left
        vertex_data[6]  = float(x_pos);
        vertex_data[7]  = float(y_pos + 1);

        // top right
        vertex_data[8]  = float(x_pos + 1);
        vertex_data[9]  = float(y_pos + 1);

        // bottom right
        vertex_data[10] = float(x_pos + 1);
        vertex_data[11] = float(y_pos);

        // Fetch the offset from the data buffer
        // Tile offset in floats
        int offset((*layer_mappings[layer_num])[y_pos*map_width + x_pos]);

        // Recalculate the layer mappings
        bool overwrite(recalculate_layer_mappings(x_pos, y_pos, layer_num));

        // TODO: small buffer
        // Get the data
        RenderableComponent* layer_renderable_component(layer->get_renderable_component());

        GLfloat* layer_texture_data(layer_renderable_component->get_texture_coords_data());
        GLfloat* layer_vertex_data (layer_renderable_component->get_vertex_data());

        size_t texture_data_size(layer_renderable_component->get_texture_coords_data_size());
        size_t vertex_data_size (layer_renderable_component->get_vertex_data_size());

        if (overwrite) {
            // We just need to overwrite the tile
            // Update the buffer
            // Just update that tile directly
            layer_renderable_component->update_texture_buffer(offset*sizeof(GLfloat), data_size, data);
        }
        else {
            // We need to insert the tile: expand the buffers

            //Generate the new buffers
            GLfloat *new_texture_data;
            GLfloat *new_vertex_data;
            int new_texture_data_size(int(texture_data_size + data_size));
            int new_vertex_data_size (int( vertex_data_size + data_size));

            try {
                new_texture_data = new GLfloat[new_texture_data_size];
                new_vertex_data  = new GLfloat[new_vertex_data_size];
            }
            catch(std::bad_alloc& ba) {
                LOG(ERROR) << "Couldn't allocate memory for new texture and vertex buffers in Map::update_tile";
                return;
            }

            //Copy the first part of the original data
            int data_length(num_tile_vertices * num_tile_dimensions);
            std::copy(layer_vertex_data, &layer_vertex_data[offset], new_vertex_data);
            std::copy(layer_texture_data, &layer_texture_data[offset], new_texture_data);

            //Insert the new data into the correct position
            std::copy(vertex_data, &vertex_data[data_length], &new_vertex_data[offset]);
            std::copy(data, &data[data_length], &new_texture_data[offset]);

            //Copy the rest of the original data
            std::copy(&layer_vertex_data[offset], &layer_vertex_data[vertex_data_size], &new_vertex_data[offset + data_length]);
            std::copy(&layer_texture_data[offset], &layer_texture_data[texture_data_size], &new_texture_data[offset + data_length]);

            //Set the new data
            layer_renderable_component->set_vertex_data(new_vertex_data, new_vertex_data_size, false);
            layer_renderable_component->set_num_vertices_render(new_texture_data_size / (int(sizeof(GLfloat)) * num_tile_dimensions));
            layer_renderable_component->set_texture_coords_data(new_texture_data, new_texture_data_size, false);
        }
        delete[] vertex_data;
    }
    delete[] data;
}

std::string Map::query_tile(int x_pos, int y_pos, const std::string layer_name) {
    std::shared_ptr<Layer> layer;
    for (unsigned int i = 0; i < layer_ids.size(); i++) {
        std::shared_ptr<Layer> layer_test(ObjectManager::get_instance().get_object<Layer>(layer_ids[i]));
        if (layer_test->get_name() == layer_name) {
            layer = layer_test;
        }
    }

    if (!layer) {
        throw std::runtime_error("Layer not found: " + layer_name);
    }

    std::pair<std::shared_ptr<TileSet>, int> tile = layer->get_tile(x_pos, y_pos);

    return tile.first ? tile.first->get_atlas()->get_index_name(tile.second) : "";
}

int Map::get_tile_texture_vbo_offset(int layer_num, int x_pos, int y_pos) {
     // Fetch the offset from the data buffer
     int tile_offset((*layer_mappings[layer_num])[y_pos*map_width + x_pos]);

     // The VBO offset is the tile offset times the dimenions and number of vertices
     return tile_offset * num_tile_vertices * num_tile_dimensions;
}
