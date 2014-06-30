#include "MAP_H"

/**
 * Constructor for Map
 */ 
Map::Map() {

}

/** 
 *Destructor for Map
 */
Map::~Map() {

}
/**
 * Function used to generate the necessary Vertex Buffer Objects to
 * hold the map data to achieve more efficient rendering.
 */
void Map::init_vbo_buffer() {

}

/**
 * The function used to generate the cache of tile texture coordinates.
 */ 
void Map::generate_tileset_coords(int tileset_width, int tileset_height) {

}

/**
 * The function which generates the texture coordinates for the map
 * geometry, using the cached tile coordinates.
 */
void Map:: generate_map_texcoords() {

}

/*
 * The function which generates the map geometry so that it can be
 * rendered to the screen
 */
void Map::generate_map_coords() {

}

/**
 * This function loads the shaders
 */ 
GLuint Map::load_shader(GLenum type, const std::string src) {

} 

/** 
 * This function crates the Opengl program
 */
GLuint Map::shader_create(const string vs, const string fs) {

}

/** 
 * This function loads the required texture images
 */ 
void Map::load_tex_images() {

}

/** 
 * This function performs the required Opengl initializatin for the textures.
 */ 
void Map::init_textures() {

}

/**
 * This function initialises the shader, creating and loading them.
 */ 
bool Map::init_shaders() {

}

/**
 * The function used to render the map. Makes the necessary Opengl
 * to correctly render the map.
 */
void Map::render_map() {

}

/**
 * The function used to update elements on the map.
 */
void update_map(float dt) {

}
