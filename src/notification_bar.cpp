#include <functional>
#include <glog/logging.h>
#include <ostream>
#include <utility>

#include "component_group.hpp"
#include "engine.hpp"
#include "event_manager.hpp"
#include "game_window.hpp"
#include "gui_manager.hpp"
#include "object_manager.hpp"
#include "map_viewer.hpp"
#include "notification_bar.hpp"
#include "text.hpp"
#include "text_font.hpp"


// configuration of notification bar
float button_size = 0.2f;
std::pair<float,float> backward_loco(0.8f,0.03f);
std::pair<float,float> forward_loco(0.8f,0.2f);

int text_border_width = 20;

NotificationBar::NotificationBar() {

    GameWindow* window = Engine::get_map_viewer()->get_window();
    CHECK_NOTNULL(window);
    TextFont notification_buttonfont = Engine::get_game_font();

    /// build back button
    backward_button = std::make_shared<Button>();
    backward_button->set_text("<- Previous");
    backward_button->set_on_click([&] () {
        LOG(INFO) << "backward button pressed";
        move_notification(Direction::PREVIOUS);
    });
    backward_button->set_width(button_size);
    backward_button->set_height(button_size);
    backward_button->set_y_offset(backward_loco.second);
    backward_button->set_x_offset(backward_loco.first);

    //build forwards button
    forward_button = std::make_shared<Button>();
    forward_button->set_text("Next ->");
    forward_button->set_on_click([&] () {
        LOG(INFO) << "forward button pressed";
        move_notification(Direction::NEXT);
    });
    forward_button->set_width(button_size);
    forward_button->set_height(button_size);
    forward_button->set_y_offset(forward_loco.second);
    forward_button->set_x_offset(forward_loco.first);


    GUIManager* gui_manager = Engine::get_map_viewer()->get_gui_manager();
    CHECK_NOTNULL(gui_manager);
    gui_manager->get_root()->add(backward_button);
    gui_manager->get_root()->add(forward_button);
    gui_manager->parse_components();

    // text object for notifications
    notification_text = new Text(window, Engine::get_game_font(), true);
    notification_text->set_bloom_radius(6);
    notification_text->set_text("Welcome to Project Zygote");
    // referring to top left corner of text window
    notification_text->move(text_border_width, text_border_width);
    auto window_size = window->get_size();
    notification_text->resize(window_size.first-text_border_width, 0);
    notification_text->align_at_origin(true);
    notification_text->align_left();
    notification_text->vertical_align_bottom();

    // callback to resize text when window size changes
    std::function<void (GameWindow *)> func = [&] (GameWindow *game_window) {
        LOG(INFO) << "text window resizing";
        auto window_size = (*game_window).get_size();
        notification_text->resize(window_size.first-text_border_width, 0);
    };

    text_box = (window->register_resize_handler(func));

    notification_stack = Notification();

}

void NotificationBar::text_displayer() {
    notification_text->display();

}

void NotificationBar::move_notification(Direction direction) {
    notification_text->set_text(
        direction == Direction::NEXT ? notification_stack.forward()
                                     : notification_stack.backward()
    );
    hide_buttons();
}

void NotificationBar::add_notification(std::string text_to_display) {
    notification_stack.add_new(text_to_display);
    EventManager::get_instance()->add_event(
        [=] () {
            notification_text->set_text(text_to_display);
            //std::cout << text_to_display << std::endl;
        }
    );
    hide_buttons();
 }

 void NotificationBar::hide_buttons() {
    forward_button->set_visible(notification_stack.can_forward);
    backward_button->set_visible(notification_stack.can_backward);
    Engine::get_map_viewer()->get_gui_manager()->parse_components();

 }

 NotificationBar::~NotificationBar() {
    GUIManager* gui_manager = Engine::get_map_viewer()->get_gui_manager();
    CHECK_NOTNULL(gui_manager);
    gui_manager->get_root()->remove(backward_button->get_id());
    gui_manager->get_root()->remove(forward_button->get_id());
    delete notification_text;
}

void NotificationBar::clear_text() {
    notification_text->set_text("");
    notification_stack.clear();
}
