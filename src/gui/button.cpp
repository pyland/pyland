#include "button.hpp"
#include "gui_text.hpp"
#include "engine.hpp"
#include "texture_atlas.hpp"
#include <iostream>
#include "text.hpp"
#include <tuple>
#include <utility>
#include <vector>
#include <memory>

Button::Button(ButtonType _type) {
    type = _type;
    alignment = ButtonAlignment::TopRight;
    button_text = std::make_shared<GUIText>();
    button_text->set_width(1.0f);
    button_text->set_height(1.0f);
    button_text->set_x_offset(0.3f);
    button_text->set_y_offset(0.1f);
    set_text("");

    get_text()->set_bloom_radius(4);

    add(button_text);

    picture_name = "gui/black-tile";

    if(type == ButtonType::Board){
        set_width(1.0f);
        set_height(0.85f);
    }
    else if(type == ButtonType::Single){
        set_width(0.5f);
        set_height(0.5f);
    }
    else if(type == ButtonType::NoPicture){
        set_width(0.5f);
        set_height(0.5f);
        button_text->set_x_offset(0.2f);
        button_text->set_y_offset(0.5f);
    }

}

Button::Button(ButtonType _type, std::shared_ptr<Text>  _text, std::function<void (void)> on_click,
               float _width, float _height, float _x_offset, float _y_offset) :
    ComponentGroup(on_click, _width, _height, _x_offset, _y_offset) {

    alignment = ButtonAlignment::TopRight;
    type= _type;
    button_text = std::make_shared<GUIText>();
    button_text->set_width(1.0f);
    button_text->set_height(1.0f);
    button_text->set_x_offset(0.3f);
    button_text->set_y_offset(0.1f);
    button_text->set_text(_text);
    get_text()->set_bloom_radius(4);

    add(button_text);
}

void Button::resize_text(float width, float height){
    button_text->set_width(width);
    button_text->set_height(height);
}

void Button::move_text(float x_offset, float y_offset){
    button_text->set_x_offset(x_offset);
    button_text->set_y_offset(y_offset);
}

void Button::set_alignment(ButtonAlignment _alignment){
    alignment = _alignment;
}

ButtonAlignment Button::get_alignment(){
    return alignment;
}

void Button::set_picture(std::string _name){
    picture_name = _name;
}

void Button::set_text(std::string text) {
    TextFont buttonfont = Engine::get_game_font();

    std::shared_ptr<Text> new_text = std::make_shared<Text>(Engine::get_game_window(), buttonfont, true);
    new_text->set_text(text);
    button_text->set_text(new_text);
    get_text()->set_bloom_radius(4);
}
std::shared_ptr<Text> Button::get_text() {
    return button_text->get_text();
}

void Button::set_text(std::shared_ptr<Text> _text) {

    button_text->set_text(_text);
    get_text()->set_bloom_radius(4);
}

void Button::set_text_colour(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return button_text->get_text()->set_colour(r, g, b, a);
}

