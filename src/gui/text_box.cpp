#include <functional>
#include <cctype>
#include <glog/logging.h>
#include <ostream>
#include <utility>

#include "text_box.hpp"

#include "component_group.hpp"
#include "engine.hpp"
#include "event_manager.hpp"
#include "game_window.hpp"
#include "text.hpp"
#include "text_font.hpp"
#include "texture_atlas.hpp"

TextBox::TextBox(TextBoxType _type) {

    type = _type;
    buffer_size = 100;
    TextFont buttonfont = Engine::get_game_font();

    //build forwards button
    forward_button = std::make_shared<Button>(ButtonType::Single);
    forward_button->set_visible(text_stack.can_forward());
    forward_button->set_alignment(ButtonAlignment::BottomLeft);

    backward_button = std::make_shared<Button>(ButtonType::Single);
    backward_button->set_visible(text_stack.can_backward());
    backward_button->set_alignment(ButtonAlignment::BottomLeft);

    if(type == TextBoxType::Bar){
        forward_button->move_text(0.0f, 0.0f);
        forward_button->set_picture("gui/status/running");

        forward_button->set_on_click([&] () {
            LOG(INFO) << "forward button pressed";

            if(text_stack.can_forward()){
                traverse_text(Direction::NEXT);

                if(!text_stack.can_forward()){
                    forward_button->set_picture("gui/status/failed");
                }
                Engine::get_gui()->refresh_gui();
            }
            else{
                hide_buttons();
                Engine::close_notification_bar();
            }
        });

        this->add(forward_button);
    }
    else if(type == TextBoxType::Display){
        forward_button->move_text(0.0f, 0.0f);
        forward_button->set_picture("gui/status/running");

        forward_button->set_on_click([&] () {
            LOG(INFO) << "forward button pressed";

            if(text_stack.can_forward()){
                traverse_text(Direction::NEXT);

                if(!text_stack.can_forward()){
                    forward_button->set_picture("gui/status/failed");
                }
                else if(text_stack.can_forward()){
                    forward_button->set_picture("gui/status/running");
                }

                if(!text_stack.can_backward()){
                    backward_button->set_picture("gui/status/failed");
                }
                else if(text_stack.can_backward()){
                    backward_button->set_picture("gui/status/running");
                }
                Engine::get_gui()->refresh_gui();
            }
        });

        backward_button->move_text(0.0f, 0.0f);
        backward_button->set_picture("gui/status/running");

        backward_button->set_on_click([&] () {
            LOG(INFO) << "backward button pressed";

            if(text_stack.can_backward()){
                traverse_text(Direction::PREVIOUS);

                if(!text_stack.can_backward()){
                    backward_button->set_picture("gui/status/failed");
                }
                else if(text_stack.can_backward()){
                    backward_button->set_picture("gui/status/running");
                }

                if(!text_stack.can_forward()){
                    forward_button->set_picture("gui/status/failed");
                }
                else if(text_stack.can_forward()){
                    forward_button->set_picture("gui/status/running");
                }

                Engine::get_gui()->refresh_gui();
            }
        });

        this->add(forward_button);
        this->add(backward_button);
    }

    // text object for notifications
    text = std::make_shared<GUIText>();
    text->set_width(1.0f);
    text->set_height(1.0f);
    text->set_x_offset(0.0f);
    text->set_y_offset(0.0f);

    set_text("");
    get_text()->set_bloom_radius(6);

    add(text);
    Engine::get_gui()->refresh_gui();
}

void TextBox::open(){

    this->set_visible(true);

    if(type == TextBoxType::Bar){
        forward_button->set_visible(true);
        forward_button->set_clickable(true);

        if(!text_stack.can_forward()){
            forward_button->set_picture("gui/status/failed");
        }
    }
    else if(type == TextBoxType::Display){
        forward_button->set_visible(true);
        forward_button->set_clickable(true);

        backward_button->set_visible(true);
        backward_button->set_clickable(true);

        if(!text_stack.can_forward()){
            forward_button->set_picture("gui/status/failed");
        }
        if(!text_stack.can_backward()){
            backward_button->set_picture("gui/status/failed");
        }
    }

    this->set_text(text_stack.present());
    Engine::get_gui()->refresh_gui();
}

