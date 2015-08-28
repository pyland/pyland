#include <glog/logging.h>
#include <utility>

#include "event_manager.hpp"
#include "input_handler.hpp"

InputHandler::InputHandler() {

}

InputHandler::~InputHandler() {
    flush_all(); //Clear all the lists
}

InputHandler *InputHandler::get_instance() {
    // Lazy instantiation of the global instance
    static InputHandler global_instance;

    return &global_instance;
}

void InputHandler::run_list(int input_key) {
    function_list *fl = grab_list(input_key);
    for (function_list::const_iterator iterator = fl->begin(); iterator != fl->end(); ++iterator) {
        EventManager::get_instance()->add_event(*iterator);
    }
    return;
}

void InputHandler::flush_list(int input_key) {
    function_list *fl = grab_list(input_key); //grab the list associated against an input key
    fl->clear();
}

void InputHandler::flush_all() {
    //TODO: make more effiecient!
    //iterate throught the whole map and delete all the function lists.
    for (std::unordered_map<int, function_list*>::const_iterator iterator = event_map.begin(); iterator != event_map.end(); ++iterator) {
        delete iterator->second;
    }
    event_map.erase(event_map.begin(), event_map.end()); //erase the keys from the map
}

void InputHandler::register_input_callback(int input_key, std::function<void()> input_callback) {
    function_list *fl = grab_list(input_key); //grab the list associated against an input key
    fl->push_back(input_callback); //push the input callback on that list
    return;
}

std::list<std::function<void()>> *InputHandler::grab_list(int input_key) {
    function_list *fl = nullptr; //declare a pointer to a funcit list

    //try to find the correct input key
    std::unordered_map<int, function_list*>::const_iterator mit = event_map.find(input_key);

    if (mit == event_map.end()) { //if the input key isn't in the map, then create a new function list and register it against the correct key in the map
        fl = new function_list();
        std::pair<int, function_list*> p;
        p.first = input_key;
        p.second = fl;
        //p = std::make_pair<int, function_list*>(input_key, fl);
        event_map.insert(p);
    } else { //if the key is in the map, return the correct list
        fl = mit->second;
    }
    return fl;
}