std::vector<std::pair<GLfloat*, int>> Button::generate_this_vertex_data() {
    delete []vertex_data;
    vertex_data = nullptr;
    std::vector<std::pair<GLfloat*, int>> vertices;
    size_vertex_data = 0;
    if(!is_visible())
        return vertices;
    //Generate coordinates in our local object space

    //Calculate any needed data
    float element_width_pixels = float(Engine::get_tile_size());
    float element_height_pixels = float(Engine::get_tile_size());
    //background
    float background_right = float(width_pixels) - element_width_pixels;
    //    if(background_right < element_width_pixels) background_right = element_width_pixels;
    float background_top = float(height_pixels) - element_height_pixels;
    //    if(background_top < element_height_pixels) background_top = element_height_pixels;

    //Generate the data buffer
    int num_floats_per_tile = 12;
    int total_floats = 0;
    int offset = 0;

    if(type == ButtonType::Board){

        std::tuple<float,float,float,float> background_bounds = std::make_tuple(element_width_pixels, background_right, background_top, element_height_pixels);

        std::tuple<float,float,float,float> corner_top_left_bounds = std::make_tuple(0.0f, float(element_width_pixels), float(height_pixels), float(height_pixels) - element_height_pixels);
        std::tuple<float,float,float,float> corner_top_right_bounds = std::make_tuple(float(width_pixels) - element_width_pixels, float(width_pixels), float(height_pixels), float(height_pixels) - element_height_pixels);
        std::tuple<float,float,float,float> corner_bottom_right_bounds = std::make_tuple(float(width_pixels) - element_width_pixels, float(width_pixels), float(element_height_pixels), 0.0f);
        std::tuple<float,float,float,float> corner_bottom_left_bounds = std::make_tuple(0.0f, float(element_width_pixels), float(element_height_pixels), 0.0f);
        std::tuple<float,float,float,float> edge_top_bounds = std::make_tuple(element_width_pixels, float(width_pixels) - element_width_pixels, float(height_pixels), float(height_pixels)  - element_height_pixels);
        std::tuple<float,float,float,float> edge_right_bounds = std::make_tuple(float(width_pixels) - element_width_pixels, float(width_pixels), float(height_pixels) - element_height_pixels,  element_height_pixels);
        std::tuple<float,float,float,float> edge_bottom_bounds = std::make_tuple(element_width_pixels, float(width_pixels) - element_width_pixels, float(element_height_pixels), 0.0f);
        std::tuple<float,float,float,float> edge_left_bounds = std::make_tuple(0.0f, float(element_width_pixels), float(height_pixels) - element_height_pixels, element_height_pixels);


        //get total number of floats
        total_floats += calculate_num_tile_elements(background_bounds, element_width_pixels, element_height_pixels) * num_floats_per_tile;
        total_floats += calculate_num_tile_elements(corner_top_left_bounds, element_width_pixels, element_height_pixels) * num_floats_per_tile;
        total_floats += calculate_num_tile_elements(corner_top_right_bounds, element_width_pixels, element_height_pixels) * num_floats_per_tile;
        total_floats += calculate_num_tile_elements(corner_bottom_right_bounds, element_width_pixels, element_height_pixels) * num_floats_per_tile;
        total_floats += calculate_num_tile_elements(corner_bottom_left_bounds, element_width_pixels, element_height_pixels) * num_floats_per_tile;
        total_floats += calculate_num_tile_elements(edge_top_bounds, element_width_pixels, element_height_pixels) * num_floats_per_tile;
        total_floats += calculate_num_tile_elements(edge_right_bounds, element_width_pixels, element_height_pixels) * num_floats_per_tile;
        total_floats += calculate_num_tile_elements(edge_bottom_bounds, element_width_pixels, element_height_pixels) * num_floats_per_tile;
        total_floats += calculate_num_tile_elements(edge_left_bounds, element_width_pixels, element_height_pixels) * num_floats_per_tile;

        vertex_data = new GLfloat[total_floats];

        //Generate the vertex coordinates for each element
        offset = generate_tile_element_vertex_coords(vertex_data, 0, background_bounds, element_width_pixels, element_height_pixels);
        offset = generate_vertex_coords_element(vertex_data, offset, corner_top_left_bounds);
        offset = generate_vertex_coords_element(vertex_data, offset, corner_top_right_bounds);
        offset = generate_vertex_coords_element(vertex_data, offset, corner_bottom_right_bounds);
        offset = generate_vertex_coords_element(vertex_data, offset, corner_bottom_left_bounds);
        offset = generate_tile_element_vertex_coords(vertex_data, offset, edge_top_bounds, element_width_pixels, element_height_pixels);
        offset = generate_tile_element_vertex_coords(vertex_data, offset, edge_right_bounds, element_width_pixels, element_height_pixels);
        offset = generate_tile_element_vertex_coords(vertex_data, offset, edge_bottom_bounds, element_width_pixels, element_height_pixels);
        offset = generate_tile_element_vertex_coords(vertex_data, offset, edge_left_bounds, element_width_pixels, element_height_pixels);
    }
    else if(type == ButtonType::Single){
        std::tuple<float,float,float,float> background_bounds;

        if(alignment == ButtonAlignment::TopLeft){
            background_bounds = std::make_tuple(0.0f, float(element_width_pixels), float(height_pixels), float(height_pixels) - element_height_pixels);
        }
        else if(alignment == ButtonAlignment::TopRight){
            background_bounds = std::make_tuple(float(width_pixels) - element_width_pixels, float(width_pixels), float(height_pixels), float(height_pixels) - element_height_pixels);
        }
        else /*if(alignment == ButtonAlignment::BottomLeft)*/{
            background_bounds = std::make_tuple(0.0f, float(element_width_pixels), float(element_height_pixels), 0.0f);
        }

        //get total number of floats
        total_floats += calculate_num_tile_elements(background_bounds, element_width_pixels, element_height_pixels) * num_floats_per_tile;

        vertex_data = new GLfloat[total_floats];

        //Generate the vertex coordinates for each element
        offset = generate_vertex_coords_element(vertex_data, 0, background_bounds);

    }
    else{
        //it is of type NoPicture, do nothing
    }
    size_vertex_data = offset;

    vertices.push_back(std::make_pair(vertex_data, total_floats));
    return vertices;
}

