#include "image.hpp"
#include "engine_api.hpp"
#include "entitythread.hpp"
#include "map_viewer.hpp"
#include "renderable_component.hpp"
#include "sprite.hpp"
#include "object_manager.hpp"

#include <new>
#include <glog/logging.h>
#include <iostream>
#include <fstream>


#ifdef USE_GLES

#include <GLES2/gl2.h>

#endif

#ifdef USE_GL

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#endif

/// TODO: REMOVE
Sprite::Sprite() {
    sprite_sheet = "../resources/characters_1.png";
    sprite_sheet_id = 0;
    x_position = 0;
    y_position = 0;
    name = "";

    init_shaders();
    load_textures();
    generate_tex_data();
    generate_vertex_data();

    // setting up sprite name text
    Typeface mytype("../fonts/hans-kendrick/HansKendrick-Regular.ttf");
    TextFont myfont(mytype, 18);
    object_text = new Text(Engine::get_map_viewer()->get_window(), myfont, true);
    object_text->set_text(name);
    Vec2D pixel_position = Engine::get_map_viewer()->tile_to_pixel(Vec2D(x_position, y_position));
    object_text->move(pixel_position.x ,pixel_position.y );
    object_text->resize(100,100);
    object_text->align_centre();
    object_text->align_at_origin(true);

    LOG(INFO) << "setting up text at " << pixel_position.to_string() ;

    // setting up status text
    status_text = new Text(Engine::get_map_viewer()->get_window(), myfont, true);
    status_text->set_text("");
    Vec2D pixel_text = Engine::get_map_viewer()->tile_to_pixel(Vec2D(x_position, y_position));
    status_text->move(pixel_text.x ,pixel_text.y);
    status_text->resize(100,100);
    status_text->align_centre();
    status_text->align_at_origin(true);

    // Starting positions should be integral
    assert(trunc(x_position) == x_position);
    assert(trunc(y_position) == y_position);

    // Make a sprite blocked
    blocked_tiles.insert(std::make_pair("stood on", Engine::get_map_viewer()->get_map()->block_tile(
        Vec2D(int(x_position), int(y_position))
    )));


    LOG(INFO) << "Sprite initialized";

}
Sprite::Sprite(int _x_position, int _y_position, std::string _name, int _sprite_sheet_id, std::string _sprite_sheet) {
    LOG(INFO) << "register new sprite called " << _name;

    // set name & positon 
    x_position = _x_position;
    y_position = _y_position;
    name = _name;
    sprite_sheet_id = _sprite_sheet_id;
    sprite_sheet = _sprite_sheet;

    init_shaders();
    load_textures();
    generate_tex_data();
    generate_vertex_data();

    // setting up sprite name text
    Typeface mytype("../fonts/hans-kendrick/HansKendrick-Regular.ttf");
    TextFont myfont(mytype, 18);
    object_text = new Text(Engine::get_map_viewer()->get_window(), myfont, true);
    object_text->set_text(name);
    Vec2D pixel_position = Engine::get_map_viewer()->tile_to_pixel(Vec2D(x_position, y_position));
    object_text->move(pixel_position.x ,pixel_position.y );
    object_text->resize(100,100);
    object_text->align_centre();
    object_text->align_at_origin(true);

    LOG(INFO) << "setting up text at " << pixel_position.to_string() ;

    // setting up status text
    status_text = new Text(Engine::get_map_viewer()->get_window(), myfont, true);
    status_text->set_text("");
    Vec2D pixel_text = Engine::get_map_viewer()->tile_to_pixel(Vec2D(x_position, y_position));
    status_text->move(pixel_text.x ,pixel_text.y);
    status_text->resize(100,100);
    status_text->align_centre();
    status_text->align_at_origin(true);

    // Starting positions should be integral
    assert(trunc(x_position) == x_position);
    assert(trunc(y_position) == y_position);

    // Make a sprite blocked
    blocked_tiles.insert(std::make_pair("stood on", Engine::get_map_viewer()->get_map()->block_tile(
        Vec2D(int(x_position), int(y_position))
    )));

    /// build focus icon
    LOG(INFO) << "setting up focus icon";
    focus_icon = std::make_shared<MapObject>(x_position, y_position, "focus icon", 96);
    ObjectManager::get_instance().add_object(focus_icon);
    auto focus_icon_id = focus_icon->get_id();
    LOG(INFO) << "created focus icon with id: " << focus_icon_id;
    Engine::get_map_viewer()->get_map()->add_map_object(focus_icon_id);


    LOG(INFO) << "Sprite initialized";

}

