#include <string>
#include <iostream>

#include "challenge.hpp"

Challenge::Challenge(std::string editor): editor(editor) {}

void Challenge::dialogue(std::string name, std::string text_to_user) {
    // TODO: Use rendered fonts
    std::cout << name << "  says: " << text_to_user << std::endl;
} 
