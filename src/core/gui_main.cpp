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
    bar_options_open(false),
    external_help_open(false),
    callback_options(false),
    option_start(0),
    option_selected(0),
    pause_open(false),
    music_on(true),
    bag_open(false),
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
    gui_window->set_clickable(false);

    gui_manager.set_root(gui_window);

    create_pause_menu();
    create_notification_bar();
    create_bag();
    create_pyguide();

    LOG(INFO) << "Constructed GUIMain.";
}

GUIMain::~GUIMain()
{
    LOG(INFO) << "Destructing GUIMain...";

    button_indices.clear();
    buttons.clear();
    notification_options.clear();
    option_buttons.clear();
    bag_items.clear();
    pyguide_explanations.clear();
    pyguide_commands.clear();

    LOG(INFO) << "Destructed GUIMain.";
}

void GUIMain::create_pause_menu(){

    pause_button = std::make_shared<Button>(ButtonType::Single);
    pause_button->set_picture("gui/game/pause");
    pause_button->set_alignment(ButtonAlignment::TopLeft);
    pause_button->set_width(button_width);
    pause_button->set_height(button_height);
    pause_button->set_x_offset(left_x_offset);
    pause_button->set_y_offset(top_y_offset);
    pause_button->set_visible(true);
    pause_button->set_clickable(true);

    pause_button->set_on_click( [&] (){

        if(bag_open || bar_open || bar_options_open || external_help_open){
            return;
        }

        if(pause_open == false){
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

    gui_window->add(pause_button);

    exit_button = std::make_shared<Button>(ButtonType::NoPicture);
    exit_button->set_text("Main Menu");
    exit_button->set_alignment(ButtonAlignment::BottomLeft);
    exit_button->set_on_click( [this] () {
        Engine::open_main_menu();
    });
    exit_button->set_clickable(false);
    exit_button->set_visible(false);
    exit_button->set_width(menu_width);
    exit_button->set_height(menu_height);
    exit_button->set_x_offset(pause_x_offset);
    exit_button->set_y_offset(pause_y_offset - 2 * menu_spacing);

    gui_window->add(exit_button);

    music_button = std::make_shared<Button>(ButtonType::NoPicture);
    music_button->set_text("Music ON");
    music_button->set_alignment(ButtonAlignment::BottomLeft);
    music_button->set_on_click( [this] () {
        if(music_on){
            music_button->set_text("Music OFF");
            refresh_gui();
            Engine::set_music(false);
        }
        else{
            music_button->set_text("Music ON");
            refresh_gui();
            Engine::set_music(true);
        }
        music_on = ! music_on;
    });
    music_button->set_visible(false);
    music_button->set_width(menu_width);
    music_button->set_height(menu_height);
    music_button->set_x_offset(pause_x_offset);
    music_button->set_y_offset(pause_y_offset - 1 * menu_spacing);

    gui_window->add(music_button);

    restart_button = std::make_shared<Button>(ButtonType::NoPicture);
    restart_button->set_text("Restart Level");
    restart_button->set_alignment(ButtonAlignment::BottomLeft);
    restart_button->set_on_click( [this] () {
        Engine::restart_level();
    });
    restart_button->set_visible(false);
    restart_button->set_width(menu_width);
    restart_button->set_height(menu_height);
    restart_button->set_x_offset(pause_x_offset);
    restart_button->set_y_offset(pause_y_offset - 0 * menu_spacing);

    gui_window->add(restart_button);
}

void GUIMain::create_notification_bar(){

    notification_bar = std::make_shared<TextBox>(TextBoxType::Bar);
    notification_bar->set_width(notification_width);
    notification_bar->set_height(notification_height);
    notification_bar->set_x_offset(left_x_offset);
    notification_bar->set_y_offset(bottom_y_offset);
    notification_bar->move_text(notification_text_x, notification_text_y);
    notification_bar->resize_text(notification_text_width, notification_text_height);
    notification_bar->resize_buttons(notification_button_width, notification_button_height);
    notification_bar->move_buttons(notification_button_x, notification_button_y);
    notification_bar->set_visible(false);
    notification_bar->set_clickable(false);
    notification_bar->set_buffer_size(notification_text_buffer);

    external_script_help = std::make_shared<TextBox>(TextBoxType::ExternalScriptHelp);
    external_script_help->set_width(notification_width);
    external_script_help->set_height(notification_height);
    external_script_help->set_x_offset(left_x_offset);
    external_script_help->set_y_offset(bottom_y_offset);
    external_script_help->move_text(notification_text_x, notification_text_y);
    external_script_help->resize_text(notification_text_width, notification_text_height);
    external_script_help->resize_buttons(notification_button_width, notification_button_height);
    external_script_help->move_buttons(notification_button_x, notification_button_y);
    external_script_help->set_visible(false);
    external_script_help->set_clickable(false);
    external_script_help->set_buffer_size(notification_text_buffer);

    options_box = std::make_shared<Board>(ButtonType::Board);
    options_box->set_width(option_width);
    options_box->set_height(option_height);
    options_box->set_x_offset(option_x);
    options_box->set_y_offset(option_y);
    options_box->set_visible(false);
    options_box->set_clickable(false);

    gui_window->add(options_box);
    gui_window->add(notification_bar);
    gui_window->add(external_script_help);
}

void GUIMain::create_bag(){

    bag_button = std::make_shared<Button>(ButtonType::Single);
    bag_button->set_picture("gui/game/bag");
    bag_button->set_text("");
    bag_button->set_alignment(ButtonAlignment::TopRight);
    bag_button->set_width(button_width);
    bag_button->set_height(button_height);
    bag_button->set_x_offset(right_x_offset);
    bag_button->set_y_offset(top_y_offset);
    bag_button->set_visible(true);
    bag_button->set_clickable(true);

    bag_button->set_on_click( [&] ()
    {
        if(pause_open || bar_open || bar_options_open || external_help_open){
            return;
        }

        if(bag_open == false)
        {
            em->pause();
            LOG(INFO) << "BAG OPEN";
            open_bag();
        }
        else
        {
            em->resume();
            LOG(INFO) << "BAG CLOSED";
            close_bag();
        }
    });

    bag_window = std::make_shared<Board>(ButtonType::Board);
    bag_window->set_text("");
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
    pyguide_button->set_clickable(false);

    pyguide_button->set_on_click( [&] ()
    {
        open_pyguide();
    });

    bag_window->add(pyguide_button);
    gui_window->add(bag_button);
    gui_window->add(bag_window);
}

void GUIMain::hide_bag(){
    bag_button->set_clickable(false);
    bag_button->set_visible(false);
}

void GUIMain::unhide_bag(){
    bag_button->set_clickable(true);
    bag_button->set_visible(true);
}

void GUIMain::create_pyguide(){

    nlohmann::json j = Config::get_instance();

    pyguide_window = std::make_shared<Board>(ButtonType::Board);
    pyguide_window->set_text("PyGuide");
    pyguide_window->set_clickable(false);
    pyguide_window->set_visible(false);
    pyguide_window->move_text(pyguide_title_x_offset, pyguide_title_y_offset);

    py_help = std::make_shared<TextBox>(TextBoxType::Display);
    py_help->set_width(py_help_width);
    py_help->set_height(py_help_height);
    py_help->set_x_offset(py_help_x);
    py_help->set_y_offset(py_help_y);
    py_help->set_visible(false);
    py_help->set_clickable(false);

    py_help->move_text(py_help_text_x, py_help_text_y);
    py_help->resize_text(py_help_text_width, py_help_text_height);
    py_help->move_buttons(py_help_button_x, py_help_button_y, py_help_button_spacing);
    py_help->resize_buttons(py_help_button_width, py_help_button_height);
    py_help->set_buffer_size(py_help_text_buffer);

    pyguide_next_button = std::make_shared<Button>(ButtonType::Single);
    pyguide_next_button->set_alignment(ButtonAlignment::BottomLeft);
    pyguide_next_button->move_text(0.0, 0.0);
    pyguide_next_button->set_picture("gui/game/next");
    pyguide_next_button->set_text("");
    pyguide_next_button->set_width(menu_move_width);
    pyguide_next_button->set_height(menu_move_height);
    pyguide_next_button->set_x_offset(menu_move_x);
    pyguide_next_button->set_y_offset(menu_move_y);
    pyguide_next_button->set_clickable(false);
    pyguide_next_button->set_visible(false);

    pyguide_next_button->set_on_click( [this] () {
        py_help->clear_text();

        if(pyguide_page.second == 1){
            //do nothing, there's only one page
            return;
        }

        //remove previous buttons
        for(unsigned int i=(pyguide_page.first-1)*menu_max;
                         i<(pyguide_page.first)*menu_max && i<py_apis_num;
                         i++){
            pyguide_window->remove(pyguide_commands[i]->get_id());
            pyguide_commands[i]->set_visible(false);
            pyguide_commands[i]->set_clickable(false);
        }

        if(pyguide_page.first == pyguide_page.second){
            //we have to wind over to the first page
            pyguide_page.first = 1;
        }
        else{
            ++pyguide_page.first;
        }

        //add new buttons
        for(unsigned int i=(pyguide_page.first-1)*menu_max;
                         i<(pyguide_page.first)*menu_max && i<py_apis_num;
                         i++){
            pyguide_window->add(pyguide_commands[i]);
            pyguide_commands[i]->set_visible(true);
            pyguide_commands[i]->set_clickable(true);
        }

        pyguide_page_display->set_text(std::to_string(pyguide_page.first) + "/" + std::to_string(pyguide_page.second));
        refresh_gui();
    });
    pyguide_window->add(pyguide_next_button);

    pyguide_back_button = std::make_shared<Button>(ButtonType::Single);
    pyguide_back_button->set_alignment(ButtonAlignment::BottomLeft);
    pyguide_back_button->move_text(0.0, 0.0);
    pyguide_back_button->set_picture("gui/game/prev");
    pyguide_back_button->set_text("");
    pyguide_back_button->set_width(menu_move_width);
    pyguide_back_button->set_height(menu_move_height);
    pyguide_back_button->set_x_offset(menu_move_x - menu_move_spacing);
    pyguide_back_button->set_y_offset(menu_move_y);
    pyguide_back_button->set_clickable(false);
    pyguide_back_button->set_visible(false);

    pyguide_back_button->set_on_click([this] () {
        py_help->clear_text();

        if(pyguide_page.second == 1){
            //do nothing, there's only one page
            return;
        }

        //remove previous buttons
        for(unsigned int i=(pyguide_page.first-1)*menu_max;
                         i<(pyguide_page.first)*menu_max && i<py_apis_num;
                         i++){
            pyguide_window->remove(pyguide_commands[i]->get_id());
            pyguide_commands[i]->set_visible(false);
            pyguide_commands[i]->set_clickable(false);
        }

        if(pyguide_page.first == 1){
            //we have to wind over to the last page
            pyguide_page.first = pyguide_page.second;
        }
        else{
            --pyguide_page.first;
        }

        //add new buttons
        for(unsigned int i=(pyguide_page.first-1)*menu_max;
                         i<(pyguide_page.first)*menu_max && i<py_apis_num;
                         i++){
            pyguide_window->add(pyguide_commands[i]);
            pyguide_commands[i]->set_visible(true);
            pyguide_commands[i]->set_clickable(true);
        }

        pyguide_page_display->set_text(std::to_string(pyguide_page.first) + "/" + std::to_string(pyguide_page.second));
        refresh_gui();
    });
    pyguide_window->add(pyguide_back_button);

    pyguide_page_display = std::make_shared<Board>(ButtonType::NoPicture);
    pyguide_page_display->set_text("0/0");
    pyguide_page_display->set_alignment(ButtonAlignment::BottomLeft);
    pyguide_page_display->set_width(menu_move_width);
    pyguide_page_display->set_height(menu_move_height);
    pyguide_page_display->set_x_offset(menu_page_display_x);
    pyguide_page_display->set_y_offset(menu_page_display_y);
    pyguide_page_display->set_visible(false);
    pyguide_back_button->set_clickable(false);
    pyguide_window->add(pyguide_page_display);

    pyguide_page.first = 1;
    pyguide_page.second = (py_apis_num/menu_max) + 1;

    if(py_apis_num % menu_max == 0) {
        pyguide_page.second--; //Because if the buttons fit in exactly, we don't want to create a new page
    }

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

            py_command->set_on_click( [this, explanation] () {
                py_help->clear_text();
                py_help->add_text(explanation);
                py_help->open();
            });

            pyguide_commands.push_back(py_command);

            pyguide_commands[i]->move_text(py_help_item_text_x, py_help_item_text_y);
            pyguide_commands[i]->set_width(py_help_item_width);
            pyguide_commands[i]->set_height(py_help_item_height);
            pyguide_commands[i]->set_x_offset(py_help_item_x);
            pyguide_commands[i]->set_y_offset(py_help_item_y - float(i % menu_max)*py_help_item_spacing);

            py_command->set_clickable(false);
            py_command->set_visible(false);
        }
    }

    pyguide_window->add(py_help);
    gui_window->add(pyguide_window);
}

void GUIMain::open_pause_window(){

    pause_button->set_x_offset(close_x_offset);
    pause_button->set_y_offset(close_y_offset);
    pause_button->set_picture("gui/game/play");
    pause_button->set_text("");

    pause_open = true;
    gui_window->set_visible(true);
    gui_window->set_clickable(false);

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
    }

    exit_button->set_visible(true);
    exit_button->set_clickable(true);

    music_button->set_visible(true);
    music_button->set_clickable(true);

    restart_button->set_visible(true);
    restart_button->set_clickable(true);

    refresh_gui();
}

