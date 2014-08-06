#include <exception>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <memory>
#include <string>

//Include GLM
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif

#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif


#include "api.hpp"
#include "engine_api.hpp"
#include "layer.hpp"
#include "map.hpp"
#include "map_loader.hpp"
#include "object.hpp"
#include "object_manager.hpp"
#include "sprite.hpp"
#include "walkability.hpp"

Map::Map(const std::string map_src):
    event_step_on(Vec2D(0, 0)),
    event_step_off(Vec2D(0, 0))
    {
        //Load the map
        MapLoader map_loader;
        bool result = map_loader.load_map(map_src);
        if(!result)  {

            LOG(ERROR) << "Couldn't load map";
            return;
        }

        //Get the loaded map data
        map_width = map_loader.get_map_width();
        map_height = map_loader.get_map_height();

        // hack to construct postion dispatcher as we need map diametions
        event_step_on = PositionDispatcher<int>(Vec2D(map_width,map_height));
        event_step_off = PositionDispatcher<int>(Vec2D(map_width,map_height));

        LOG(INFO) << "Map loading: ";
        LOG(INFO) << "Map width: " << map_width << " Map height: " << map_height;

        layers = map_loader.get_layers();
        tilesets = map_loader.get_tilesets();
        //Get map objects
        for(auto map_object : map_loader.get_objects()) {
            //Add the object to the object manager and the map
            int object_id = map_object->get_id();
            ObjectManager::get_instance().add_object(map_object);
            map_object_ids.push_back(object_id);
        }
        //Get the tilesets
        //TODO: We'll only support one tileset at the moment
        //Get an object list
        blocker = std::vector<std::vector<int>>(map_width, std::vector<int>(map_height, 0));


        //Generate the geometry needed for this map
        init_shaders();
        init_textures();
        generate_tileset_coords(&texture_images[0]);
        generate_data();

}

Map::~Map() {
    // texture_images stored within struct, implicit destruction.
    // release buffers
    delete[] map_data;
    delete[] map_tex_coords;
    delete[] tileset_tex_coords;

    LOG(INFO) << "Map destructed";
}

bool Map::is_walkable(int x_pos, int y_pos) {
    //Default is walkable
    bool walkable = true;
    //return true;
    //Iterate through all sprites
    for(auto sprite : sprite_ids) {
        //If its an invalid object
        if (sprite == 0)
            continue;

        std::shared_ptr<Sprite> object = ObjectManager::get_instance().get_object<Sprite>(sprite);

        //If we cannot walk on this object
        if(object) {
            if(object->get_walkability() == Walkability::BLOCKED) {
                walkable = false;

                //We can stop checking further objects and tiles
                return walkable;
            }
        }
    }

    //Iterate through all map objects
    for(auto map_object : map_object_ids) {
        //If its an invalid object
        if (map_object == 0)
            continue;

        std::shared_ptr<MapObject> object = ObjectManager::get_instance().get_object<MapObject>(map_object);

        //If we cannot walk on this object
        if(object) {
            if(object->get_walkability() == Walkability::BLOCKED) {
                walkable = false;

                //We can stop checking further objects and tiles
                return walkable;
            }
        }
    }

    //Iterate through all tiles
    for(auto layer : layers ) {

        //determine if we can walk on the map
        if(layer->get_name() == "Collisions") {

            //if there is a tile, treat it as blocked
            if(layer->get_tile(x_pos, y_pos) != 0) {
                 walkable = false;
                 //We can stop checking further objects and tiles
                 return walkable;
            }
        }
    }

    return walkable;
}

void Map::add_sprite(int sprite_id) {
    if(ObjectManager::is_valid_object_id(sprite_id))
        sprite_ids.push_back(sprite_id);
}

void Map::remove_sprite(int sprite_id) {
    if(ObjectManager::is_valid_object_id(sprite_id)){
        for(auto it = sprite_ids.begin(); it != sprite_ids.end(); ++it) {
            //If a valid object
            if(*it != 0) {
                //remove it if its the sprite
                if(*it == sprite_id) {
                    sprite_ids.erase(it);
                    return;
                }
            }
        }
    }
}

/**
 * The function used to generate the cache of tile texture coordinates.
 */
