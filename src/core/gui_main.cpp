#include "gui_main.hpp"

#include "engine.hpp"
#include "button.hpp"
#include "config.hpp"
#include "gui_manager.hpp"
#include "gui_window.hpp"
#include "map_viewer.hpp"
#include "notification_bar.hpp"

GUIMain::GUIMain(GameWindow * _embedWindow):
	embedWindow(_embedWindow),
	gui_manager(),
	map_viewer(embedWindow, &gui_manager),
	em(EventManager::get_instance()),
	bag_open(false),
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

    notification_bar = new NotificationBar();

    Engine::set_notification_bar(notification_bar);

    pause_button = std::make_shared<Button>(ButtonType::Single);
    pause_button->set_picture("gui/game/pause");
    pause_button->set_alignment(ButtonAlignment::TopLeft);
    pause_button->set_width(button_width);
    pause_button->set_height(button_height);
    pause_button->set_y_offset(float(embedWindow->get_game_window_height())*y_scale);
    pause_button->set_x_offset(0.00f);
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
    bag_button->set_y_offset(float(embedWindow->get_game_window_height())*y_scale);
    bag_button->set_x_offset(float(embedWindow->get_game_window_width())*x_scale);

    bag_button->set_on_click( [&] () {

        if(bag_open == false){

            bag_open = true;
            LOG(INFO) << "Bag opened";
            bag_window->set_visible(true);
            refresh_gui();
            //bag_menu();
        }
        else{

            bag_open = false;
            LOG(INFO) << "Bag closed";
            bag_window->set_visible(false);
            refresh_gui();
        }
    });

    bag_window = std::make_shared<Button>(ButtonType::Board);
    bag_window->set_text("Bag");
	bag_window->set_clickable(false);
	bag_window->set_visible(false);
	bag_window->move_text(0.5f, 0.9f);

	pyguide_button = std::make_shared<Button>(ButtonType::NoPicture);
	pyguide_button->set_text("PyGuide");
    pyguide_button->set_alignment(ButtonAlignment::TopRight);
    pyguide_button->set_width(button_width);
    pyguide_button->set_height(button_height);
    pyguide_button->set_y_offset(0.8f);
    pyguide_button->set_x_offset(0.5f);
	pyguide_button->set_visible(false);

    bag_window->add(pyguide_button);

    gui_window->add(pause_button);
    gui_window->add(bag_button);
    gui_window->add(bag_window);


    LOG(INFO) << "Constructed GUIMain.";
}

void GUIMain::config_gui()
{
	nlohmann::json j = Config::get_instance();

    button_width = j["gui_constants"]["button_width"];
    button_height = j["gui_constants"]["button_height"];
    x_scale = j["gui_constants"]["x_scale"];
    y_scale = j["gui_constants"]["y_scale"];
    button_max = j["gui_constants"]["button_max"];
    button_spacing = j["gui_constants"]["button_spacing"];
}

void GUIMain::close_pause_window(){
	gui_window->set_visible(false);

	const std::map<int, std::shared_ptr<Component>>* gui_components = gui_window->get_components();

	typedef std::map<int, std::shared_ptr<Component>>::const_iterator it_type;

	for(it_type i = gui_components->begin(); i !=gui_components->end(); ++i){
		if(i->second == bag_window){
			continue;
		}
		else{
			i->second->set_visible(true);
			i->second->set_clickable(true);
		}
	}

	refresh_gui();
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
        cycle_button->set_y_offset(float(embedWindow->get_game_window_height())*y_scale);
        cycle_button->set_x_offset(float(embedWindow->get_game_window_width())*x_scale - float(button_max + 1) * button_spacing);

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
    float org_x_location = float(embedWindow->get_game_window_width()) * x_scale;

    new_button->set_x_offset(org_x_location - float(((buttons.size()-1) % button_max) + 1) * button_spacing);
    new_button->set_y_offset(float(embedWindow->get_game_window_height()) * y_scale);

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
    delete notification_bar;

    LOG(INFO) << "Destructed GUIMain.";
}
