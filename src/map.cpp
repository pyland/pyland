#include <exception>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
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
#include "walkability.hpp"

Map::Map(const std::string map_src):
    renderable_component(),
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
        objects  = map_loader.get_objects();

        //Get the tilesets
        //TODO: We'll only support one tileset at the moment
        //Get an object list
        blocker = std::vector<std::vector<int>>(map_width, std::vector<int>(map_height, 0));


        //Generate the geometry needed for this map
        init_shaders();
        init_textures();
        generate_tileset_coords(&texture_images[0]);
        generate_map_texcoords();
        generate_map_coords();

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
    //Iterate through all objects
    for(auto character : characters) {
        //If its an invalid object
        if (character == 0)
            continue;

        std::shared_ptr<Object> object = ObjectManager::get_instance().get_object<Object>(character);

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

void Map::add_character(int character_id) {
    if(ObjectManager::is_valid_object_id(character_id))
        characters.push_back(character_id);
}

void Map::remove_character(int character_id) {
    if(ObjectManager::is_valid_object_id(character_id)){
        for(auto it = characters.begin(); it != characters.end(); ++it) {
            //If a valid object
            if(*it != 0) {
                //remove it if its the character
                if(*it == character_id) {
                    characters.erase(it);       
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

    //Tiles are indexed from top left but Openl uses texture coordinates from bottom left
    //so we remap these 
    GLfloat tileset_inc_x = GLfloat(texture_image->width) / GLfloat(texture_image->store_width) / static_cast<GLfloat>(num_tiles_x);
    GLfloat tileset_inc_y = GLfloat(texture_image->height) / GLfloat(texture_image->store_height) / static_cast<GLfloat>(num_tiles_y);
    //We work from left to right, moving down the gl texture
    GLfloat tileset_offset_x = 0.0;
    GLfloat tileset_offset_y = GLfloat(num_tiles_y - 1) * tileset_inc_y;
    //TODo: DIV ZEro HERRE 

    //TODO: REMEMBER TILESET COORDINATES ARE INVERSE OF IMAGE FILE ONES
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
            //      for(int i =0 ; i < 8; i++)
            //        cout << " " <<        tileset_tex_coords[y* num_tiles_x*4*2+x*4*2+i];
            //      cout << endl;
        }
        tileset_offset_x = 0.0;
        tileset_offset_y -= tileset_inc_y;
    }
}

/**
 * The function which generates the texture coordinates for the map
 * geometry, using the cached tile coordinates.
 */
void Map::generate_map_texcoords() {
    LOG(INFO) << "Generating map texture data";

    //holds the map data
    //need 12 float for the 2D texture coordinates
    int num_floats = 12;
    size_t data_size = sizeof(GLfloat) * map_height * map_width * num_floats * layers.size();
    try {
        map_tex_coords = new GLfloat[data_size];
    }
    catch(std::bad_alloc& ba) {
        LOG(ERROR) << "Out of memory in Map::generate_map_texcoords";
    }

    //Get each layer of the map
    //Start at layer 0
    int offset = 0;
    int layer =0;
    for(auto iter = layers.begin(); iter != layers.end(); ++iter) {
        auto layer_data = (*iter)->get_layer_data();

        if ((*iter)->get_name() == "Collisions") {
            continue;
        }
        //Get all the tiles in the layer, moving from left to right and down 
        for(auto tile_data = layer_data->begin(); tile_data != layer_data->end(); ++tile_data) {
            std::string tileset_name = tile_data->first;
            int tile_id = tile_data->second;
            if (tileset_name == "") {
                tile_id = 119; //move to default blank tile
            }

            GLfloat *tileset_ptr = &tileset_tex_coords[(tile_id)*8];
            //bottom left
            map_tex_coords[offset+0] = tileset_ptr[0];
            map_tex_coords[offset+1] = tileset_ptr[1];

            //top left
            map_tex_coords[offset+2] = tileset_ptr[2];
            map_tex_coords[offset+3] = tileset_ptr[3];

            //bottom right
            map_tex_coords[offset+4] = tileset_ptr[4];
            map_tex_coords[offset+5] = tileset_ptr[5];

            //top left
            map_tex_coords[offset+6] = tileset_ptr[2];
            map_tex_coords[offset+7] = tileset_ptr[3];

            //top right
            map_tex_coords[offset+8] = tileset_ptr[6];
            map_tex_coords[offset+9] = tileset_ptr[7];
            
            //bottom right
            map_tex_coords[offset+10] = tileset_ptr[4];
            map_tex_coords[offset+11] = tileset_ptr[5];
            //layer*map_height*map_width*num_floats + x*map_height*num_floats + y*num_floats

            offset += num_floats;
        }
        //next layer
        layer++;
    }

    //Set this data in the renderable component
    renderable_component.set_texture_coords_data(map_tex_coords, data_size, false);

}

/*
 * The function which generates the map geometry so that it can be
 * rendered to the screen
 */
void Map::generate_map_coords() {
    LOG(INFO) << "Generating map coordinate data";

    //holds the map data
    //need 18 floats for each coordinate as these hold 3D coordinates
    int num_floats = 18;
    size_t data_size = sizeof(GLfloat) * map_height * map_width * num_floats * layers.size();
    try {
        map_data = new GLfloat[data_size]; 
    }
    catch(std::bad_alloc& ba) {
        LOG(ERROR) << "Out of memory in Map::generate_map_coords";
        return;
    }

    float scale = (float)(Engine::get_tile_size() * Engine::get_global_scale());

    //generate the map data
    ///
    /// Vertex winding order:
    /// 1, 3   4
    ///  * --- * 
    ///  |     |
    ///  |     | 
    ///  * --- *
    /// 0       2,5
    ///

    //For all the layers
    //Start at layer 0
    int offset = 0;
    float layer_offset = -1.0f;
    float layer_inc = 1.0f / float(layers.size());
    for (unsigned int layer = 0; layer < layers.size(); layer++) {
        LOG(INFO) << "Map::generate_map_coords: Layer offset of " << layer_offset;

        //Generate one layer's worth of data
        for(int y = 0; y < map_height; y++) {
            for(int x = 0; x < map_width; x++) {
                // If it is a blank tile, draw it as a zero-area dot out of the way (hack).
                // if (layers[layer]->get_tile(x, y) == 0) {
                //     map_data[offset+0] =
                //         map_data[offset+1] =
                //         map_data[offset+3] =
                //         map_data[offset+4] =
                //         map_data[offset+6] =
                //         map_data[offset+7] =
                //         map_data[offset+9] =
                //         map_data[offset+10] =
                //         map_data[offset+12] =
                //         map_data[offset+13] =
                //         map_data[offset+15] =
                //         map_data[offset+16] = scale * float(-1.0);
                //     map_data[offset+2] =
                //         map_data[offset+5] =
                //         map_data[offset+8] =
                //         map_data[offset+11] =
                //         map_data[offset+14] =
                //         map_data[offset+17] = layer_offset;
                //     continue;
                // }
     
                //generate one tile's worth of data

                //bottom left
                map_data[offset+ 0] = scale * float(x);
                map_data[offset+ 1] = scale * float(y);
                map_data[offset+ 2] = layer_offset;
             
                //top left
                map_data[offset+ 3] = scale * float(x);
                map_data[offset+ 4] = scale * float(y + 1);
                map_data[offset+ 5] = layer_offset;

                //bottom right
                map_data[offset+ 6] = scale * float(x + 1);
                map_data[offset+ 7] = scale * float(y);
                map_data[offset+ 8] = layer_offset;
            
                //top left
                map_data[offset+ 9]  = scale * float(x);
                map_data[offset+10] = scale * float(y+1);
                map_data[offset+11] = layer_offset;
        
                //top right
                map_data[offset+12] = scale * float(x+1);
                map_data[offset+13] = scale * float(y+1);
                map_data[offset+14] = layer_offset;

                //bottom right
                map_data[offset+15] = scale * float(x+1);
                map_data[offset+16] = scale * float(y);
                map_data[offset+17] = layer_offset;

                //layer*map_width*map_height*num_floats + x*map_height*num_floats + y*num_floats
                offset += num_floats;

            }
        }
        layer_offset += layer_inc;
    }

    //Set this data in the renderable component
    renderable_component.set_vertex_data(map_data, data_size, false);
    renderable_component.set_num_vertices_render(GLsizei(layers.size() * 6 * map_width * map_height));
}

void Map::init_textures() {
    texture_images[0] = Image("../resources/basictiles_2.png");

    //Set the texture data in the rederable component
    renderable_component.set_texture_image(&texture_images[0]);
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
    
    renderable_component.set_shader(shader);

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
    (*blocker)[tile.x][tile.y] = (*blocker)[tile.x][tile.y] - 1 ;

    LOG(INFO) << "Block level at tile " << tile.x << " " <<tile.y
      << " decreased from " << (*blocker)[tile.x][tile.y] + 1
      << " to " << (*blocker)[tile.x][tile.y] << ".";
}

Map::Blocker Map::block_tile(Vec2D tile) {
    return Blocker(tile, &blocker);
}