void Map::generate_tileset_coords(Image* texture_image) {
    LOG(INFO) << "Generating tileset texture coords";

    if(Engine::get_tile_size() == 0) {
        LOG(ERROR) << "Tile size is 0 in Map::generate_tileset_coords";
        return;
    }

    int num_tiles_x = texture_image->width / Engine::get_tile_size();
    int num_tiles_y = texture_image->height / Engine::get_tile_size();
    LOG(INFO) << "Tileset size: " << num_tiles_x << " " << num_tiles_y;

    //Each tile needs 8 floats to describe its position in the image
    try {
        tileset_tex_coords = new GLfloat[sizeof(GLfloat)* num_tiles_x * num_tiles_y * 4 * 2];
    } 
    catch (std::bad_alloc& ba) {
        LOG(ERROR) << "Out of Memory in Map::generate_tileset_coords";
        return;
    }

    //Check tileset dimensions
    if(texture_image->store_width == 0 || texture_image->store_height == 0) {
        LOG(ERROR) << "Texture image has 0 in at least one dimension in Map::generate_tileset_coords";
        return;
    }

    //Tiles are indexed from top left but Openl uses texture coordinates from bottom left
    //so we remap these
    GLfloat tileset_inc_x = GLfloat(texture_image->width) / GLfloat(texture_image->store_width) / static_cast<GLfloat>(num_tiles_x);
    GLfloat tileset_inc_y = GLfloat(texture_image->height) / GLfloat(texture_image->store_height) / static_cast<GLfloat>(num_tiles_y);
    //We work from left to right, moving down the gl texture
    GLfloat tileset_offset_x = 0.0;
    GLfloat tileset_offset_y = GLfloat(num_tiles_y - 1) * tileset_inc_y;

    //generate the coordinates for each tile
    for (int y = 0; y < num_tiles_y; y++) {
        for (int x = 0; x < num_tiles_x; x++) {
            //bottom left
            tileset_tex_coords[y* num_tiles_x*4*2+x*4*2+0] = tileset_offset_x;
            tileset_tex_coords[y* num_tiles_x*4*2+x*4*2+1] = tileset_offset_y;

            //top left
            tileset_tex_coords[y* num_tiles_x*4*2+x*4*2+2] = tileset_offset_x;
            tileset_tex_coords[y* num_tiles_x*4*2+x*4*2+3] = tileset_offset_y + tileset_inc_y;

            //bottom right
            tileset_tex_coords[y* num_tiles_x*4*2+x*4*2+4] = tileset_offset_x + tileset_inc_x;
            tileset_tex_coords[y* num_tiles_x*4*2+x*4*2+5] = tileset_offset_y;

            //top right
            tileset_tex_coords[y* num_tiles_x*4*2+x*4*2+6] = tileset_offset_x + tileset_inc_x;
            tileset_tex_coords[y* num_tiles_x*4*2+x*4*2+7] = tileset_offset_y + tileset_inc_y;

            tileset_offset_x += tileset_inc_x;
        }
        tileset_offset_x = 0.0;
        tileset_offset_y -= tileset_inc_y;
    }
}