int Button::generate_tile_element_vertex_coords(GLfloat* data, int offset, std::tuple<float,float,float,float> bounds, float element_width, float element_height) {

    float left = std::get<0>(bounds);
    float right = std::get<1>(bounds);
    float top = std::get<2>(bounds);
    float bottom = std::get<3>(bounds);


    float num_x_float = (right - left) / element_width;
    float num_y_float = (top - bottom) / element_height;
    if(num_x_float < 0.0f) num_x_float = 0.0f;
    if(num_y_float < 0.0f) num_y_float = 0.0f;

    int num_x = int(num_x_float);
    int num_y = int(num_y_float);

    //if we need to create a tile that is smaller than a tile
    bool need_subtile_x = false;
    bool need_subtile_y = false;

    //truncate to remove decimal then compare with float again - if not whole tiles
    if(num_x_float > float(num_x)) {
        num_x++;
        need_subtile_x = true;
    }
    if(num_y_float > float(num_y)) {
        num_y++;
        need_subtile_y = true;
    }

    std::tuple<float,float,float,float> tile_bounds;
    for(int y = 0; y < num_y; y++) {
        for(int x = 0; x < num_x; x++) {
            ///The area that we have to fill
            float tile_left = left + float(x)*element_width;
            float tile_right = left+(float(x)+1.0f)*element_width;
            float tile_top = bottom + (float(y)+1.0f)*element_height;
            float tile_bottom = bottom + float(y)*element_height;

            //If we're on a sub tile
            if(y+1 == num_y && need_subtile_y) {
                tile_top = top;
            }

            if(x+1 == num_x && need_subtile_x) {
                tile_right = right;
            }

            //Generate the vertex data
            tile_bounds = std::make_tuple(tile_left, tile_right, tile_top, tile_bottom);
            offset = generate_vertex_coords_element(data, offset, tile_bounds);
        }
    }
    return offset;
}
int Button::calculate_num_tile_elements(std::tuple<float,float,float,float> bounds, float element_width, float element_height) {

    float left = std::get<0>(bounds);
    float right = std::get<1>(bounds);
    float top = std::get<2>(bounds);
    float bottom = std::get<3>(bounds);

    float num_x_float = (right - left) / element_width;
    float num_y_float = (top - bottom) / element_height;
    if(num_x_float < 0.0f) num_x_float = 0.0f;
    if(num_y_float < 0.0f) num_y_float = 0.0f;
    int num_x = int(num_x_float);
    int num_y = int(num_y_float);

    //truncate to remove decimal then compare with float again - if not whole tiles
    if(num_x_float > float(num_x)) {
        num_x++;
    }
    if(num_y_float > float(num_y)) {
        num_y++;
    }

    return num_x * num_y;
}
int Button::generate_tile_element_texture_coords(GLfloat* data, int offset, std::tuple<float,float,float,float> vertex_bounds, float element_width, float element_height, std::tuple<float,float,float,float> texture_bounds) {

    float left = std::get<0>(vertex_bounds);
    float right = std::get<1>(vertex_bounds);
    float top = std::get<2>(vertex_bounds);
    float bottom = std::get<3>(vertex_bounds);

    float num_x_float = (right - left) / element_width;
    float num_y_float = (top - bottom) / element_height;
    if(num_x_float < 0.0f) num_x_float = 0.0f;
    if(num_y_float < 0.0f) num_y_float = 0.0f;
    int num_x = int(num_x_float);
    int num_y = int(num_y_float);

    //if we need to create a tile that is smaller than a tile
    bool need_subtile_x = false;
    bool need_subtile_y = false;

    //truncate to remove decimal then compare with float again - if not whole tiles
    if(num_x_float > float(num_x)) {
        num_x++;
        need_subtile_x = true;
    }
    if(num_y_float > float(num_y)) {
        num_y++;
        need_subtile_y = true;
    }

    std::tuple<float,float,float,float> tile_vertex_bounds;
    for(int y = 0; y < num_y; y++) {
        for(int x = 0; x < num_x; x++) {
            float tile_left = std::get<0>(texture_bounds);
            float tile_right = std::get<1>(texture_bounds);
            float tile_top  = std::get<3>(texture_bounds);
            float tile_bottom = std::get<2>(texture_bounds);

            if(y+1 == num_y && need_subtile_y)  {
                //How much of the image do we need?
                float scale =  ((top - (bottom + element_height*float(y) ))/ element_height);
                tile_top = tile_bottom + scale * (tile_top - tile_bottom);
            }

            if(x+1 == num_x && need_subtile_x) {
                //How much of the image do we need?
                float scale = ((right - (left + element_width*float(x) ))/ element_width);
                tile_right = tile_left + scale *(tile_right - tile_left);

            }

            //Generate the vertex data
            offset = generate_texture_coords_element(data, offset,  std::make_tuple(tile_left, tile_right, tile_bottom, tile_top));
        }
    }

    return offset;
}

