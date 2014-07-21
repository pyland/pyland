#include "game_window.hpp"
#include "layer.hpp"
#include "map.hpp"
#include "map_loader.hpp"
#include "map_object.hpp"
#include "object_manager.hpp"
#include "tileset.hpp"
#include "walkability.hpp"

#include <string>
#include <iostream>
#include <fstream>

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


#define VERTEX_POS_INDX 0
#define VERTEX_TEXCOORD0_INDX 1
#define PATH "./"
#define GLOBAL_SCALE 2

#define IMAGE1_SIZE_WIDTH 128
#define IMAGE1_NUM_COMPONENTS 4
#define IMAGE1_SIZE_HEIGHT 240


#define TILESET_ELEMENT_SIZE 16

Map::Map(const std::string map_src) : renderable_component() {
    //Load the map
    MapLoader map_loader;
    bool result = map_loader.load_map(map_src);
    if(!result)  {

        std::cerr << "COULDN't LOAD MAP" << std::endl;
        return;
    }
    
    //Get the loaded map data
    map_width = map_loader.get_map_width();
    map_height = map_loader.get_map_height();

    std::cout << "MAP LOADING: " <<std::endl;
    std::cout << "WIDTH: " << map_width << " HEIGHT: " << map_height << std::endl;
    layers = map_loader.get_layers();
    tilesets = map_loader.get_tilesets();
    objects  = map_loader.get_objects();

    //Get the tilesets
    //TODO: We'll only support one tileset at the moment
    //Get an object list
    


    //Generate the geometry needed for this map
    init_shaders();
    generate_tileset_coords(IMAGE1_SIZE_WIDTH, IMAGE1_SIZE_HEIGHT);
    generate_map_texcoords();
    generate_map_coords();
    init_textures();
}

Map::~Map() {
    // release buffers
    delete[] tex_buf[0];
    delete[] map_data;
    delete[] map_tex_coords;
    delete[] tileset_tex_coords;

    std::cout << std::endl << "Closed" << std::endl;
}

