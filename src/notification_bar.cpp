#include <glog/logging.h>

#include "engine.hpp"
#include "game_window.hpp"
#include "map_viewer.hpp"
#include "notification_bar.hpp"

// configuration of notification bar
float button_size = 0.05f;
std::pair<float,float> backward_loco(0.85f,0.05f);
std::pair<float,float> forward_loco(0.95f,0.05f);

int text_border_width = 20;
int text_height = 80;

NotificationBar::NotificationBar() {

    GameWindow* window = Engine::get_map_viewer()->get_window();
    CHECK_NOTNULL(window);
    TextFont notification_buttonfont = Engine::get_game_font();

    /// build back button
    std::shared_ptr<Button> backward_button = std::make_shared<Button>();
    backward_button->set_text("backward");
    backward_button->set_on_click([&] () {
        LOG(INFO) << "backward button pressed";
        move_notification(Direction::PREVIOUS);
    });
    backward_button->set_width(button_size);
    backward_button->set_height(button_size);
    backward_button->set_y_offset(backward_loco.second);
    backward_button->set_x_offset(backward_loco.first);
    backward_button_id = backward_button->get_id();

    //build forwards button
    std::shared_ptr<Button> forward_button = std::make_shared<Button>();
    forward_button->set_text("forward");
    forward_button->set_on_click([&] () {
        LOG(INFO) << "forward button pressed";
        move_notification(Direction::NEXT);
    });
    forward_button->set_width(button_size);
    forward_button->set_height(button_size);
    forward_button->set_y_offset(forward_loco.second);
    forward_button->set_x_offset(forward_loco.first);
    forward_button_id = forward_button->get_id();

    GUIManager* gui_manager = Engine::get_map_viewer()->get_gui_manager();
    CHECK_NOTNULL(gui_manager);
    gui_manager->get_root()->add(backward_button);
    gui_manager->get_root()->add(forward_button);
    gui_manager->parse_components();

    // button text as Button::set_text() don't currently work
    backward_text.reset(new Text(window, notification_buttonfont, true));
    backward_text->set_text("<-");
    backward_text->move_ratio(backward_loco.first, backward_loco.second);
    backward_text->resize_ratio(button_size,button_size);

    forward_text.reset(new Text(window, notification_buttonfont, true));
    forward_text->set_text("->");
    forward_text->move_ratio(forward_loco.first,forward_loco.second);
    forward_text->resize_ratio(button_size,button_size);

    // text object for notifications
    notification_text = new Text(window, Engine::get_game_font(), true);
    notification_text->set_text("Welcome to Project Zygote");
    // referring to top left corner of text window
    notification_text->move(text_border_width, text_height + text_border_width);
    auto window_size = window->get_size();
    notification_text->resize(window_size.first-text_border_width, text_height + text_border_width);

    // callback to resize text when window size changes
    std::function<void (GameWindow *)> func = [&] (GameWindow *game_window) {
        LOG(INFO) << "text window resizing";
        auto window_size = (*game_window).get_size();
        notification_text->resize(window_size.first-text_border_width, text_height + text_border_width);
    };

    text_box = (window->register_resize_handler(func));

    notification_stack = Notification();

}

void NotificationBar::text_displayer() {
    notification_text->display();
    backward_text->display();
    forward_text->display();

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
    EventManager::get_instance().add_event(
        [=] () {
            notification_text->set_text(text_to_display);
            //std::cout << text_to_display << std::endl;
        }
    );
    hide_buttons();
 }

 void NotificationBar::hide_buttons(){
    // TODO: this is a hack until I can hide buttons

    if (notification_stack.can_forward) {
        forward_text->set_text("->");
    } else {
        forward_text->set_text("");
    }

    if (notification_stack.can_backward) {
        backward_text->set_text("<-");
    } else {
        backward_text->set_text("");
    }

 }

 NotificationBar::~NotificationBar() {
    GUIManager* gui_manager = Engine::get_map_viewer()->get_gui_manager();
    CHECK_NOTNULL(gui_manager);
    gui_manager->get_root()->remove(backward_button_id);
    gui_manager->get_root()->remove(forward_button_id);
}
>>>>>>> master
