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
    bar_open(false),
    bag_open(false),
    pyguide_open(false),
    display_button_start(0)

{
    LOG(INFO) << "Constructing GUIMain...";

    config_gui();

    Engine::set_gui(this);
    //Create the game embedWindow to present to the users
    embedWindow->use_context();

    Engine::set_game_window(embedWindow);

    Engine::set_map_viewer(&map_viewer);

    gui_window = std::make_shared<GUIWindow>();
    gui_window->set_visible(false);

    gui_manager.set_root(gui_window);

    create_notification_bar();
    create_pause_menu();
    create_bag();
    create_pyguide();


    LOG(INFO) << "Constructed GUIMain.";
}

GUIMain::~GUIMain()
{
    LOG(INFO) << "Destructing GUIMain...";

    buttons.clear();

    LOG(INFO) << "Destructed GUIMain.";
}

void GUIMain::create_pyguide(){

    nlohmann::json j = Config::get_instance();

    pyguide_window = std::make_shared<Board>(ButtonType::Board);
    pyguide_window->set_text("PyGuide");
    pyguide_window->set_clickable(false);
    pyguide_window->set_visible(false);
    pyguide_window->move_text(pyguide_title_x_offset, pyguide_title_y_offset);

    py_help = std::make_shared<TextBox>(TextBoxType::Both);
    py_help->set_width(py_help_width);
    py_help->set_height(py_help_height);
    py_help->set_x_offset(py_help_x);
    py_help->set_y_offset(py_help_y);
    py_help->set_visible(false);

    py_help->move_text(py_help_text_x, py_help_text_y);
    py_help->resize_text(py_help_text_width, py_help_text_height);
    py_help->move_buttons(py_help_button_x, py_help_button_y, py_help_button_spacing);
    py_help->resize_buttons(py_help_button_width, py_help_button_height);
    py_help->set_buffer_size(py_help_text_buffer);

    for(unsigned int i=0; i<py_apis_num; i++){
        std::string help = j["pyguide_apis"][std::to_string(i)];
        unsigned int beg_name = help.find(":");

        if(beg_name == std::string::npos){
            LOG(INFO) << "ERROR: Pycommand " << i << " in the config file does not follow the format required";
            return;
        }
        else{
            std::string name = help.substr(0, beg_name);
            std::string explanation = help.substr(beg_name+1, std::string::npos);


            std::shared_ptr<Button> py_command = std::make_shared<Button>(ButtonType::NoPicture);
            py_command->set_text(name);
            pyguide_explanations.push_back(explanation);

            py_command->set_alignment(ButtonAlignment::BottomLeft);
            py_command->set_visible(false);

            py_command->set_on_click( [this, explanation] () {
                py_help->clear_text();
                py_help->add_text(explanation);
                py_help->open();
            });

            py_command->set_clickable(false);

            pyguide_commands.push_back(py_command);
            pyguide_window->add(py_command);

            pyguide_commands[i]->set_width(py_help_item_width);
            pyguide_commands[i]->set_height(py_help_item_height);
            pyguide_commands[i]->set_x_offset(py_help_item_x);
            pyguide_commands[i]->set_y_offset(py_help_item_y - float(i)*py_help_item_spacing);

        }
    }

    pyguide_window->add(py_help);
    gui_window->add(pyguide_window);
}

void GUIMain::create_pause_menu(){

    pause_button = std::make_shared<Button>(ButtonType::Single);
    pause_button->set_picture("gui/game/pause");
    pause_button->set_alignment(ButtonAlignment::TopLeft);
    pause_button->set_width(button_width);
    pause_button->set_height(button_height);
    pause_button->set_x_offset(left_x_offset);
    pause_button->set_y_offset(top_y_offset);
    pause_button->set_on_click( [&] ()
    {

        if(em->is_paused() == false)
        {
            em->pause();
            LOG(INFO) << "PAUSED";
            open_pause_window();
        }
        else
        {
            em->resume();
            LOG(INFO) << "RESUMED";
            close_pause_window();
        }
    });
    gui_window->add(pause_button);
}