void TextBox::proceed(){
    forward_button->call_on_click();
}

void TextBox::close(){

    hide_buttons();

    this->set_visible(false);
    Engine::get_gui()->refresh_gui();
}

void TextBox::traverse_text(Direction direction) {
    this->set_text(
        direction == Direction::NEXT ? text_stack.forward()
                                     : text_stack.backward()
    );
    Engine::get_gui()->refresh_gui();
}

void TextBox::add_message(std::string text_to_display) {
    text_stack.add_new(text_to_display);
}


void TextBox::resize_buttons(float width, float height){

    if(type == TextBoxType::Bar){
        forward_button->set_width(width);
        forward_button->set_height(height);
    }
    else if(type == TextBoxType::Display){
        forward_button->set_width(width);
        forward_button->set_height(height);
        backward_button->set_width(width);
        backward_button->set_height(height);
    }
}

void TextBox::move_buttons(float x_offset, float y_offset, float spacing /* = 0.0f */){

    if(type == TextBoxType::Bar){
        forward_button->set_x_offset(x_offset);
        forward_button->set_y_offset(y_offset);
    }
    else if(type == TextBoxType::Display){
        forward_button->set_x_offset(x_offset);
        forward_button->set_y_offset(y_offset);
        backward_button->set_x_offset(x_offset - spacing);
        backward_button->set_y_offset(y_offset);
    }
}

void TextBox::hide_buttons(){

    if(type == TextBoxType::Bar){
        forward_button->set_visible(false);
        forward_button->set_clickable(false);
    }
    else if(type == TextBoxType::Display){
        forward_button->set_visible(false);
        forward_button->set_clickable(false);
        backward_button->set_visible(false);
        backward_button->set_clickable(false);
    }
}

void TextBox::add_text(std::string text){

    for(unsigned int i=0; i <text.length(); i += buffer_size){
        std::string part = text.substr(i, buffer_size);

        if(i+buffer_size >= text.length()){
            part = part; //end of text
        }
        else if(isalnum(text[i]) && isalnum(text[i+1])){
            part = part + "-";
        }
        else{
            part = part + "...";
        }

        this->add_message(part);
    }
}

void TextBox::clear_text() {
    set_text("");
    text_stack.clear();
    Engine::get_gui()->refresh_gui();
}

void TextBox::set_text(std::string _text) {
    TextFont font = Engine::get_game_font();

    std::shared_ptr<Text> new_text = std::make_shared<Text>(Engine::get_game_window(), font, true);
    new_text->set_text(_text);
    text->set_text(new_text);
    get_text()->set_bloom_radius(6);
    Engine::get_gui()->refresh_gui();
}

std::shared_ptr<Text> TextBox::get_text() {
    return text->get_text();
}

void TextBox::set_text(std::shared_ptr<Text> _text) {

    text->set_text(_text);
    get_text()->set_bloom_radius(6);
    Engine::get_gui()->refresh_gui();
}

void TextBox::resize_text(float width, float height){
    text->set_width(width);
    text->set_height(height);
    Engine::get_gui()->refresh_gui();
}

void TextBox::move_text(float x_offset, float y_offset){
    text->set_x_offset(x_offset);
    text->set_y_offset(y_offset);
    Engine::get_gui()->refresh_gui();
}

std::vector<std::pair<GLfloat*, int>> TextBox::generate_this_vertex_data() {
    delete []vertex_data;
    vertex_data = nullptr;
    std::vector<std::pair<GLfloat*, int>> vertices;
    size_vertex_data = 0;
    if(!is_visible())
        return vertices;

    int total_floats = 0;

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
    int offset = 0;

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

    size_vertex_data = offset;

    vertices.push_back(std::make_pair(vertex_data, total_floats));
    return vertices;
}

