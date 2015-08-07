#include "gui_main.hpp"

#include "engine.hpp"
#include "button.hpp"
#include "config.hpp"
#include "gui_manager.hpp"
#include "gui_window.hpp"
#include "map_viewer.hpp"
#include "text_box.hpp"

GUIMain::GUIMain(GameWindow * _embedWindow):
    embedWindow(_embedWindow),
    gui_manager(),
    map_viewer(embedWindow, &gui_manager),
    em(EventManager::get_instance()),
    bag_open(false),
    pyguide_open(false),
    display_button_start(0)

{
    LOG(INFO) << "Constructing GUIMain...";

    config_gui();

    //Create the game embedWindow to present to the users
    embedWindow->use_context();
    Engine::set_game_window(embedWindow);

    Engine::set_map_viewer(&map_viewer);

    gui_window = std::make_shared<GUIWindow>();
    gui_window->set_visible(false);

    gui_manager.set_root(gui_window);

    notification_bar = std::make_shared<TextBox>();

    Engine::set_notification_bar(notification_bar);

    pause_button = std::make_shared<Button>(ButtonType::Single);
    pause_button->set_picture("gui/game/pause");
    pause_button->set_alignment(ButtonAlignment::TopLeft);
    pause_button->set_width(button_width);
    pause_button->set_height(button_height);
    pause_button->set_x_offset(left_x_offset);
    pause_button->set_y_offset(top_y_offset);
    pause_button->set_on_click( [&] () {

        if(em->is_paused() == false){
            em->pause();
            LOG(INFO) << "PAUSED";
            open_pause_window();
        }
        else{
            em->resume();
            LOG(INFO) << "RESUMED";
            close_pause_window();
        }
    });

    bag_button = std::make_shared<Button>(ButtonType::Single);
    bag_button->set_picture("gui/game/bag");
    bag_button->set_text("Bag");
    bag_button->set_alignment(ButtonAlignment::TopRight);
    bag_button->set_width(button_width);
    bag_button->set_height(button_height);
    bag_button->set_x_offset(right_x_offset);
    bag_button->set_y_offset(top_y_offset);

    bag_button->set_on_click( [&] () {

        if(bag_open == false){
            bag_open = true;
            open_bag();
        }
        else{
            bag_open = false;
            close_bag();
        }
    });

    bag_window = std::make_shared<Button>(ButtonType::Board);
    bag_window->set_text("Bag");
    bag_window->set_clickable(false);
    bag_window->set_visible(false);
    bag_window->move_text(title_x_offset, title_y_offset);

    pyguide_window = std::make_shared<Button>(ButtonType::Board);
    pyguide_window->set_text("PyGuide");
    pyguide_window->set_clickable(false);
    pyguide_window->set_visible(false);
    pyguide_window->move_text(title_x_offset, title_y_offset);

    pyguide_button = std::make_shared<Button>(ButtonType::NoPicture);
    pyguide_button->set_text("PyGuide");
    pyguide_button->set_alignment(ButtonAlignment::BottomLeft);
    pyguide_button->set_width(button_width);
    pyguide_button->set_height(button_height);
    pyguide_button->set_x_offset(menu_x_offset);
    pyguide_button->set_y_offset(menu_y_offset);
    pyguide_button->set_visible(false);

    pyguide_button->set_on_click( [&] () {
        pyguide_open = true;
        open_pyguide();
    });

    bag_window->add(pyguide_button);

    gui_window->add(pause_button);
    gui_window->add(bag_button);
    gui_window->add(bag_window);
    gui_window->add(pyguide_window);

    LOG(INFO) << "Constructed GUIMain.";
}

void GUIMain::open_pyguide(){

    close_bag();
    pyguide_window->set_visible(true);

    refresh_gui();
    LOG(INFO) << "PyGuide opened";
}

void GUIMain::close_pyguide(){

    pyguide_window->set_visible(false);

    refresh_gui();
    LOG(INFO) << "PyGuide closed";
}

void GUIMain::open_bag(){

    bag_window->set_visible(true);

    close_pyguide();
    pyguide_open = false;

    pyguide_button->set_visible(true);
    pyguide_button->set_clickable(true);

    for(unsigned int i=0; i!=bag_items.size(); i++){
        bag_items[i]->set_visible(true);
        bag_items[i]->set_clickable(true);
    }

    refresh_gui();
    LOG(INFO) << "Bag opened";
}

void GUIMain::close_bag(){

    bag_window->set_visible(false);

    close_pyguide();
    pyguide_open = false;

    pyguide_button->set_visible(false);
    pyguide_button->set_clickable(false);

    for(unsigned int i=0; i!=bag_items.size(); i++){
        bag_items[i]->set_visible(false);
        bag_items[i]->set_clickable(false);
    }

    refresh_gui();
    LOG(INFO) << "Bag closed";
}