std::vector<std::pair<GLfloat*, int>> Button::generate_this_texture_data() {
    delete []texture_data;
    texture_data = nullptr;
    std::vector<std::pair<GLfloat*, int>> texture_coords;
    size_texture_data = 0;
    if(!is_visible())
        return texture_coords;

    float element_width_pixels = float(Engine::get_tile_size());
    float element_height_pixels = float(Engine::get_tile_size());
    //background
    float background_right = float(width_pixels) - element_width_pixels;
    //    if(background_right < element_width_pixels) background_right = element_width_pixels;
    float background_top = float(height_pixels) - element_height_pixels;
    //    if(background_top < element_height_pixels) background_top = element_height_pixels;
    //Generate the data buffer
    int num_floats_per_tile = 12;
    int total_floats = 0;
    int offset = 0;

    if(type == ButtonType::Board){

        std::tuple<float,float,float,float> background_bounds_vertex = std::make_tuple(element_width_pixels, background_right, background_top, element_height_pixels);
        std::tuple<float,float,float,float> edge_top_bounds_vertex = std::make_tuple(element_width_pixels, float(width_pixels) - element_width_pixels, float(height_pixels), float(height_pixels)  - element_height_pixels);
        std::tuple<float,float,float,float> edge_right_bounds_vertex = std::make_tuple(float(width_pixels) - element_width_pixels, float(width_pixels), float(height_pixels) - element_height_pixels,  element_height_pixels);
        std::tuple<float,float,float,float> edge_bottom_bounds_vertex = std::make_tuple(element_width_pixels, float(width_pixels) - element_width_pixels, float(element_height_pixels), 0.0f);
        std::tuple<float,float,float,float> edge_left_bounds_vertex = std::make_tuple(0.0f, float(element_width_pixels), float(height_pixels) - element_height_pixels, element_height_pixels);

        //Load data for texture coordinate bouds
        std::tuple<float,float,float,float> background_bounds = texture_atlas->index_to_coords(texture_atlas->get_name_index("gui/wood/background/1"));
        std::tuple<float,float,float,float> corner_top_left_bounds = texture_atlas->index_to_coords(texture_atlas->get_name_index("gui/wood/corner/topleft"));
        std::tuple<float,float,float,float> corner_top_right_bounds = texture_atlas->index_to_coords(texture_atlas->get_name_index("gui/wood/corner/topright"));
        std::tuple<float,float,float,float> corner_bottom_right_bounds = texture_atlas->index_to_coords(texture_atlas->get_name_index("gui/wood/corner/bottomright"));
        std::tuple<float,float,float,float> corner_bottom_left_bounds = texture_atlas->index_to_coords(texture_atlas->get_name_index("gui/wood/corner/bottomleft"));
        std::tuple<float,float,float,float> edge_top_bounds = texture_atlas->index_to_coords(texture_atlas->get_name_index("gui/wood/edge/top"));
        std::tuple<float,float,float,float> edge_right_bounds = texture_atlas->index_to_coords(texture_atlas->get_name_index("gui/wood/edge/right"));
        std::tuple<float,float,float,float> edge_bottom_bounds = texture_atlas->index_to_coords(texture_atlas->get_name_index("gui/wood/edge/bottom"));
        std::tuple<float,float,float,float> edge_left_bounds = texture_atlas->index_to_coords(texture_atlas->get_name_index("gui/wood/edge/left"));

        //get total number of floats
        total_floats += calculate_num_tile_elements(background_bounds_vertex, element_width_pixels, element_height_pixels) * num_floats_per_tile;
        total_floats += num_floats_per_tile * 4 ; // 4 corners
        total_floats += calculate_num_tile_elements(edge_top_bounds_vertex, element_width_pixels, element_height_pixels) * num_floats_per_tile;
        total_floats += calculate_num_tile_elements(edge_right_bounds_vertex, element_width_pixels, element_height_pixels) * num_floats_per_tile;
        total_floats += calculate_num_tile_elements(edge_bottom_bounds_vertex, element_width_pixels, element_height_pixels) * num_floats_per_tile;
        total_floats += calculate_num_tile_elements(edge_left_bounds_vertex, element_width_pixels, element_height_pixels) * num_floats_per_tile;

        texture_data = new GLfloat[total_floats];

        offset = generate_tile_element_texture_coords(texture_data, 0, background_bounds_vertex, element_width_pixels, element_height_pixels, background_bounds);
        offset =  generate_texture_coords_element(texture_data, offset, corner_top_left_bounds);
        offset =  generate_texture_coords_element(texture_data, offset, corner_top_right_bounds);
        offset =  generate_texture_coords_element(texture_data, offset, corner_bottom_right_bounds);
        offset =  generate_texture_coords_element(texture_data, offset, corner_bottom_left_bounds);
        offset =   generate_tile_element_texture_coords(texture_data, offset, edge_top_bounds_vertex, element_width_pixels, element_height_pixels, edge_top_bounds);
        offset =   generate_tile_element_texture_coords(texture_data, offset, edge_right_bounds_vertex, element_width_pixels, element_height_pixels, edge_right_bounds);
        offset =   generate_tile_element_texture_coords(texture_data, offset, edge_bottom_bounds_vertex, element_width_pixels, element_height_pixels, edge_bottom_bounds);
        offset =   generate_tile_element_texture_coords(texture_data, offset, edge_left_bounds_vertex, element_width_pixels, element_height_pixels, edge_left_bounds);

    }
    else if(type== ButtonType::Single){
        //Load data for texture coordinate bouds
        std::tuple<float,float,float,float> background_bounds = texture_atlas->index_to_coords(texture_atlas->get_name_index(picture_name));

        //get total number of floats
        total_floats += num_floats_per_tile;

        texture_data = new GLfloat[total_floats];

        offset =  generate_texture_coords_element(texture_data, 0, background_bounds);
    }
    else{
        //it is of type NoPicture, do nothing
    }
    size_texture_data = offset;

    texture_coords.push_back(std::make_pair(texture_data, total_floats));
    return texture_coords;
}