Sprite::~Sprite() {
    delete object_text;
    delete status_text;
    LOG(INFO) << "Sprite destructed";
}


void Sprite::set_state_on_moving_start(Vec2D target) {
    moving = true;
    // adding blocker to new tile
    blocked_tiles.insert(std::make_pair("walking to", Engine::get_map_viewer()->get_map()->block_tile(target)));
}

void Sprite::set_state_on_moving_finish() {
    moving = false;
    // removing old blocker and changing key for new one
    blocked_tiles.erase("stood on");
    blocked_tiles.insert(std::make_pair("stood on", blocked_tiles.at("walking to")));
    blocked_tiles.erase("walking to");
}


void Sprite::generate_tex_data() {
  
    //holds the texture data
    //need 12 float for the 2D texture coordinates
    int num_floats = 12;

    GLfloat* sprite_tex_data = nullptr;
    try {

        sprite_tex_data = new GLfloat[sizeof(GLfloat)*num_floats];
    }
    catch(std::bad_alloc& ba) {
        LOG(ERROR) << "ERROR in Sprite::generate_tex_data(), cannot allocate memory";
        return;
    }

    int image_width = renderable_component.get_texture_image()->store_width;
    int image_height = renderable_component.get_texture_image()->store_height;
    if(image_width == 0 || image_height == 0 ) {
        LOG(ERROR) << "At least one image dimension is 0";
        return;
    }

    GLfloat inc_x = GLfloat(Engine::get_tile_size()) / (GLfloat)image_width;
    GLfloat inc_y = GLfloat(Engine::get_tile_size()) / (GLfloat)image_height;
    if(inc_x == 0.0f  ||inc_y == 0.0f) {
        LOG(ERROR) << "Increments are 0";
        return;
    }

    //Tile ids are from top left but opengl texture coordinates are bottom left so adjust as needed
    GLfloat offset_x = (GLfloat)(sprite_sheet_id % (int)(image_width/Engine::get_tile_size()))*inc_x;
    GLfloat offset_y = (1.0f - inc_y)-  (GLfloat)(sprite_sheet_id / (int)(image_width/Engine::get_tile_size()))*inc_y;

    //bottom left
    sprite_tex_data[0]  = offset_x;
    sprite_tex_data[1]  = offset_y;

    //top left
    sprite_tex_data[2]  = offset_x;
    sprite_tex_data[3]  = offset_y + inc_y;

    //bottom right
    sprite_tex_data[4]  = offset_x + inc_x;
    sprite_tex_data[5]  = offset_y;

    //top left
    sprite_tex_data[6]  = offset_x;
    sprite_tex_data[7]  = offset_y + inc_y;

    //top right
    sprite_tex_data[8]  = offset_x + inc_x;
    sprite_tex_data[9]  = offset_y + inc_y;

    //bottom right
    sprite_tex_data[10] = offset_x + inc_x;
    sprite_tex_data[11] = offset_y; 

    renderable_component.set_texture_coords_data(sprite_tex_data, sizeof(GLfloat)*num_floats, false);
} 
void Sprite::set_sprite_sheet_id(int _sprite_sheet_id) {
    sprite_sheet_id  = _sprite_sheet_id; 
    
    generate_tex_data();
}
void Sprite::set_sprite_sheet(std::string _sprite_sheet) {
    sprite_sheet = _sprite_sheet; 
    generate_tex_data();
}
void Sprite::generate_vertex_data() {
    //holds the sprite vertex data
    int num_dimensions = 2;
    int num_floats = num_dimensions*6;//GLTRIANGLES
    GLfloat* sprite_data = nullptr;
    try {

        sprite_data = new GLfloat[sizeof(GLfloat)*num_floats];
    }
    catch(std::bad_alloc& ba) {
        LOG(ERROR) << "ERROR in Sprite::generate_vertex_data(), cannot allocate memory";
        return;
    }

    float scale =(float)( Engine::get_tile_size() * Engine::get_global_scale());
 
    //bottom left 
    sprite_data[0] = 0;
    sprite_data[1] = 0;

    //top left
    sprite_data[2] = 0;
    sprite_data[3] = scale;

    //bottom right
    sprite_data[4] = scale;
    sprite_data[5] = 0;

    //top left
    sprite_data[6] = 0;
    sprite_data[7] = scale;

    //top right
    sprite_data[8] = scale;
    sprite_data[9] = scale;

    //bottom right
    sprite_data[10] = scale;
    sprite_data[11] = 0;

    renderable_component.set_vertex_data(sprite_data,sizeof(GLfloat)*num_floats, false);
    renderable_component.set_num_vertices_render(num_floats/num_dimensions);//GL_TRIANGLES being used
}