void GUIMain::open_pause_window(){
    gui_window->set_visible(true);

    const std::map<int, std::shared_ptr<Component>>* gui_components = gui_window->get_components();

    typedef std::map<int, std::shared_ptr<Component>>::const_iterator it_type;

    for(it_type i = gui_components->begin(); i !=gui_components->end(); ++i){
        if(i->second == pause_button){
            continue;
        }
        else{
            i->second->set_visible(false);
            i->second->set_clickable(false);
        }
        bag_open = false;
        pyguide_open = false;
    }

    refresh_gui();
}

void GUIMain::close_pause_window(){
    gui_window->set_visible(false);

    const std::map<int, std::shared_ptr<Component>>* gui_components = gui_window->get_components();

    typedef std::map<int, std::shared_ptr<Component>>::const_iterator it_type;

    for(it_type i = gui_components->begin(); i !=gui_components->end(); ++i){
        if(i->second == bag_window){
            continue;
        }
        else if(i->second == pyguide_window){
            continue;
        }
        else{
            i->second->set_visible(true);
            i->second->set_clickable(true);
        }
    }

    refresh_gui();
}

void GUIMain::add_button(std::string file_path, std::string name, std::function<void (void)> callback){

    if(buttons.size() == button_max){
        cycle_button = std::make_shared<Button>(ButtonType::Single);
        cycle_button->set_picture("gui/game/buttons/cycle");
        cycle_button->set_text("Cycle");
        cycle_button->set_width(button_width);
        cycle_button->set_height(button_height);
        cycle_button->set_x_offset(right_x_offset - float(button_max + 1) * horizontal_button_spacing);
        cycle_button->set_y_offset(top_y_offset);

        cycle_button->set_on_click( [&] () {

            //remove previous set of buttons
            for(unsigned int i=0; i<button_max && display_button_start + i < buttons.size(); i++)
            {
                gui_window->remove(buttons[display_button_start + i]->get_id());
            }

            //update the button index of buttons to be displayed
            if(display_button_start + button_max >= buttons.size()){
                display_button_start = 0;
            }
            else{
                display_button_start += button_max;
            }

            //display new buttons
            for(unsigned int i=0; i<button_max && display_button_start + i < buttons.size(); i++)
            {
                gui_window->add(buttons[display_button_start + i]);
            }

            refresh_gui();
        });

        gui_window->add(cycle_button);

        refresh_gui();
    }

    std::shared_ptr<Button> new_button;
    new_button = std::make_shared<Button>(ButtonType::Single);
    buttons.push_back(new_button);
    new_button->set_picture(file_path);
    new_button->set_text(name);
    new_button->set_on_click(callback);
    new_button->set_width(button_width);
    new_button->set_height(button_height);

    //make space for previous buttons
    float org_x_location = right_x_offset;

    new_button->set_x_offset(org_x_location - float(((buttons.size()-1) % button_max) + 1) * horizontal_button_spacing);
    new_button->set_y_offset(top_y_offset);

    //add the button onto the screen
    gui_window->add(new_button);

    if((buttons.size() - 1) % button_max == 0 && buttons.size() != 1)
    {
        cycle_button->call_on_click();
    }

    refresh_gui();
}

void GUIMain::refresh_gui()
{
    gui_manager.parse_components();
}

GUIMain::~GUIMain()
{
    LOG(INFO) << "Destructing GUIMain...";

    buttons.clear();

    LOG(INFO) << "Destructed GUIMain.";
}

void GUIMain::config_gui()
{
    Config::json j = Config::get_instance();

    x_scale = j["scales"]["x_scale"];
    y_scale = j["scales"]["y_scale"];

    left_x_offset = j["scales"]["left_x_offset"];
    right_x_offset = j["scales"]["right_x_offset"];
    bottom_y_offset = j["scales"]["bottom_y_offset"];
    top_y_offset = j["scales"]["top_y_offset"];

    title_x_offset = j["scales"]["title_x_offset"];
    title_y_offset = j["scales"]["title_y_offset"];

    menu_x_offset = j["scales"]["menu_x_offset"];
    menu_y_offset = j["scales"]["menu_y_offset"];

    button_width = j["scales"]["button_width"];
    button_height = j["scales"]["button_height"];

    horizontal_button_spacing = j["scales"]["horizontal_button_spacing"];
    vertical_button_spacing = j["scales"]["vertical_button_spacing"];

    button_max = j["scales"]["button_max"];
}

