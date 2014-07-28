#include <string>
#include <iostream>

#include "challenge.hpp"

Challenge::Challenge(Text* text_displayer): text_displayer(text_displayer) {}

void Challenge::dialogue(std::string name, std::string text_to_user) {
    // TODO: Use rendered fonts
    std::string text_to_display = name + " : " + text_to_user;
    (*text_displayer).set_text(text_to_display);
    (*text_displayer).display();
    std::cout << name << "  says:\n" << text_to_user << std::endl;
} 
