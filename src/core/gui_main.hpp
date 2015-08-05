#ifndef GUI_MAIN_H
#define GUI_MAIN_H

#include <memory>
#include <deque>

#include "button.hpp"
#include "config.hpp"
#include "event_manager.hpp"
#include "gui_manager.hpp"
#include "gui_window.hpp"
#include "map_viewer.hpp"
#include "notification_bar.hpp"

///
/// The entire GUI, which consists of all the components in it.
/// Created by GameMain and contained in it
///
class GUIMain {

private:

	float button_width;
	float button_height;
	float x_scale;
	float y_scale;
	unsigned int button_max;
	float button_spacing;

	GameWindow * embedWindow;
	GUIManager gui_manager;
	MapViewer map_viewer;
	EventManager *em;

	std::shared_ptr<GUIWindow> gui_window;
    NotificationBar *notification_bar;

    std::shared_ptr<Button> pause_button;

    bool bag_open; //whether or not the bag is open
    std::shared_ptr<Button> bag_button;
    std::shared_ptr<Button> bag_window;
    std::deque<std::shared_ptr<Button>> bag_items;
    std::shared_ptr<Button> pyguide_button;
    std::deque<std::shared_ptr<Button>> pyguide_items;

    //The gameplay buttons for the gui displayed on the screen
    //created by GameEngine
    std::deque<std::shared_ptr<Button>> buttons;
    //While cycling through sprites, this is the index of the first button on the visible page
    unsigned int display_button_start;
    //A button used to cycle through the sprite heads
    std::shared_ptr<Button> cycle_button;

    void open_pause_window();
	void close_pause_window();
	void config_gui();

public:

    GUIMain(GameWindow * embedWindow);
    ~GUIMain();

	std::shared_ptr<GUIWindow> get_gui_window(){
		return gui_window;
	}

	GUIManager * get_gui_manager(){
		return &gui_manager;
	}

	NotificationBar * get_notification_bar(){
		return notification_bar;
	}

	MapViewer * get_map_viewer(){
		return &map_viewer;
	}

	void refresh_gui();
    void add_button(std::string file_path, std::string name, std::function<void (void)> callback);
};

#endif