void GUIMain::close_pause_window(){

    pause_button->set_x_offset(left_x_offset);
    pause_button->set_y_offset(top_y_offset);
    pause_button->set_picture("gui/game/pause");
    pause_button->set_text("");

    pause_open = false;

    gui_window->set_visible(false);
    gui_window->set_clickable(false);

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
        else if(i->second == external_script_help){
            continue;
        }
        else if(i->second == options_box){
            continue;
        }
        else{
            i->second->set_visible(true);
            i->second->set_clickable(true);
        }
    }

    exit_button->set_visible(false);
    exit_button->set_clickable(false);

    music_button->set_visible(false);
    music_button->set_clickable(false);

    restart_button->set_visible(false);
    restart_button->set_clickable(false);

    refresh_gui();
}

void GUIMain::open_notification_bar(std::function<void ()> func){
    bar_open = true;
    callback_options = false;
    notification_func = func;
    notification_bar->open();
    LOG(INFO) << "Notification Bar open";
}

void GUIMain::open_notification_bar_with_options(std::deque<std::pair<std::string, std::function<void ()> > > options){
    bar_open = true;
    callback_options = true;
    notification_options.clear();
    notification_options = options;
    notification_bar->open();
    LOG(INFO) << "Notification Bar open";
}

void GUIMain::proceed_notification_bar(){
    notification_bar->proceed();
}

