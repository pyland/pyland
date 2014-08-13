#include "challenge_base.hpp"


ChallengeBase::ChallengeBase(Interpreter* _interpreter, GUIManager* _gui_manager, GameWindow* _game_window) :
    interpreter(_interpreter), gui_manager(_gui_manager),
    game_window(_game_window)
{
    map = new Map("../resources/map0.tmx");
    
    // create first character, TODO: move this into challenge
    CallbackState callbackstate(interpreter, "John");

}

Challenge::~Challenge() {
    delete map;
    
}

void ChallengeBase::create_gui() {

    //TODO : REMOVE THIS HACKY EDIT - done for the demo tomorrow
    TextFont buttonfont = Engine::get_game_font();
    Text stoptext(&window, buttonfont, true);
    Text runtext(&window, buttonfont, true);
    stoptext.set_text("Stop");
    runtext.set_text("Run");
    // referring to top left corner of text window
    stoptext.move(105, 240 + 20);
    runtext.move(5, 240 + 20);
    stoptext.resize(window.get_size().first-20, 80 + 20);
    runtext.resize(window.get_size().first-20, 80 + 20);

    std::shared_ptr<GUIWindow> sprite_window = std::make_shared<GUIWindow>();;
    sprite_window->set_width_pixels(300);
    sprite_window->set_height_pixels(300);
    std::shared_ptr<Button> run_button = std::make_shared<Button>();
    run_button->set_text("Run");
    run_button->set_on_click([&] () { LOG(ERROR) << "RUN"; callbackstate.restart(); });
    run_button->set_width(0.2f);
    run_button->set_height(0.2f);
    run_button->set_y_offset(0.8f);
    run_button->set_x_offset(0.0f);

    std::shared_ptr<Button> stop_button = std::make_shared<Button>();
    stop_button->set_text("Stop");
    stop_button->set_on_click([&] () {LOG(ERROR) << "STOP";  callbackstate.stop(); });
    stop_button->set_width(0.2f);
    stop_button->set_height(0.2f);
    stop_button->set_y_offset(0.8f);
    stop_button->set_x_offset(0.8f);

    // build navigation bar buttons
    NotificationBar notification_bar;
    Engine::set_notification_bar(&notification_bar);
    SpriteSwitcher sprite_switcher;

    sprite_window->add(run_button);
    sprite_window->add(stop_button);
    for (auto button: notification_bar.get_navigation_buttons()) {
        sprite_window->add(button);
    }

    Engine::set_gui_window(sprite_window);
    gui_manager.set_root(sprite_window);

    // quick fix so buttons in correct location in initial window before gui_resize_func callback
    auto original_window_size = game_window.get_size();
    sprite_window->set_width_pixels(original_window_size.first);
    sprite_window->set_height_pixels(original_window_size.second);

    gui_manager.parse_components();

    std::function<void(GameWindow*)> gui_resize_func = [&] (GameWindow* game_window) {
        LOG(INFO) << "GUI resizing";
        auto window_size = (*game_window).get_size();
        sprite_window->set_width_pixels(window_size.first);
        sprite_window->set_height_pixels(window_size.second);
        gui_manager.parse_components();
    };
    Lifeline gui_resize_lifeline = window.register_resize_handler(gui_resize_func);


    // WARNING: Fragile reference capture
    Lifeline map_resize_lifeline = window.register_resize_handler([&] (GameWindow *) {
        map_viewer.resize();
    });


}