void GUIMain::create_bag(){

    bag_button = std::make_shared<Button>(ButtonType::Single);
    bag_button->set_picture("gui/game/bag");
    bag_button->set_text("Bag");
    bag_button->set_alignment(ButtonAlignment::TopRight);
    bag_button->set_width(button_width);
    bag_button->set_height(button_height);
    bag_button->set_x_offset(right_x_offset);
    bag_button->set_y_offset(top_y_offset);

    bag_button->set_on_click( [&] ()
    {

        if(bag_open == false)
        {
            bag_open = true;
            open_bag();
        }
        else
        {
            bag_open = false;
            close_bag();
        }
    });

    bag_window = std::make_shared<Board>(ButtonType::Board);
    bag_window->set_text("Bag");
    bag_window->set_clickable(false);
    bag_window->set_visible(false);
    bag_window->move_text(title_x_offset, title_y_offset);

    pyguide_button = std::make_shared<Button>(ButtonType::NoPicture);
    pyguide_button->set_text("PyGuide");
    pyguide_button->set_alignment(ButtonAlignment::BottomLeft);
    pyguide_button->set_width(button_width);
    pyguide_button->set_height(button_height);
    pyguide_button->set_x_offset(menu_x_offset);
    pyguide_button->set_y_offset(menu_y_offset);
    pyguide_button->set_visible(false);

    pyguide_button->set_on_click( [&] ()
    {
        pyguide_open = true;
        open_pyguide();
    });

    bag_window->add(pyguide_button);
    gui_window->add(bag_button);
    gui_window->add(bag_window);
}

void GUIMain::create_notification_bar(){

    notification_bar = std::make_shared<TextBox>(TextBoxType::Forward);
    notification_bar->set_width(notification_width);
    notification_bar->set_height(notification_height);
    notification_bar->set_x_offset(left_x_offset);
    notification_bar->set_y_offset(bottom_y_offset);
    notification_bar->move_text(notification_text_x, notification_text_y);
    notification_bar->resize_text(notification_text_width, notification_text_height);
    notification_bar->resize_buttons(notification_button_width, notification_button_height);
    notification_bar->move_buttons(notification_button_x, notification_button_y);
    notification_bar->set_visible(false);
    notification_bar->set_buffer_size(notification_text_buffer);

    gui_window->add(notification_bar);
}

void GUIMain::open_notification_bar(){
    notification_bar->open();
    bar_open = true;
}

void GUIMain::close_notification_bar(){
    notification_bar->close();
    bar_open = false;
}

void GUIMain::add_message(std::string text){
    notification_bar->add_message(text);
}

void GUIMain::add_text(std::string text){
    notification_bar->add_text(text);
}

void GUIMain::open_pause_window(){

    close_bag();
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
        else if(i->second == notification_bar){
            continue;
        }
        else{
            i->second->set_visible(true);
            i->second->set_clickable(true);
        }
    }

    refresh_gui();
}

void GUIMain::open_bag()
{

    close_notification_bar();
    bag_window->set_visible(true);

    close_pyguide();
    pyguide_open = false;

    pyguide_button->set_visible(true);
    pyguide_button->set_clickable(true);

    for(unsigned int i=0; i!=bag_items.size(); i++)
    {
        bag_items[i]->set_visible(true);
        bag_items[i]->set_clickable(true);
    }

    refresh_gui();
    LOG(INFO) << "Bag opened";
}

void GUIMain::close_bag()
{

    bag_window->set_visible(false);

    close_pyguide();
    pyguide_open = false;

    pyguide_button->set_visible(false);
    pyguide_button->set_clickable(false);

    for(unsigned int i=0; i!=bag_items.size(); i++)
    {
        bag_items[i]->set_visible(false);
        bag_items[i]->set_clickable(false);
    }

    if(bar_open){
        open_notification_bar();
    }

    refresh_gui();
    LOG(INFO) << "Bag closed";
}

