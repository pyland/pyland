#include "print_debug.hpp"
#include "engine_api.hpp"
#include "map_viewer.hpp"
#include "challenge1.hpp"

//TODO: later this will be fetched from the map
std::vector <std::pair<int, int>> target = { std::make_pair(4,15), std::make_pair(5,15), std::make_pair(6,15), std::make_pair(7,15), std::make_pair(8,15), std::make_pair(9,15), std::make_pair(10,15) };

void dialogue (std::string name, std::string text_to_user) {
    std::cout << name << " : " << text_to_user << std::endl;
} 

// void position_reset (int n) {
//     // EVENT_TODO: wait until R has been pressed
//     while (Engine::find_object(1) != target[n]) {
//         dialogue ("Tom",
//             "That's doesn't seem to have worked"
//             "I'll pop you back and you can try again");
//         Engine::move_object(1, target[n-1].first, target[n-1].second); //move player back to start
//         // EVENT_TODO: wait until script has finished
//         //PYTHON: need a way to know if a script is done
//     }
// }

void init_challenge() {
    //Coordinates are those from tiled
    // ENGINE_TODO: load map for challenge one, walls around the edges of the map
    dialogue ( "Tom",
        "Welcome to Project Zgyote"
        "My name is Tom and I am here to help you learn to move around"
        "Let's get started, press the N key to create a new character"
        "You can move Adam using arrow keys, try moving Adam to the first green marker on the path");

    // ENGINE_TODO: when (Engine::find_object(1) == target[1]) call
    // assume there is a dispatcher on adding objects to event manager

    Engine::get_map_viewer()->get_map()->event_step_on.register_callback(
        target[1],
        [] (int object_id) {
            int main_character = 1; //placeholder
            if (object_id == main_character) {
                intro_coding();
                return false;
            }
            return true;
        }
    );
}

void intro_coding() {
    dialogue ( "Tom",
        "Well Done, now we are going look at a new way of walking");

    // PY_TODO: open text editor window with c1_sample1.py

    dialogue ("Tom",
        "This text is used to control the movement of the Adam"
        "You can edit it, then save it and run it so that Adam follows your commands"
        "Let's begin, by pressing the R key to run the commands");
   
    // ENGINE_TODO: when (Engine::find_object(1) == target[2]) call

    Engine::get_map_viewer()->get_map()->event_step_on.register_callback(
        target[2],
        [] (int object_id) {
            int main_character = 1; //placeholder
            if (object_id == main_character) {
                first_user_program();
                return false;
            }
            return true;
        }
    );
}

void first_user_program() {

    dialogue ("Tom",
        "As you can see the move(north) command has moved Adam, north by 1 square"
        "Now we will edit this to get Adam to move to the next green marker"
        "Once complete, save the file and press R to the run again");

    // ENGINE_TODO: when (Engine::find_object(1) == target[3]) call
    Engine::get_map_viewer()->get_map()->event_step_on.register_callback(
        target[3],
        [] (int object_id) {
            int main_character = 1; //placeholder
            if (object_id == main_character) {
                incr_scale();
                return false;
            }
            return true;
        }
    ); 


}

void incr_scale() {

    dialogue ("Tom",
        "Well Done, your getting the hang of this now"
        "Next, lets get Adam to next green marker"
        "We will have to move Adam 10 times to the east");

    // ENGINE_TODO: when (Engine::find_object(1) == target[3]) call
    Engine::get_map_viewer()->get_map()->event_step_on.register_callback(
        target[4],
        [] (int object_id) {
            int main_character = 1; //placeholder
            if (object_id == main_character) {
                incr_scale2();
                return false;
            }
            return true;
        }
    );
}

void incr_scale2() {

    dialogue ("Tom",
        "That was exhausting !"
        "The next red box will require you to move to the east 20 times ..."
        "There's a new command known as a for loop, which we can use to repeat a collection of commands"
        "Type in 'for _ in range (1,20):' and on a new line type move(north)"
        "This will repeat the move(north) command 20 times"
        "Run it and see how it goes" );

    // ENGINE_TODO: when (Engine::find_object(1) == target[4]) call
    Engine::get_map_viewer()->get_map()->event_step_on.register_callback(
        target[5],
        [] (int object_id) {
            int main_character = 1; //placeholder
            if (object_id == main_character) {
                std::cout << "challenge completed" << std::endl;
                return false;
            }
            return true;
        }
    ); 
}