void Map::generate_data() {
    LOG(INFO) << "Generating map data";

    //Get each layer of the map
    //Start at layer 0
    int layer_num = 0;
    for(auto layer_iter = layers.begin(); layer_iter != layers.end(); ++layer_iter) {
        auto layer_data = (*layer_iter)->get_layer_data();


        //Build the mapping from (x, y) to offsets in the vertex and texture buffers
        std::shared_ptr<std::map<int, int>> buffer_map = std::make_shared<std::map<int, int>>();        
        layer_mappings[layer_num] = buffer_map;


        //Don't generate data for the collisions layer
        //TODO: handle this not being in layer_mappings
        if ((*layer_iter)->get_name() == "Collisions") {
            layer_num++;
            continue;
        }


        //Work out if we need a dense or a sparse buffer
        int total_tiles = 0;
        int num_blank_tiles = 0;
        //Get all the tiles in the layer, moving from left to right and down
        for(auto tile_data = layer_data->begin(); tile_data != layer_data->end(); ++tile_data) {
            int tile_id = tile_data->second;
            if(tile_id == 0) {
                num_blank_tiles++;
            }
            total_tiles++;
        }

        //Spare packing by default
        Layer::Packing layer_packing = Layer::Packing::SPARSE;
        
        //If more than 50% of layer has tiles, make it dense
        if(num_blank_tiles < total_tiles/2 ) {
            layer_packing = Layer::Packing::DENSE;
            std::cout <<" DENSE "<< layer_num << std::endl;
        }

        (*layer_iter)->set_packing(layer_packing);

        //Create the buffer for the layer
        int num_tiles = total_tiles - num_blank_tiles;
        //The number of bytes needed - *6 for the GLTRIANGLES
        int tex_data_size = sizeof(GLfloat)*num_tiles*num_tile_vertices*num_tile_dimensions;
        int vert_data_size = sizeof(GLfloat)*num_tiles*num_tile_vertices*num_tile_dimensions;
        GLfloat* layer_tex_coords = nullptr;
        GLfloat* layer_vert_coords = nullptr;

        try {
            layer_tex_coords = new GLfloat[tex_data_size];
            layer_vert_coords = new GLfloat[vert_data_size];
        }
        catch(std::bad_alloc& ba) {
            LOG(ERROR) << "Out of memory in Map::generate_data";
            return;
        }

        //Build the layer data based on the 
        if(layer_packing == Layer::Packing::DENSE) {
            generate_dense_layer_tex_coords(layer_tex_coords, tex_data_size, num_tiles, *(layer_iter));
            generate_dense_layer_vert_coords(layer_vert_coords, vert_data_size, num_tiles,*(layer_iter));
        } 
        else {
            generate_sparse_layer_tex_coords(layer_tex_coords, tex_data_size, num_tiles, (*layer_iter));
            generate_sparse_layer_vert_coords(layer_vert_coords, vert_data_size, num_tiles, (*layer_iter));
        }
        //Set this data in the renderable component for the layer
        RenderableComponent* renderable_component = (*layer_iter)->get_renderable_component();
        renderable_component->set_texture_coords_data(layer_tex_coords, tex_data_size, false);
        renderable_component->set_vertex_data(layer_vert_coords, vert_data_size, false);
        renderable_component->set_num_vertices_render(num_tiles*num_tile_vertices);

        //Generate the mappings
        int x = 0;
        int y = 0;
        int idx = 0;
        for(auto tile_data = layer_data->begin(); tile_data != layer_data->end(); ++tile_data) {
            int tile_id = tile_data->second;            

            //Set the index into the buffer
            (*buffer_map)[y*map_width+ x] = idx;

            //Calculate the next index
            //If we're not looking at a blank tile
            if(tile_id != 0) {
                //Calculate the new offset
                idx += num_tile_dimensions*num_tile_vertices;
            }
            //ELSE: Offset unchanged
        }
        


        layer_num++;
    }
}

void Map::generate_layer_tex_coords(GLfloat* data, int data_size, int num_tiles, std::shared_ptr<Layer> layer, bool dense) {
    auto layer_data = layer->get_layer_data();

    //Get all the tiles in the layer, moving from left to right and down 
    int offset = 0;
    int num_floats = 12;
    for(auto tile_data = layer_data->begin(); tile_data != layer_data->end(); ++tile_data) {
        std::string tileset_name = tile_data->first;
        int tile_id = tile_data->second;

        //IF WE ARE GENERATING A SPARSE LAYER
        //Skip out blank tiles:
        //This get's us our sparse data structure
        if(dense == false && tile_id == 0)
            continue;

        //Get the texture coordinates for this tile
        GLfloat *tileset_ptr = &tileset_tex_coords[(tile_id)*8]; //*8 as 8 coordinates per tile

        //bottom left
        data[offset+0] = tileset_ptr[0];
        data[offset+1] = tileset_ptr[1];

        //top left
        data[offset+2] = tileset_ptr[2];
        data[offset+3] = tileset_ptr[3];

        //bottom right
        data[offset+4] = tileset_ptr[4];
        data[offset+5] = tileset_ptr[5];

        //top left
        data[offset+6] = tileset_ptr[2];
        data[offset+7] = tileset_ptr[3];

        //top right
        data[offset+8] = tileset_ptr[6];
        data[offset+9] = tileset_ptr[7];
            
        //bottom right
        data[offset+10] = tileset_ptr[4];
        data[offset+11] = tileset_ptr[5];

        offset += num_floats;
    }
}

void Map::generate_sparse_layer_tex_coords(GLfloat* data, int data_size, int num_tiles, std::shared_ptr<Layer> layer) {
    generate_layer_tex_coords(data, data_size, num_tiles, layer, false);
}


