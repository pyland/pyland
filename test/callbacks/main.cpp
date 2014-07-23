#include <iostream>

extern "C" {
#include <SDL.h>
  
#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif

#ifdef USE_GL
#include <GL/gl.h>
#endif
}


#include "game_window.hpp"
#include "input_manager.hpp"
#include "callback.hpp"
#include "lifeline.hpp"
#include "filters.hpp"

#include "keyboard_input_event.hpp"
#include "mouse_input_event.hpp"



void callback_function(const char* action, KeyboardInputEvent event) {
    std::cerr << "\t" << event.scan_code << ":" << action;
}

void mouse_callback_function(MouseInputEvent event) {
    std::cerr << "Mouse:" << std::endl;
    std::cerr << "\tStart:\t" << event.start.x << "\t" << event.start.y << "\t" << event.start.buttons << std::endl;
    std::cerr << "\tFrom:\t"  << event.from.x  << "\t" << event.from.y  << "\t" << event.from.buttons  << std::endl;
    std::cerr << "\tTo:\t"    << event.to.x    << "\t" << event.to.y    << "\t" << event.to.buttons    << std::endl;
    std::cerr << "\tButton:\t" << event.button << "\t" << event.was_down << "\t" << event.down << std::endl;
}



int main(int argc, char** argv) {
    std::cerr << "Startup..." << std::endl;
    
    try {
        GameWindow window = GameWindow(640, 400, false);
        InputManager* input_manager = window.get_input_manager();
        window.use_context();
        glClearColor(0.25f, 0.50f, 1.0f, 1.0f);
        float r, g, b;
        r = g = b = 0.0f;
        Callback<void, KeyboardInputEvent> release_callback([&] (KeyboardInputEvent event) {
                callback_function(".#.", event);
            });
        Callback<void, KeyboardInputEvent> down_callback([&] (KeyboardInputEvent event) {
                callback_function("...", event);
            });
        Callback<void, KeyboardInputEvent> type_callback([&] (KeyboardInputEvent event) {
                callback_function(".*.", event);
            });
        Callback<void, KeyboardInputEvent> press_callback([&] (KeyboardInputEvent event) {
                if (event.key_code == SDLK_ESCAPE) {
                    release_callback.unregister_everywhere();
                    down_callback.unregister_everywhere();
                    type_callback.unregister_everywhere();
                    press_callback.unregister_everywhere();
                }
                else {
                    callback_function("._.", event);
                }
            });
        {
            Lifeline press_lifeline = input_manager->register_key_press_handler([&] (KeyboardInputEvent event) {
                    callback_function("!!!", event);
                });
        }
        input_manager->register_key_release_handler(release_callback);
        input_manager->register_key_down_handler(down_callback);
        input_manager->register_key_type_handler(type_callback);
        input_manager->register_key_press_handler(press_callback);
        
        // Lifeline mouse_lifeline = input_manager->register_mouse_handler(&mouse_callback_function);
        Lifeline mouse_button_lifeline = input_manager->register_mouse_handler(filter({ANY_OF({MOUSE_PRESS, MOUSE_RELEASE})}, &mouse_callback_function));
        while (window.check_close() == false) {
            // Little basic colour change test.
            r += 0.001;
            g += 0.01;
            b += 0.1;
            if (r >= 1.0f) r = 0.0f;
            if (g >= 1.0f) g = 0.0f;
            if (b >= 1.0f) b = 0.0f;
            glClearColor(r, g, b, 0.8f);
            glClear(GL_COLOR_BUFFER_BIT);
            window.swap_buffers();
            SDL_Delay(0);
            GameWindow::update();
            if (input_manager->is_key_down(SDLK_ESCAPE) && input_manager->is_key_down(SDLK_LSHIFT)) {
                std::cerr << "Shift+Escape pressed!" << std::endl;
                break;
            }
        }
	release_callback.unregister_everywhere();
	down_callback.unregister_everywhere();
	type_callback.unregister_everywhere();
	press_callback.unregister_everywhere();
    }
    catch (GameWindow::InitException e) {
        std::cerr << e.what() << std::endl;
    }

    std::cerr << "Terminate" << std::endl;
    
    return 0;
}
