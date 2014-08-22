#include <glog/logging.h>
#include <memory>
#include <ostream>

#include "button.hpp"
#include "component_group.hpp"
#include "dispatcher.hpp"
#include "engine.hpp"
#include "gui_manager.hpp"
#include "object_manager.hpp"
#include "map.hpp"
#include "map_viewer.hpp"
#include "sprite.hpp"
#include "sprite_switcher.hpp"


float sprite_button_size = 0.2f;
int button_num = 0;
float y_offset = 0.8f;
float x_offset = 0.8f;

void SpriteSwitcher::add_sprite(int id) {
    // get sprite_name
    std::string name = ObjectManager::get_instance().get_object<Sprite>(id)->get_sprite_name();

    // build button
    std::shared_ptr<Button> new_button = std::make_shared<Button>();
    new_button->set_text(name);
    new_button->set_on_click([id] () {
        LOG(INFO) << "switch sprite focus to " << id;
        Engine::get_map_viewer()->set_map_focus_object(id);
    });
    float y_location = y_offset - (float)button_num * 0.13f;
    new_button->set_width(sprite_button_size);
    new_button->set_height(sprite_button_size);
    new_button->set_y_offset(y_location);
    new_button->set_x_offset(x_offset);

    // store button locally for later extensions
    switcher_buttons.push_back(new_button->get_id());

    button_num++;

    // add button to GUI
    GUIManager* gui_manager = Engine::get_map_viewer()->get_gui_manager();
    gui_manager->get_root()->add(new_button);
    gui_manager->parse_components();

}

SpriteSwitcher::SpriteSwitcher() {
    switcher_buttons = std::vector<int>();
    Engine::get_map_viewer()->get_map()->event_sprite_add.register_callback(
        [&] (int id) {
            add_sprite(id);
            return true;
        }
    );
}
SpriteSwitcher::~SpriteSwitcher() {
    button_num = 0;
    GUIManager* gui_manager = Engine::get_map_viewer()->get_gui_manager();
    CHECK_NOTNULL(gui_manager);
    for (int button_id: switcher_buttons) {
        gui_manager->get_root()->remove(button_id);
    }
}