void Map::generate_dense_layer_tex_coords(GLfloat* data, int data_size, int num_tiles, std::shared_ptr<Layer> layer) {
    generate_layer_tex_coords(data, data_size, num_tiles, layer, true);
}

void Map::generate_layer_vert_coords(GLfloat* data, int data_size, int num_tiles, std::shared_ptr<Layer> layer, bool dense) {
    LOG(INFO) << "Generating map coordinate data";

    float scale = (float)(Engine::get_tile_size() * Engine::get_global_scale());
    int num_floats = 12;
    int offset = 0;
    ///
    /// Vertex winding order:
    /// 1, 3   4
    ///  * --- *
    ///  |     |
    ///  |     |
    ///  * --- *
    /// 0       2,5
    ///

    //The current tile's data
    auto tile_data = layer->get_layer_data()->begin();
    std::cout << "LAYER " << std::endl;
    //Generate one layer's worth of data
    for(int y = 0; y < map_height; y++) {
        for(int x = 0; x < map_width; x++) {
            //If we exhaust the layer's data
            if(tile_data == layer->get_layer_data()->end()) {
                LOG(ERROR) << "Layer had less data than map dimensions in Map::generate_layer_vert_coords";
                return;
            }

            //IF GENERATING A SPARSE LAYER
            //Skip empty tiles
            int tile_id = tile_data->second;
            if(dense == false && tile_id == 0) {
                ++tile_data;
                continue;
            }
            //bottom left
            data[offset+ 0] = scale * float(x);
            data[offset+ 1] = scale * float(y);

            //top left
            data[offset+ 2] = scale * float(x);
            data[offset+ 3] = scale * float(y + 1);

            //bottom right
            data[offset+ 4] = scale * float(x + 1);
            data[offset+ 5] = scale * float(y);
            
            //top left
            data[offset+ 6]  = scale * float(x);
            data[offset+ 7] = scale * float(y+1);

            //top right
            data[offset+ 8] = scale * float(x+1);
            data[offset+ 9] = scale * float(y+1);

            //bottom right
            data[offset+10] = scale * float(x+1);
            data[offset+11] = scale * float(y);

            offset += num_floats;
            ++tile_data;
        }
    }
}

void Map::generate_dense_layer_vert_coords(GLfloat* data, int data_size, int num_tiles, std::shared_ptr<Layer> layer) {
    generate_layer_vert_coords(data, data_size, num_tiles, layer, true);
}

void Map::generate_sparse_layer_vert_coords(GLfloat* data, int data_size, int num_tiles, std::shared_ptr<Layer> layer) {
    generate_layer_vert_coords(data, data_size, num_tiles, layer, false);
}

void Map::init_textures() {
    texture_images[0] = Image("../resources/basictiles_2.png");

    //Set the texture data in the rederable component for each layer
    for(auto layer: layers) {
        layer->get_renderable_component()->set_texture_image(&texture_images[0]);
    }
}

/**
 * This function initialises the shader, creating and loading them.
 */
bool Map::init_shaders() {
    Shader* shader = nullptr;
    try {
#ifdef USE_GLES
        shader = new Shader("vert_shader.glesv", "frag_shader.glesf");
#endif
#ifdef USE_GL
        shader = new Shader("vert_shader.glv", "frag_shader.glf");
#endif
    }
    catch (std::exception e) {
        delete shader;
        shader = nullptr;
        LOG(ERROR) << "Failed to create the shader";
        return false;
    }

    //Set the shader for each layer
    for(auto layer: layers) {
        layer->get_renderable_component()->set_shader(shader);
    }
    return true;

}

Map::Blocker::Blocker(Vec2D tile, std::vector <std::vector<int>>* blocker):
    tile(tile), blocker(blocker) {
        (*blocker)[tile.x][tile.y]++;

        LOG(INFO) << "Block level at tile " << tile.x << " " <<tile.y
          << " increased from " << (*blocker)[tile.x][tile.y] - 1
          << " to " << (*blocker)[tile.x][tile.y] << ".";
}

Map::Blocker::Blocker(const Map::Blocker &other):
    tile(other.tile), blocker(other.blocker) {
        (*blocker)[tile.x][tile.y]++;

        LOG(INFO) << "Block level at tile " << tile.x << " " <<tile.y
          << " increased from " << (*blocker)[tile.x][tile.y] - 1
          << " to " << (*blocker)[tile.x][tile.y] << ".";
}