void GUIMain::proceed_notification_bar_with_options(){
    for(unsigned int i=0; i<=1; i++){
        if ((option_selected == i) && (notification_bar_with_options_arrow_select_enable)){
            option_buttons[i]->call_on_click();
        }
    }
}

void GUIMain::proceed_selection_notification_bar_with_options(bool forward){
    notification_bar_with_options_arrow_select_enable = true;

	if(forward)
	{
		if(option_selected > 0){
			option_selected--;
		}
	}
	else{
		if(option_selected < 1){
			option_selected++; 
		}
	}
    for(unsigned int i=0; i<2; i++){
        if (i == option_selected){
            option_buttons[i]->highlight();
        }
        else{
            option_buttons[i]->unhighlight();
        }
    }
}

void GUIMain::close_notification_bar(){
    if(callback_options){
        bar_options_open = true;
        options_box->set_visible(true);
        options_box->set_clickable(false);

        for(int i=0; i<=1; i++){
            std::shared_ptr<Button> option_button = std::make_shared<Button>(ButtonType::NoPicture);
            //option_button->set_alignment(ButtonAlignment::BottomLeft);
            option_button->set_width(option_button_width);
            option_button->set_height(option_button_height);
            option_button->set_x_offset(option_button_x - float(i)*option_button_spacing);
            option_button->set_y_offset(option_button_y);
            option_button->set_text(" " + notification_options[i].first); 
            option_button->move_text(option_button_text_x, option_button_text_y);
            option_button->set_visible(true);
            option_button->set_clickable(true);

            option_button->set_on_click([this, i] (){
                notification_bar->clear_text();
                notification_bar->close();
                options_box->set_visible(false);
                options_box->set_clickable(false);
                for(int i=0; i<=1; i++){
                    option_buttons[i]->set_visible(false);
                    option_buttons[i]->set_clickable(false);
                }
                em->add_event([this, i] {
                    notification_options[i].second();
                });
                option_buttons.clear();

                refresh_gui();
                Engine::enable_py_scripter();
                bar_open = false;
                bar_options_open = false;
            });

            option_buttons.push_back(option_button);
            options_box->add(option_buttons[i]);
        }
        notification_bar_with_options_arrow_select_enable = false;
    }
    else{
        notification_bar->clear_text();
        notification_bar->close();
        bar_open = false;
        em->add_event([this] {
            notification_func();
        });
    }

    refresh_gui();
    LOG(INFO) << "Notification Bar closed";
}

