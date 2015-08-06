#include <functional>
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

TextBox::TextBox() {

    TextFont buttonfont = Engine::get_game_font();

    /// build back button
    backward_button = std::make_shared<Button>(ButtonType::Single);
    backward_button->set_text("<- Previous");
    backward_button->set_on_click([&] () {
        LOG(INFO) << "backward button pressed";
        traverse_text(Direction::PREVIOUS);
    });

    //build forwards button
    forward_button = std::make_shared<Button>(ButtonType::Single);
    forward_button->set_text("Next ->");
    forward_button->set_on_click([&] () {
        LOG(INFO) << "forward button pressed";
        traverse_text(Direction::NEXT);
    });

    this->add(backward_button);
    this->add(forward_button);

    // text object for notifications
    text = std::make_shared<GUIText>();

    set_text("");

    get_text()->set_bloom_radius(6);

    add(text);

//    text = new Text(window, Engine::get_game_font(), true);
//	text->set_bloom_radius(6);
//    text->set_text("Welcome to Project Zygote");
    // referring to top left corner of text window
//	notification_text->move(text_border_width, text_border_width);
//    auto window_size = window->get_size();
//    notification_text->resize(window_size.first-text_border_width, 0);
//    notification_text->align_at_origin(true);
//    notification_text->align_left();
//    notification_text->vertical_align_bottom();

    // callback to resize text when window size changes
//    std::function<void (GameWindow *)> func = [&] (GameWindow *game_window) {
//        LOG(INFO) << "text window resizing";
//        auto window_size = (*game_window).get_size();
//        notification_text->resize(window_size.first-text_border_width, 0);
//    };
//
//    text_box = (window->register_resize_handler(func));
//
//    notification_stack = Notification();

}

void TextBox::set_text(std::string _text) {
    TextFont font = Engine::get_game_font();

    std::shared_ptr<Text> new_text = std::make_shared<Text>(Engine::get_game_window(), font, true);
    new_text->set_text(_text);
    text->set_text(new_text);
    get_text()->set_bloom_radius(6);
}

std::shared_ptr<Text> TextBox::get_text() {
    return text->get_text();
}

void TextBox::set_text(std::shared_ptr<Text> _text) {

    text->set_text(_text);
    get_text()->set_bloom_radius(6);
}

void TextBox::resize_text(float width, float height){
    text->set_width(width);
    text->set_height(height);
}

void TextBox::move_text(float x_offset, float y_offset){
    text->set_x_offset(x_offset);
    text->set_y_offset(y_offset);
}

void TextBox::traverse_text(Direction direction) {
    set_text(
        direction == Direction::NEXT ? text_stack.forward()
                                     : text_stack.backward()
    );
    hide_buttons();
}

void TextBox::add_message(std::string text_to_display) {
    text_stack.add_new(text_to_display);
    EventManager::get_instance()->add_event(
        [=] () {
            set_text(text_to_display);
            //std::cout << text_to_display << std::endl;
        }
    );
    hide_buttons();
 }

 void TextBox::hide_buttons() {
    forward_button->set_visible(text_stack.can_forward);
    backward_button->set_visible(text_stack.can_backward);
//    Engine::get_map_viewer()->get_gui_manager()->parse_components();

 }

TextBox::~TextBox() {
//    GUIManager* gui_manager = Engine::get_map_viewer()->get_gui_manager();
//    CHECK_NOTNULL(gui_manager);
//    gui_manager->get_root()->remove(backward_button->get_id());
//    gui_manager->get_root()->remove(forward_button->get_id());

}

void TextBox::clear_text() {
    set_text("");
    text_stack.clear();
}

std::vector<std::pair<GLfloat*, int>> TextBox::generate_this_vertex_data() {
    delete []vertex_data;
    vertex_data = nullptr;
    std::vector<std::pair<GLfloat*, int>> vertices;
    size_vertex_data = 0;
    if(!is_visible())
        return vertices;

    int total_floats = 0;

    size_vertex_data = 0;

    vertices.push_back(std::make_pair(vertex_data, total_floats));
    return vertices;
}

std::vector<std::pair<GLfloat*, int>> TextBox::generate_this_texture_data() {
    delete []texture_data;
    texture_data = nullptr;
    std::vector<std::pair<GLfloat*, int>> texture_coords;
    size_texture_data = 0;
    if(!is_visible())
        return texture_coords;

    int total_floats = 0;
    int offset = 0;

    size_texture_data = offset;

    texture_coords.push_back(std::make_pair(texture_data, total_floats));
    return texture_coords;
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