Map::Blocker::~Blocker() {
    LOG(INFO) << "Unblocking tile at " << tile.x << ", " << tile.y << ".";

    blocker->at(tile.x).at(tile.y) = blocker->at(tile.x).at(tile.y) - 1;

    LOG(INFO) << "Block level at tile " << tile.x << " " <<tile.y
      << " decreased from " << (*blocker)[tile.x][tile.y] + 1
      << " to " << (*blocker)[tile.x][tile.y] << ".";
}

Map::Blocker Map::block_tile(Vec2D tile) {
    return Blocker(tile, &blocker);
}

void Map::recalculate_layer_mappings(int x_pos, int y_pos, int layer_num) {
    //The index into the map
    int index = y_pos*map_width + x_pos;
    std::shared_ptr<std::map<int ,int>> layer = layer_mappings[layer_num];
    int size = layer->size();
    //The current tile offset
    int tile_offset = (*layer)[index];
    //The offsets are the buffer indices. Now, if the next element in
    // the layer has the same offset, then this means that this sparse
    // index has not been given a value so far. Thus, we need to put
    // the data here and then increment all the offsets after the
    // current offset. This gap of one indicates a used position.
    //
    // If this is not the case, then the offset is unique which means
    // the tile has already been placed so just update it
    //
    if(index >= size || index < 0)
        return;

    //Handle last element
    if(index + 1 == size) {
        return;
    }

    //Do we need to increment mappings?
    //Compare offsets
    if((*layer)[index+ 1] == (*layer)[index]) {
        //if the next element is the same, then we need to increment
        //further offsets.

        //Shift all of the offsets down as we're putting a tile into 
        //this position
        for(int i = index +1; i < size; i++)
            (*layer)[i]++;

    }
    //ELSE. No, there is already a tile here
}