void GUIMain::open_pyguide()
{
    close_bag();
    pyguide_window->set_visible(true);
    py_help->set_visible(true);

    for(unsigned int i=0; i<py_apis_num; i++){
        pyguide_commands[i]->set_visible(true);
        pyguide_commands[i]->set_clickable(true);
    }

    py_help->open();
    refresh_gui();
    LOG(INFO) << "PyGuide opened";
}

void GUIMain::close_pyguide()
{
    pyguide_window->set_visible(false);
    py_help->set_visible(false);

    for(unsigned int i=0; i<py_apis_num; i++){
        pyguide_commands[i]->set_visible(false);
        pyguide_commands[i]->set_clickable(false);
    }

    py_help->close();
    refresh_gui();
    LOG(INFO) << "PyGuide closed";
}

void GUIMain::add_button(std::string file_path, std::string name, std::function<void (void)> callback, unsigned int button_id)
{
    if(buttons.size() == button_max)
    {
        cycle_button = std::make_shared<Button>(ButtonType::Single);
        cycle_button->set_picture("gui/game/buttons/cycle");
        cycle_button->set_text("Cycle");
        cycle_button->set_width(button_width);
        cycle_button->set_height(button_height);
        cycle_button->set_x_offset(right_x_offset - float(button_max + 1) * horizontal_button_spacing);
        cycle_button->set_y_offset(top_y_offset);

        cycle_button->set_on_click([&] ()
        {
            cycle();
        });

        gui_window->add(cycle_button);

        refresh_gui();
    }

    std::shared_ptr<Button> new_button;
    new_button = std::make_shared<Button>(ButtonType::Single);
    buttons.push_back(new_button);

    new_button->set_picture(file_path);
    new_button->set_text(name);

    unsigned int new_button_index = (int) (buttons.size() - 1);
    new_button->set_on_click([this, new_button_index, callback] ()
    {
        callback();
        this->set_button_index(new_button_index);
    });
    new_button->set_width(button_width);
    new_button->set_height(button_height);

    //Push to index element 'id'
    if (button_id > (button_indexs.size() - 1))
    {
        button_indexs.resize(button_id+1, 0);
    }
    button_indexs[button_id] = new_button_index;

    //make space for previous buttons
    float org_x_location = right_x_offset;

    new_button->set_x_offset(org_x_location - float(((buttons.size()-1) % button_max) + 1) * horizontal_button_spacing);
    new_button->set_y_offset(top_y_offset);

    //add the button onto the screen
    gui_window->add(new_button);

    if((buttons.size() - 1) % button_max == 0 && buttons.size() != 1)
    {
        cycle();
    }

    refresh_gui();
}

void GUIMain::cycle()
{
    //remove previous set of buttons
    for(unsigned int i=0; i<button_max && display_button_start + i < buttons.size(); i++)
    {
        gui_window->remove(buttons[display_button_start + i]->get_id());
    }

    //update the button index of buttons to be displayed
    if(display_button_start + button_max >= buttons.size())
    {
        display_button_start = 0;
    }
    else
    {
        display_button_start += button_max;
    }

    //display new buttons
    for(unsigned int i=0; i<button_max && display_button_start + i < buttons.size(); i++)
    {
        gui_window->add(buttons[display_button_start + i]);
    }

    refresh_gui();
}