void GUIMain::show_external_script_help(std::string text){
    external_help_open = true;
    external_script_help->add_text(text);
    external_script_help->open();
    LOG(INFO) << "ExternalScript Help open";
}

void GUIMain::proceed_external_script_help(){
    external_script_help->proceed();
}

void GUIMain::close_external_script_help(){
    if (external_help_open){
        external_script_help->clear_text();
        external_script_help->close();
        external_help_open = false;
    }
}

void GUIMain::open_bag()
{
    bag_open = true;
    bag_window->set_visible(true);
    bag_window->set_clickable(false);

    bag_button->set_text("");

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
    bag_open = false;
    bag_window->set_visible(false);
    bag_window->set_clickable(false);

    close_pyguide();
    bag_button->set_text("");

    pyguide_button->set_visible(false);
    pyguide_button->set_clickable(false);

    for(unsigned int i=0; i!=bag_items.size(); i++)
    {
        bag_items[i]->set_visible(false);
        bag_items[i]->set_clickable(false);
    }

    refresh_gui();
    LOG(INFO) << "Bag closed";
}

void GUIMain::open_pyguide()
{
    close_bag();
    bag_open = true; //because the pyguide is now open
    bag_button->set_text("");

    pyguide_window->set_visible(true);
    pyguide_window->set_clickable(false);
    py_help->set_visible(true);
    py_help->set_clickable(false);
    py_help->open();

    pyguide_next_button->set_visible(true);
    pyguide_next_button->set_clickable(true);
    pyguide_back_button->set_visible(true);
    pyguide_back_button->set_clickable(true);

    pyguide_page_display->set_text(std::to_string(pyguide_page.first) + "/" + std::to_string(pyguide_page.second));
    pyguide_page_display->set_visible(true);
    pyguide_page_display->set_clickable(false);

    for(unsigned int i=(pyguide_page.first-1)*menu_max;
                         i<(pyguide_page.first)*menu_max && i<py_apis_num;
                         i++){
        pyguide_window->add(pyguide_commands[i]);
        pyguide_commands[i]->set_visible(true);
        pyguide_commands[i]->set_clickable(true);
    }

    refresh_gui();
    LOG(INFO) << "PyGuide opened";
}

