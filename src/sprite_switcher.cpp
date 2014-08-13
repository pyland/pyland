#include <glog/logging.h>

#include "dispatcher.hpp"
#include "engine.hpp"
#include "sprite_switcher.hpp"
#include "map.hpp"
#include "map_viewer.hpp"
#include "gui_window.hpp"
#include "component_group.hpp"

float sprite_button_size = 0.05f;
float button_space = 0.02f;
int button_num = 0;
float y_offset = 0.7f;
float x_offset = 0.1f;

void SpriteSwitcher::add_sprite(int id) {
    // build button
    std::shared_ptr<Button> new_button = std::make_shared<Button>();
    new_button->set_text("TEXT");
    new_button->set_on_click([id] () {
        LOG(INFO) << "switch sprite focus to " << id;
        Engine::get_map_viewer()->set_map_focus_object(id);
    });
    float y_location = y_offset - (float)button_num * (sprite_button_size + button_space);
    new_button->set_width(sprite_button_size);
    new_button->set_height(sprite_button_size);
    new_button->set_y_offset(y_location);
    new_button->set_x_offset(x_offset);

    // store button locally for later extensions
    switcher_buttons.push_back(new_button);

    button_num++;

    // add button to GUI
    GUIManager* gui_manager = Engine::get_map_viewer()->get_gui_manager();
    gui_manager->get_root()->add(new_button);
    gui_manager->parse_components();

}

SpriteSwitcher::SpriteSwitcher() {
    switcher_buttons = std::vector<std::shared_ptr<Button>>();
    Engine::get_map_viewer()->get_map()->event_sprite_add.register_callback(
        [&] (int id) {
            add_sprite(id);
            return true;
        }
    );
}