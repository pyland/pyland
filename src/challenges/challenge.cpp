#include <string>
#include <iostream>

#include "challenge.hpp"

Challenge::Challenge(std::string editor, Text* text_displayer): editor(editor), text_displayer(text_displayer) {}

void Challenge::dialogue(std::string name, std::string text_to_user) {
    // TODO: Use rendered fonts
    (*text_displayer).set_text(text_to_user);
    (*text_displayer).display();
    std::cout << name << "  says:\n" << text_to_user << std::endl;
} 