int Button::generate_vertex_coords_element(GLfloat* data, int offset, std::tuple<float,float,float,float> bounds) {
    //bottom left
    data[offset]  = std::get<0>(bounds);
    data[offset + 1]  = std::get<2>(bounds);

    //top left
    data[offset + 2]  = std::get<0>(bounds);
    data[offset + 3]  = std::get<3>(bounds);

    //bottom right
    data[offset + 4]  = std::get<1>(bounds);
    data[offset + 5]  = std::get<2>(bounds);

    //top left
    data[offset + 6]  = std::get<0>(bounds);
    data[offset + 7]  = std::get<3>(bounds);

    //top right
    data[offset + 8]  = std::get<1>(bounds);
    data[offset + 9]  = std::get<3>(bounds);

    //bottom right
    data[offset + 10] = std::get<1>(bounds);
    data[offset + 11] = std::get<2>(bounds);

    return offset + 12;
}
int Button::generate_texture_coords_element(GLfloat* data, int offset, std::tuple<float,float,float,float> bounds) {
    //tuple is;
    //left right top bottom
    //bottom left
    data[offset]  = std::get<0>(bounds);
    data[offset + 1]  = std::get<3>(bounds);

    //top left
    data[offset + 2]  = std::get<0>(bounds);
    data[offset + 3]  = std::get<2>(bounds);

    //bottom right
    data[offset + 4]  = std::get<1>(bounds);
    data[offset + 5]  = std::get<3>(bounds);

    //top left
    data[offset + 6]  = std::get<0>(bounds);
    data[offset + 7]  = std::get<2>(bounds);

    //top right
    data[offset + 8]  = std::get<1>(bounds);
    data[offset + 9]  = std::get<2>(bounds);

    //bottom right
    data[offset + 10] = std::get<1>(bounds);
    data[offset + 11] = std::get<3>(bounds);


    return offset + 12;
}


std::vector<std::shared_ptr<GUIText>> Button::generate_this_text_data() {
    std::vector<std::shared_ptr<GUIText>> text_data;
    if(!is_visible())
        return text_data;


    text_data.push_back(button_text);
    button_text->get_gui_text()->set_transformed_x_offset(0);
    button_text->get_gui_text()->set_transformed_y_offset(0);
    return text_data;
}