void GUIMain::close_pyguide()
{
    bag_open = false;
    bag_button->set_text("");

    pyguide_window->set_visible(false);
    pyguide_window->set_clickable(false);
    py_help->set_visible(false);
    py_help->set_clickable(false);
    py_help->close();

    pyguide_next_button->set_visible(false);
    pyguide_next_button->set_clickable(false);
    pyguide_back_button->set_visible(false);
    pyguide_back_button->set_clickable(false);
    pyguide_page_display->set_visible(false);
    pyguide_page_display->set_clickable(false);

    for(unsigned int i=(pyguide_page.first-1)*menu_max;
                         i<(pyguide_page.first)*menu_max && i<py_apis_num;
                         i++){
        pyguide_commands[i]->set_visible(false);
        pyguide_commands[i]->set_clickable(false);
        pyguide_window->remove(pyguide_commands[i]->get_id());
    }

    refresh_gui();
    LOG(INFO) << "PyGuide closed";
}

void GUIMain::add_message(std::string text){
    notification_bar->add_message(text);
}

void GUIMain::add_text(std::string text){
    notification_bar->add_text(text);
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
    new_button->set_visible(true);
    new_button->set_clickable(true);
    new_button->move_text(button_text_x, button_text_y);

    //Push to index element 'id'
    if (button_id > (button_indices.size() - 1))
    {
        button_indices.resize(button_id+1, 0);
    }
    button_indices[button_id] = new_button_index;

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

void GUIMain::set_button_index(unsigned int value)
{
    cur_button_index = value;
    update_selected();
}

void GUIMain::click_next_player()
{
    if (buttons.size() < 1) return;
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
    unsigned int click_button_index = button_indices[button_id];
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
    unsigned int update_button_index = button_indices[button_id];
    buttons[update_button_index]->set_text(name);
}

void GUIMain::update_selected()
{
    //Highlight the selected player
    for (unsigned int i=0; i<buttons.size(); i++)
    {
        if (cur_button_index == i)
        {
            buttons[i]->set_text_colour(0, 0, 0, 255);
            refresh_gui();
        }
        else
        {
            buttons[i]->set_text_colour(150, 150, 200, 240);
            refresh_gui();
        }
    }
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

    close_x_offset = j["scales"]["close_x_offset"];
    close_y_offset = j["scales"]["close_y_offset"];

    title_x_offset = j["scales"]["title_x_offset"];
    title_y_offset = j["scales"]["title_y_offset"];
    pyguide_title_x_offset = j["scales"]["pyguide_title_x_offset"];
    pyguide_title_y_offset = j["scales"]["pyguide_title_y_offset"];

    pause_x_offset = j["scales"]["pause_x_offset"];
    pause_y_offset = j["scales"]["pause_y_offset"];

    menu_x_offset = j["scales"]["menu_x_offset"];
    menu_y_offset = j["scales"]["menu_y_offset"];
    menu_width = j["scales"]["menu_width"];
    menu_height = j["scales"]["menu_height"];
    menu_spacing = j["scales"]["menu_spacing"];
    menu_max = j["scales"]["menu_max"];

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

    option_max = j["scales"]["option_max"];
    option_width = j["scales"]["option_width"];
    option_height = j["scales"]["option_height"];
    option_x = j["scales"]["option_x"];
    option_y = j["scales"]["option_y"];
    option_button_width = j["scales"]["option_button_width"];
    option_button_height = j["scales"]["option_button_height"];
    option_button_x = j["scales"]["option_button_x"];
    option_button_y = j["scales"]["option_button_y"];
    option_button_text_x = j["scales"]["option_button_text_x"];
    option_button_text_y = j["scales"]["option_button_text_y"];
    option_button_spacing = j["scales"]["option_button_spacing"];

    py_help_width = j["scales"]["py_help_width"];
    py_help_height = j["scales"]["py_help_height"];
    py_help_x = j["scales"]["py_help_x"];
    py_help_y = j["scales"]["py_help_y"];

    py_help_item_width = j["scales"]["py_help_item_width"];
    py_help_item_height = j["scales"]["py_help_item_height"];
    py_help_item_x = j["scales"]["py_help_item_x"];
    py_help_item_y = j["scales"]["py_help_item_y"];
    py_help_item_spacing = j["scales"]["py_help_item_spacing"];
    py_help_item_text_x = j["scales"]["py_help_item_text_x"];
    py_help_item_text_y = j["scales"]["py_help_item_text_y"];

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

    menu_move_height = j["scales"]["menu_move_height"];
    menu_move_width = j["scales"]["menu_move_width"];
    menu_move_x = j["scales"]["menu_move_x"];
    menu_move_y = j["scales"]["menu_move_y"];
    menu_move_spacing = j["scales"]["menu_move_spacing"];
    menu_page_display_x = j["scales"]["menu_page_display_x"];
    menu_page_display_y = j["scales"]["menu_page_display_y"];

    button_width = j["scales"]["button_width"];
    button_height = j["scales"]["button_height"];
    button_text_x = j["scales"]["button_text_x"];
    button_text_y = j["scales"]["button_text_y"];

    horizontal_button_spacing = j["scales"]["horizontal_button_spacing"];
    vertical_button_spacing = j["scales"]["vertical_button_spacing"];

    button_max = j["scales"]["button_max"];

    py_apis_num = j["pyguide_apis"]["number"];
}

void GUIMain::refresh_gui()
{
    gui_manager.parse_components();
}