bool Map::is_walkable(int x_pos, int y_pos) {
    //Default is walkable
    bool walkable = true;

    //Iterate through all objects
    for(auto character : characters) {
        //If its an invalid object
        if (character == 0)
            continue;

        std::shared_ptr<Object> object = ObjectManager::get_instance().get_object(character);

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
    for(auto iter = layers.begin(); iter != layers.end(); ++iter) {
        std::shared_ptr<Layer> layer = *iter;
    
        //determine if we can walk on the map

        if(layer->get_name() == "Collisions") {

            //if there is a tile, treat it as blocked
            if(layer->get_tile(x_pos, y_pos) != 0) {
                walkable = true;
                // std::cout << "COLLIDING" << layer->get_tile(x_pos, y_pos) << std::endl;
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
void Map::generate_tileset_coords(int tileset_width, int tileset_height) {
#ifdef DEBUG
    printf("GENERATING TILESET TEXTURE COORDS...");
#endif
    //check the tilset image height and widths are multiples of the tiles
    //  assert(image_height % TILESET_ELEMENT_SIZE != 0 || image_width % TILESET_ELEMENT_SIZE != 0);

    assert(TILESET_ELEMENT_SIZE != 0);

    int num_tiles_x = tileset_width  / TILESET_ELEMENT_SIZE;
    int num_tiles_y = tileset_height / TILESET_ELEMENT_SIZE;
    std::cout << num_tiles_x << " " << num_tiles_y << std::endl;

    assert(num_tiles_x);
    assert(num_tiles_y);

    //Each tile needs 8 floats to describe its position in the image
    tileset_tex_coords = new GLfloat[sizeof(GLfloat)* num_tiles_x * num_tiles_y * 4 * 2];
    assert(tileset_tex_coords);

    //Tiles are indexed from top left but Openl uses texture coordinates from bottom left
    //so we remap these 
    //We work from left to right, moving down
    GLfloat tileset_offset_x = 0.0;
    GLfloat tileset_offset_y = 0.0;
    GLfloat tileset_inc_x = GLfloat(1.0) / static_cast<GLfloat>(num_tiles_x);
    GLfloat tileset_inc_y = GLfloat(1.0) / static_cast<GLfloat>(num_tiles_y);
    //TODo: DIV ZEro HERRE 

    //TODO: REMEMBER TILESET COORDINATES ARE INVERSE OF IMAGE FILE ONES
    //generate the coordinates for each tile
    for (int y = 0; y < num_tiles_y; y++) {
        for (int x = 0; x < num_tiles_x; x++) {
            //bottom left
            tileset_tex_coords[y* num_tiles_x*4*2+x*(4*2)]  = tileset_offset_x;
            tileset_tex_coords[y* num_tiles_x*4*2+x*4*2 +1] = tileset_offset_y + tileset_inc_y;
 
            //top left
            tileset_tex_coords[y* num_tiles_x*4*2+x*4*2+ 2] = tileset_offset_x;
            tileset_tex_coords[y* num_tiles_x*4*2+x*4*2+3]  = tileset_offset_y;
 
            //bottom right
            tileset_tex_coords[y* num_tiles_x*4*2+x*4*2+4] = tileset_offset_x + tileset_inc_x;
            tileset_tex_coords[y* num_tiles_x*4*2+x*4*2+5] = tileset_offset_y + tileset_inc_y;
 
            //top right
            tileset_tex_coords[y* num_tiles_x*4*2+x*4*2+6] = tileset_offset_x + tileset_inc_x;
            tileset_tex_coords[y* num_tiles_x*4*2+x*4*2+7] = tileset_offset_y;
 
            tileset_offset_x += tileset_inc_x;
            //      for(int i =0 ; i < 8; i++)
            //        cout << " " <<        tileset_tex_coords[y* num_tiles_x*4*2+x*4*2+i];
            //      cout << endl;
        }
        tileset_offset_x = 0.0;
        tileset_offset_y += tileset_inc_y;
    }
}

/**
 * The function which generates the texture coordinates for the map
 * geometry, using the cached tile coordinates.
 */
void Map::generate_map_texcoords() {
#ifdef DEBUG
    printf("GENERATING MAP TEXTURE DATA...");
#endif
    //holds the map data
    //need 12 float for the 2D texture coordinates
    int num_floats = 12;
    int data_size = sizeof(GLfloat)*map_height*map_width*num_floats*layers.size();
    map_tex_coords = new GLfloat[data_size]; 
    assert(map_tex_coords);

    //Get each layer of the map
    //Start at layer 0
    int offset = 0;
    int layer =0;
    for(auto iter = layers.begin(); iter != layers.end(); ++iter) {
        auto layer_data = (*iter)->get_layer_data();

        //Get all the tiles in the layer, moving from left to right and down 
        for(auto tile_data = layer_data->begin(); tile_data != layer_data->end(); ++tile_data) {
            int tile_id = tile_data->second;
            //TODO, stop out of bounds here - if it exceeds the tileset size
            if(tile_id == 0)
                tile_id = 83; //move to default blank tile
            GLfloat *tileset_ptr = &tileset_tex_coords[tile_id*8];
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

    /*
    //generate the map data
    // get the tile set coordinates for the particular tile
    for(int x = 0; x < map_width; x++) {
        for(int y = 0; y < map_height; y++) {
            int curr_tile = world_data[x][y];

            GLfloat *tileset_ptr = &tileset_tex_coords[curr_tile*8];
            //bottom left
            map_tex_coords[x*map_height*num_floats + y*num_floats+0] = tileset_ptr[0];
            map_tex_coords[x*map_height*num_floats + y*num_floats+1] = tileset_ptr[1];

            //top left
            map_tex_coords[x*map_height*num_floats + y*num_floats+2] = tileset_ptr[2];
            map_tex_coords[x*map_height*num_floats + y*num_floats+3] = tileset_ptr[3];

            //bottom right
            map_tex_coords[x*map_height*num_floats + y*num_floats+4] = tileset_ptr[4];
            map_tex_coords[x*map_height*num_floats + y*num_floats+5] = tileset_ptr[5];

            //top left
            map_tex_coords[x*map_height*num_floats + y*num_floats+6] = tileset_ptr[2];
            map_tex_coords[x*map_height*num_floats + y*num_floats+7] = tileset_ptr[3];

            //top right
            map_tex_coords[x*map_height*num_floats + y*num_floats+8] = tileset_ptr[6];
            map_tex_coords[x*map_height*num_floats + y*num_floats+9] = tileset_ptr[7];
            
            //bottom right
            map_tex_coords[x*map_height*num_floats + y*num_floats+10] = tileset_ptr[4];
            map_tex_coords[x*map_height*num_floats + y*num_floats+11] = tileset_ptr[5];
        }
    }
    */
    //Set this data in the renderable component
    renderable_component.set_texture_coords_data(map_tex_coords, data_size, false);

}

/*
 * The function which generates the map geometry so that it can be
 * rendered to the screen
 */
void Map::generate_map_coords() {
#ifdef DEBUG
    printf("GENERATING MAP DATA...");
#endif
    //holds the map data
    //need 18 floats for each coordinate as these hold 3D coordinates
    int num_floats = 18;
    int data_size = sizeof(GLfloat)*map_height*map_width*num_floats*layers.size();
    map_data = new GLfloat[data_size]; 
    assert(map_data);

    float scale = TILESET_ELEMENT_SIZE * GLOBAL_SCALE;

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
    float layer_inc = 1.0f / (float)layers.size();
    for(int layer = 0; layer < layers.size(); layer++) {
        std::cout << "OFFSET " << layer_offset << std::endl;

        //Generate one layer's worth of data
        for(int y = 0; y < map_height; y++) {
            for(int x = 0; x < map_width; x++) {
     
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
    /*
    for(int x = 0; x < map_width; x++) {
        for(int y = 0; y < map_height; y++) {
            //generate one tile's worth of data

            //bottom left
            map_data[x*map_height*num_floats + y*num_floats+ 0] = scale * float(x);
            map_data[x*map_height*num_floats + y*num_floats+ 1] = scale * float(y);
            map_data[x*map_height*num_floats + y*num_floats+ 2] = 0;
             
            //top left
            map_data[x*map_height*num_floats + y*num_floats+ 3] = scale * float(x);
            map_data[x*map_height*num_floats + y*num_floats+ 4] = scale * float(y + 1);
            map_data[x*map_height*num_floats + y*num_floats+ 5] = 0;

            //bottom right
            map_data[x*map_height*num_floats + y*num_floats+ 6] = scale * float(x + 1);
            map_data[x*map_height*num_floats + y*num_floats+ 7] = scale * float(y);
            map_data[x*map_height*num_floats + y*num_floats+ 8] = 0;
            
            //top left
            map_data[x*map_height*num_floats + y*num_floats+ 9]  = scale * float(x);
            map_data[x*map_height*num_floats + y*num_floats+10] = scale * float(y+1);
            map_data[x*map_height*num_floats + y*num_floats+11] = 0;
        
            //top right
            map_data[x*map_height*num_floats + y*num_floats+12] = scale * float(x+1);
            map_data[x*map_height*num_floats + y*num_floats+13] = scale * float(y+1);
            map_data[x*map_height*num_floats + y*num_floats+14] = 0;

            //bottom right
            map_data[x*map_height*num_floats + y*num_floats+15] = scale * float(x+1);
            map_data[x*map_height*num_floats + y*num_floats+16] = scale * float(y);
            map_data[x*map_height*num_floats + y*num_floats+17] = 0;
        }
    }*/

#ifdef DEBUG
    printf("DONE.");
#endif

    //Set this data in the renderable component
    renderable_component.set_vertex_data(map_data, data_size, false);
    renderable_component.set_num_vertices_render(layers.size()*6*map_width*map_height);
}

void Map::init_textures() {
    
    FILE *tex_file1 = nullptr;
    size_t bytes_read = 0;
    size_t image_sz_1 = IMAGE1_SIZE_WIDTH*IMAGE1_SIZE_HEIGHT*IMAGE1_NUM_COMPONENTS;

    tex_buf[0] = new char[image_sz_1];

    tex_file1 = fopen(PATH "../resources/basictiles_2.raw", "rb");
    if(tex_file1 == nullptr) {
        std::cerr << "ERROR: Couldn't load textures" << std::endl;
    }

    if (tex_file1 && tex_buf[0]) {
        bytes_read = fread(tex_buf[0], 1, image_sz_1, tex_file1);
        assert(bytes_read == image_sz_1);  // some problem with file?
        fclose(tex_file1);
    }
    //Set the texture data in the rederable component
    renderable_component.set_texture_data(tex_buf[0], static_cast<int>(image_sz_1), IMAGE1_SIZE_WIDTH, IMAGE1_SIZE_HEIGHT, false);
}

/**
 * This function initialises the shader, creating and loading them.
 */ 
bool Map::init_shaders() {
#ifdef USE_GLES
    //read in the shaders
    std::ifstream vertex_shader_src("vert_shader.glesv");
    std::ifstream fragment_shader_src("frag_shader.glesf");
#endif
#ifdef USE_GL
    //read in the shaders
    std::ifstream vertex_shader_src("vert_shader.glv");
    std::ifstream fragment_shader_src("frag_shader.glf");
#endif

    if (!vertex_shader_src.good()){
        std::cerr << "Failed to load vertex shader" << std::endl;
        return false;
    }
    
    if (!fragment_shader_src.good()) {
        std::cerr << "Failed to load fragment shader" << std::endl;
        return false;
    }

    std::string vert_src, frag_src, line;
    while (getline(vertex_shader_src, line)) {
        vert_src += line + "\n";
    }

    while (getline(fragment_shader_src, line)) {
        frag_src += line + "\n";
    }

    Shader* shader = new Shader(vert_src, frag_src);
  
    if (!shader->is_loaded()) {
        delete shader;
        shader = nullptr;
        std::cerr << "Failed to create the shader" << std::endl;
        return false;
    }

    //Set the shader
    renderable_component.set_shader(shader);

    return true;

}


/**
 * The function used to update elements on the map.
 */
void Map::update_map(float dt) {
}