int TextBox::generate_tile_element_vertex_coords(GLfloat* data, int offset, std::tuple<float,float,float,float> bounds, float element_width, float element_height) {

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

int TextBox::generate_tile_element_texture_coords(GLfloat* data, int offset, std::tuple<float,float,float,float> vertex_bounds, float element_width, float element_height, std::tuple<float,float,float,float> texture_bounds) {

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

int TextBox::calculate_num_tile_elements(std::tuple<float,float,float,float> bounds, float element_width, float element_height) {

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

std::vector<std::pair<GLfloat*, int>> TextBox::generate_this_texture_data() {
    delete []texture_data;
    texture_data = nullptr;
    std::vector<std::pair<GLfloat*, int>> texture_coords;
    size_texture_data = 0;
    if(!is_visible()) return texture_coords;

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

    std::tuple<float,float,float,float> background_bounds_vertex = std::make_tuple(element_width_pixels, background_right, background_top, element_height_pixels);
    std::tuple<float,float,float,float> edge_top_bounds_vertex = std::make_tuple(element_width_pixels, float(width_pixels) - element_width_pixels, float(height_pixels), float(height_pixels)  - element_height_pixels);
    std::tuple<float,float,float,float> edge_right_bounds_vertex = std::make_tuple(float(width_pixels) - element_width_pixels, float(width_pixels), float(height_pixels) - element_height_pixels,  element_height_pixels);
    std::tuple<float,float,float,float> edge_bottom_bounds_vertex = std::make_tuple(element_width_pixels, float(width_pixels) - element_width_pixels, float(element_height_pixels), 0.0f);
    std::tuple<float,float,float,float> edge_left_bounds_vertex = std::make_tuple(0.0f, float(element_width_pixels), float(height_pixels) - element_height_pixels, element_height_pixels);

    //Load data for texture coordinate bouds
    std::tuple<float,float,float,float> background_bounds = texture_atlas->index_to_coords(texture_atlas->get_name_index("gui/scroll/background/1"));
    std::tuple<float,float,float,float> corner_top_left_bounds = texture_atlas->index_to_coords(texture_atlas->get_name_index("gui/scroll/corner/topleft"));
    std::tuple<float,float,float,float> corner_top_right_bounds = texture_atlas->index_to_coords(texture_atlas->get_name_index("gui/scroll/corner/topright"));
    std::tuple<float,float,float,float> corner_bottom_right_bounds = texture_atlas->index_to_coords(texture_atlas->get_name_index("gui/scroll/corner/bottomright"));
    std::tuple<float,float,float,float> corner_bottom_left_bounds = texture_atlas->index_to_coords(texture_atlas->get_name_index("gui/scroll/corner/bottomleft"));
    std::tuple<float,float,float,float> edge_top_bounds = texture_atlas->index_to_coords(texture_atlas->get_name_index("gui/scroll/edge/top"));
    std::tuple<float,float,float,float> edge_right_bounds = texture_atlas->index_to_coords(texture_atlas->get_name_index("gui/scroll/edge/right"));
    std::tuple<float,float,float,float> edge_bottom_bounds = texture_atlas->index_to_coords(texture_atlas->get_name_index("gui/scroll/edge/bottom"));
    std::tuple<float,float,float,float> edge_left_bounds = texture_atlas->index_to_coords(texture_atlas->get_name_index("gui/scroll/edge/left"));

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

    size_texture_data = offset;

    texture_coords.push_back(std::make_pair(texture_data, total_floats));
    return texture_coords;
}

int TextBox::generate_vertex_coords_element(GLfloat* data, int offset, std::tuple<float,float,float,float> bounds) {
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
int TextBox::generate_texture_coords_element(GLfloat* data, int offset, std::tuple<float,float,float,float> bounds) {
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

std::vector<std::shared_ptr<GUIText>> TextBox::generate_this_text_data() {
    std::vector<std::shared_ptr<GUIText>> text_data;
    if(!is_visible())
        return text_data;

    text_data.push_back(text);
    text->get_gui_text()->set_transformed_x_offset(0);
    text->get_gui_text()->set_transformed_y_offset(0);
    return text_data;
}

TextBox::~TextBox() {

}