void Map::update_tile(int x_pos, int y_pos, int layer_num, int tile_id, std::string tileset_name) {
    //BUILD THE DATA
    //Data for the update
    GLfloat* data = nullptr;
    size_t data_size = (size_t)(sizeof(GLfloat)*num_tile_dimensions*num_tile_vertices);

    //Get a buffer for the data
    try {
        data = new GLfloat[data_size];
    }
    catch(std::bad_alloc& ba) {
        LOG(ERROR) << "Couldn't allocate memory in Map::update_tile";
        data = nullptr;
        return;
    }
    //Set the needed data

    //Vertex

    //Get the texture coordinates for this tile
    GLfloat *tileset_ptr = &tileset_tex_coords[(tile_id)*8]; //*8 as 8 coordinates per tile

    //bottom left
    data[0] = tileset_ptr[0];
    data[1] = tileset_ptr[1];

    //top left
    data[2] = tileset_ptr[2];
    data[3] = tileset_ptr[3];

    //bottom right
    data[4] = tileset_ptr[4];
    data[5] = tileset_ptr[5];

    //top left
    data[6] = tileset_ptr[2];
    data[7] = tileset_ptr[3];

    //top right
    data[8] = tileset_ptr[6];
    data[9] = tileset_ptr[7];
            
    //bottom right
    data[10] = tileset_ptr[4];
    data[11] = tileset_ptr[5];

    //PUT IT INTO THE BUFFERS

    //Determine if the layer is dense or sparse
    std::shared_ptr<Layer> layer = layers[layer_num];
    Layer::Packing packing = layer->get_packing();

    //Add this tile to the layer data structure
    layer->update_tile(x_pos, y_pos, tile_id, "");

    int tile_offset = 0;

    //Perform O(1) update. no need to do mapping changes
    if(packing == Layer::Packing::DENSE) {

        tile_offset = y_pos*map_width + x_pos;

        //Update the buffer
        //Just update that tile directly
        RenderableComponent* layer_renderable_component = layer->get_renderable_component();
        layer_renderable_component->update_texture_buffer(tile_offset*num_tile_vertices*num_tile_dimensions*sizeof(GLfloat), data_size, data);

    }
    else {
        //TODO: create a small buffer to hold these updates rather than rebuilding the entire buffer

        //Generate the new tile's data
        GLfloat* vertex_data = nullptr;
        float scale = (float)(Engine::get_tile_size() * Engine::get_global_scale());

        //Get a buffer for the data
        try {
            vertex_data = new GLfloat[data_size];
        }
        catch(std::bad_alloc& ba) {
            LOG(ERROR) << "Couldn't allocate memory in Map::update_tile";
            data = nullptr;
            return;
        }

        //Set the needed data

        //bottom left
        vertex_data[0] = scale * float(x_pos);
        vertex_data[1] = scale * float(y_pos);

        //top left
        vertex_data[2] = scale * float(x_pos);
        vertex_data[3] = scale * float(y_pos + 1);

        //bottom right
        vertex_data[4] = scale * float(x_pos + 1);
        vertex_data[5] = scale * float(y_pos);
            
        //top left
        vertex_data[6]  = scale * float(x_pos);
        vertex_data[7] = scale * float(y_pos+1);
        
        //top right
        vertex_data[8] = scale * float(x_pos+1);
        vertex_data[9] = scale * float(y_pos+1);

        //bottom right
        vertex_data[10] = scale * float(x_pos+1);
        vertex_data[11] = scale * float(y_pos);

        //Fetch the offset from the data buffer
        tile_offset = (*layer_mappings[layer_num])[y_pos*map_width + x_pos];

        //Recalculate the layer mappings
        recalculate_layer_mappings(x_pos, y_pos, layer_num);
        //The offset into the buffers
        //The mappins have been updated by the previous function call
        int offset = tile_offset*num_tile_vertices*num_tile_dimensions;

        //TODO: small buffer
        //Get the data 
        RenderableComponent* layer_renderable_component = layer->get_renderable_component();
        GLfloat* layer_texture_data = layer_renderable_component->get_texture_coords_data();
        GLfloat* layer_vertex_data = layer_renderable_component->get_vertex_data();
        size_t texture_data_size =  layer_renderable_component->get_texture_coords_data_size();
        size_t vertex_data_size = layer_renderable_component->get_vertex_data_size();

        //Generate the new buffers
        GLfloat* new_texture_data = nullptr;
        GLfloat* new_vertex_data = nullptr;
        size_t new_texture_data_size = texture_data_size + data_size;
        size_t new_vertex_data_size = vertex_data_size + data_size;

        try {
            new_texture_data = new GLfloat[new_texture_data_size];
            new_vertex_data = new GLfloat[new_vertex_data_size];
        }
        catch(std::bad_alloc& ba) {
            LOG(ERROR) << "Couldn't allocate memory for new texture and vertex buffers in Map::update_tile";
            new_texture_data  = nullptr;
            new_vertex_data = nullptr;
            return;
        }

        //Copy the first part of the original data
        int data_length = num_tile_vertices*num_tile_dimensions;
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
        layer_renderable_component->set_texture_coords_data(new_texture_data, new_texture_data_size, false);
    }
}
int Map::get_tile_texture_vbo_offset(int layer_num, int x_pos, int y_pos) {
     //Fetch the offset from the data buffer
     int tile_offset = (*layer_mappings[layer_num])[y_pos*map_width + x_pos];

     //The VBO offset is the tile offset times the dimenions and number of vertices
     return tile_offset*num_tile_vertices*num_tile_dimensions;
}

Vec2D Map::pixel_to_tile(Vec2D pixel_location) {
    float scale(Engine::get_tile_size()*Engine::get_global_scale());
    // convert pixel location to absolute instead of relative to current window

    Vec2D map_pixel(pixel_location + Vec2D(int(Map::get_display_x() * scale),
                                           int(Map::get_display_y() * scale)));

    // convert from pixel to map ints
    return Vec2D(int(float(map_pixel.x) / scale),
                 int(float(map_pixel.y) / scale));
}

Vec2D Map::tile_to_pixel(Vec2D tile_location) {
    float scale(Engine::get_tile_size()*Engine::get_global_scale());

    Vec2D results(int(float(tile_location.x) * scale),
                  int(float(tile_location.y) * scale));

    results -= Vec2D(int(Map::get_display_x() * scale),
                     int(Map::get_display_y() * scale));
    return results;
}

Vec2D Map::tile_to_pixel(std::pair<double,double> tile_location) {
    double scale(Engine::get_tile_size()*Engine::get_global_scale());

    std::pair<double,double> results(tile_location.first * scale, tile_location.second * scale);
    results.first = results.first - (Map::get_display_x() * scale) ;
    results.second = results.second - (Map::get_display_y() * scale) ;
    return Vec2D( (int)results.first, (int)results.second);
}    


/*
void Map::merge_layer_data() {


}
*/