void Sprite::load_textures() {
    Image* texture_image = nullptr;

    try {
        texture_image = new Image(sprite_sheet.c_str());
    }
    catch(std::exception e) {
        texture_image = nullptr;
        LOG(ERROR) << "Failed to create texture";
        return;
    }

    //Set the texture data in the rederable component
    renderable_component.set_texture_image(texture_image);
}

bool Sprite::init_shaders() {
    std::shared_ptr<Shader> shader;
    try {
        shader = Shader::get_shared_shader("tile_shader");
    }
    catch (std::exception e) {
        LOG(ERROR) << "Failed to create the shader";
        return false;
    }

    //Set the shader
    renderable_component.set_shader(shader);

    return true;

}

void Sprite::add_to_inventory(std::shared_ptr<MapObject> new_object) {
    LOG(INFO) << "adding item to sprites inventory";
    new_object->set_x_position(x_position);
    new_object->set_y_position(y_position);
    inventory.push_back(new_object);
}

void Sprite::set_y_position(int y_pos) { 
    y_position = y_pos; 
    for (auto item: get_inventory()) {
        item->set_y_position(y_position);
    }
    focus_icon->set_y_position(y_position);
}
void Sprite::set_x_position(int x_pos) { 
    x_position = x_pos;
    for (auto item: get_inventory()) {
        item->set_x_position(x_position);
    }
    focus_icon->set_x_position(x_position);
    x_position = x_pos; 
}

void Sprite::set_y_position(double y_pos) { 
    y_position = y_pos; 
    for (auto item: get_inventory()) {
        item->set_y_position(y_position);
    }
    focus_icon->set_y_position(y_position);
}
void Sprite::set_x_position(double x_pos) { 
    x_position = x_pos;
    for (auto item: get_inventory()) {
        item->set_x_position(x_position);
    }
    focus_icon->set_x_position(x_position);
    x_position = x_pos; 
}

bool Sprite::remove_from_inventory(std::shared_ptr<MapObject> old_object) {
    // there must be a better way to do this
    auto it = std::find(std::begin(inventory), std::end(inventory), old_object);
    if (it != std::end(inventory)) {
        inventory.erase(it);
        LOG(INFO) << "removing item to sprites inventory";
        return true;
    } else {
        return false;
    }
}

bool Sprite::is_in_inventory(std::shared_ptr<MapObject> object) {
    auto it = std::find(std::begin(inventory), std::end(inventory), object);
    return (it != std::end(inventory)); 
}


Status Sprite::string_to_status(std::string status) {
    std::map<std::string,Status> string_map;
    string_map["running"] = RUNNING;
    string_map["stopped"] = STOPPED; 
    string_map[""] = NOTHING;
    string_map["failed"] = FAILED;
    string_map["killed"] = KILLED;
    return string_map[status];
}

void Sprite::set_sprite_status(std::string _sprite_status) {
        sprite_status = string_to_status(_sprite_status);
        status_text->set_text(_sprite_status);
}

 void Sprite::set_focus(bool is_focus) {
    LOG(INFO) << "trying to set focus to "<< is_focus;
    focus_icon->set_renderable(is_focus);

 }
