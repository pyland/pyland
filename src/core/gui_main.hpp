#ifndef GUI_MAIN_H
#define GUI_MAIN_H

#include <memory>
#include <deque>
#include <utility>

#include "button.hpp"
#include "config.hpp"
#include "event_manager.hpp"
#include "gui_manager.hpp"
#include "gui_window.hpp"
#include "map_viewer.hpp"
#include "text_box.hpp"

///
/// The entire GUI, which consists of all the components in it.
/// Created by GameMain and contained in it
///

typedef Button Board;

class GUIMain {

private:

    ///refer to the config.jsonnet under "scales" for these variables:

    ///****************Config variables start
    float x_scale;
    float y_scale;

    float left_x_offset;
    float right_x_offset;
    float bottom_y_offset;
    float top_y_offset;

    float close_x_offset;
    float close_y_offset;

    float title_x_offset;
    float title_y_offset;
    float pyguide_title_x_offset;
    float pyguide_title_y_offset;

    float pause_x_offset;
    float pause_y_offset;

    float menu_x_offset;
    float menu_y_offset;
    float menu_width;
    float menu_height;
    float menu_spacing;
    unsigned int menu_max;

    float notification_width;
    float notification_height;

    float notification_text_width;
    float notification_text_height;
    float notification_text_x;
    float notification_text_y;
    unsigned int notification_text_buffer;

    float notification_button_width;
    float notification_button_height;
    float notification_button_x;
    float notification_button_y;

    unsigned int option_max;
    float option_width;
    float option_height;
    float option_x;
    float option_y;
    float option_button_width;
    float option_button_height;
    float option_button_x;
    float option_button_y;
    float option_button_text_x;
    float option_button_text_y;
    float option_button_spacing;

    float py_help_width;
    float py_help_height;
    float py_help_x;
    float py_help_y;

    float py_help_item_width;
    float py_help_item_height;
    float py_help_item_x;
    float py_help_item_y;
    float py_help_item_spacing;
    float py_help_item_text_x;
    float py_help_item_text_y;

    float py_help_text_width;
    float py_help_text_height;
    float py_help_text_x;
    float py_help_text_y;
    unsigned int py_help_text_buffer;

    float py_help_button_width;
    float py_help_button_height;
    float py_help_button_x;
    float py_help_button_y;
    float py_help_button_spacing;

    float menu_move_width;
    float menu_move_height;
    float menu_move_x;
    float menu_move_y;
    float menu_move_spacing;
    float menu_page_display_x;
    float menu_page_display_y;

    float button_width;
    float button_height;

    float horizontal_button_spacing;
    float vertical_button_spacing;

    unsigned int button_max;

    unsigned int py_apis_num;
    ///****************Config variables end

    GameWindow * embedWindow;
    GUIManager gui_manager;
    MapViewer map_viewer;
    EventManager *em;

    std::shared_ptr<GUIWindow> gui_window;

    bool bar_open; //whether or not the notification bar is open
    bool external_help_open;
    bool callback_options; //whether or not there are options at the end of the notification bar
    void create_notification_bar();
    std::shared_ptr<TextBox> notification_bar;
    std::shared_ptr<TextBox> external_script_help;

    std::function<void ()> notification_func; // the function to be called after the bar is closed
    std::deque<std::pair<std::string, std::function<void ()> > > notification_options;

    unsigned int option_start;
    unsigned int option_selected;
    std::shared_ptr<Board> options_box;
    std::deque<std::shared_ptr<Button>> option_buttons;
    std::shared_ptr<Button> next_option;
    std::shared_ptr<Button> prev_option;

    void create_pause_menu();
    bool pause_open; //whether or not the pause window is open
    std::shared_ptr<Button> pause_button; //The button to pause the game
    void open_pause_window();
    void close_pause_window();

    std::shared_ptr<Button> exit_button;
    std::shared_ptr<Button> music_button;
    std::shared_ptr<Button> sound_button;

    void create_bag();
    bool bag_open; //whether or not the bag is open
    std::shared_ptr<Button> bag_button;
    std::shared_ptr<Board> bag_window;
    std::shared_ptr<Button> pyguide_button;
    std::deque<std::shared_ptr<Button>> bag_items;
    void open_bag();
    void close_bag();

    void create_pyguide();
    std::shared_ptr<Board> pyguide_window;
    std::shared_ptr<TextBox> py_help;
    std::deque<std::string> pyguide_explanations;
    std::deque<std::shared_ptr<Button>> pyguide_commands;
    std::shared_ptr<Button> pyguide_next_button;
    std::shared_ptr<Button> pyguide_back_button;
    std::shared_ptr<Board> pyguide_page_display;
    std::pair<unsigned int, unsigned int> pyguide_page; //page first out of second
    void open_pyguide();
    void close_pyguide();

    //The gameplay buttons for the gui displayed on the screen
    //created by GameEngine
    std::deque<std::shared_ptr<Button>> buttons;
    //Store each button index at the 'id'th element, so can map directly
    //from button id to index (so can get the button from buttons).
    //This is because the ids are passed through an event queue (and not directly), using the
    //id directly because an index can't be assumed
    std::deque<unsigned int> button_indices;
    //The index for the currently highlighted player
    unsigned int cur_button_index;
    //While cycling through sprites, this is the index of the first button on the visible page
    unsigned int display_button_start;
    //A button used to cycle through the sprite heads
    std::shared_ptr<Button> cycle_button;

    //Gets the config variables from config.jsonnet
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

    std::shared_ptr<TextBox> get_notification_bar(){
        return notification_bar;
    }

    MapViewer * get_map_viewer(){
        return &map_viewer;
    }

    //Add a player focus button to the screen (called from game engine)
    //file path is the location of the image in gui.png, and name is the text to be displayed with the button
    //callback is added to the event manager when the button is pressed
    void add_button(std::string file_path, std::string name, std::function<void (void)> callback, unsigned int button_id);

    //Cycle onto the next set of 'button_max' player focus buttons
    //This can only occur when there are more than 'button_max' players
    void cycle();


    //To add a message to the notification bar
    void add_message(std::string text);
    void add_text(std::string text);

    //To open and close the notification bar, func is the callback function to be called after the user finished reading the notification
    void open_notification_bar(std::function<void ()> func);
    void open_notification_bar_with_options(std::deque<std::pair<std::string, std::function<void ()> > > options);
    void proceed_notification_bar();
    void close_notification_bar();

    void show_external_script_help(std::string);
    void proceed_external_script_help();
    void close_external_script_help();

    //get whether or not the bar is open -required if show_dialogue is run consecutively
    bool is_bar_open() {return bar_open;}
    bool is_external_help_open() {return external_help_open;}

    //This is used to render the components to the screen after any changes have made to the gui
    void refresh_gui();

    //Update the currently selected player focus button index
    void set_button_index(unsigned int value);

    //Simulate clicking on the next player's focus button
    //(for when the user is tabbing through players)
    void click_next_player();

    //Simulate clicking on the player button with the given button_id
    //(for when the focus is changed to a player in python code)
    void click_player(unsigned int button_id);

    //Update the text for the button of given id
    //(for when a player's name is changed in python)
    void update_button_text(std::string name, unsigned int button_id);

    //Update GUI buttons to distinguish the currently selected sprite from the others
    void update_selected();

};
#endif