void GUIMain::refresh_gui()
{
    gui_manager.parse_components();
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
    pyguide_title_x_offset = j["scales"]["pyguide_title_x_offset"];
    pyguide_title_y_offset = j["scales"]["pyguide_title_y_offset"];

    menu_x_offset = j["scales"]["menu_x_offset"];
    menu_y_offset = j["scales"]["menu_y_offset"];
    menu_width = j["scales"]["menu_width"];
    menu_height = j["scales"]["menu_height"];
    menu_spacing = j["scales"]["menu_spacing"];

    notification_width = j["scales"]["notification_width"];
    notification_height = j["scales"]["notification_height"];

    notification_text_width = j["scales"]["notification_text_width"];
    notification_text_height = j["scales"]["notification_text_height"];
    notification_text_x = j["scales"]["notification_text_x"];
    notification_text_y = j["scales"]["notification_text_y"];
    notification_text_buffer = j["scales"]["notification_text_buffer"];

    notification_button_width = j["scales"]["notification_button_width"];
    notification_button_height = j["scales"]["notification_button_height"];
    notification_button_x = j["scales"]["notification_button_x"];
    notification_button_y = j["scales"]["notification_button_y"];

    py_help_width = j["scales"]["py_help_width"];
    py_help_height = j["scales"]["py_help_height"];
    py_help_x = j["scales"]["py_help_x"];
    py_help_y = j["scales"]["py_help_y"];

    py_help_item_width = j["scales"]["py_help_item_width"];
    py_help_item_height = j["scales"]["py_help_item_height"];
    py_help_item_x = j["scales"]["py_help_item_x"];
    py_help_item_y = j["scales"]["py_help_item_y"];
    py_help_item_spacing = j["scales"]["py_help_item_spacing"];

    py_help_text_width = j["scales"]["py_help_text_width"];
    py_help_text_height = j["scales"]["py_help_text_height"];
    py_help_text_x = j["scales"]["py_help_text_x"];
    py_help_text_y = j["scales"]["py_help_text_y"];
    py_help_text_buffer = j["scales"]["py_help_text_buffer"];

    py_help_button_width = j["scales"]["py_help_button_width"];
    py_help_button_height = j["scales"]["py_help_button_height"];
    py_help_button_x = j["scales"]["py_help_button_x"];
    py_help_button_y = j["scales"]["py_help_button_y"];
    py_help_button_spacing = j["scales"]["py_help_button_spacing"];

    button_width = j["scales"]["button_width"];
    button_height = j["scales"]["button_height"];

    horizontal_button_spacing = j["scales"]["horizontal_button_spacing"];
    vertical_button_spacing = j["scales"]["vertical_button_spacing"];

    button_max = j["scales"]["button_max"];

    py_apis_num = j["pyguide_apis"]["number"];
}

void GUIMain::set_button_index(unsigned int value)
{
    cur_button_index = value;
    update_selected();
}

void GUIMain::click_next_player()
{
    cur_button_index = cur_button_index + 1;
    if (cur_button_index >= buttons.size())
    {
        cur_button_index = 0;
    }
    update_selected();
    buttons[cur_button_index]->call_on_click();
}

void GUIMain::click_player(unsigned int button_id)
{
    //Get the index at which the player's button is stored (in buttons)
    //using the button's identifier
    unsigned int click_button_index = button_indexs[button_id];
    set_button_index(click_button_index);
    //Cycle through the pages of sprite buttons until the current player is present
    //(only try cycling a finite number of attempts, to prevent infinite loops)
    for (unsigned int attempts = 0; attempts<buttons.size(); attempts++)
    {
        if (!((click_button_index < display_button_start+button_max) && (click_button_index >= display_button_start)))
        {
            cycle();
        }
        else
        {
            break;
        }
    }
    return;
}

void GUIMain::update_button_text(std::string name, unsigned int button_id)
{
    unsigned int update_button_index = button_indexs[button_id];
    buttons[update_button_index]->set_text(name);
}

void GUIMain::update_selected()
{
    //Highlight the selected player
    for (unsigned int i=0; i<buttons.size(); i++)
    {
        if (cur_button_index == i)
        {
            buttons[i]->set_text_colour(255, 255, 255, 255);
            refresh_gui();
        }
        else
        {
            buttons[i]->set_text_colour(255, 255, 255, 0);
            refresh_gui();
        }
    }

}

