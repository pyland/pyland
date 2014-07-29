#ifndef CHALLENGE_H
#define CHALLENGE_H

#include <string>

#include "typeface.hpp"
#include "text_font.hpp"
#include "text.hpp"

class InputManager;

class Challenge {
    public:
        // TODO: Make the editor part of the core
        // engine, not the challenge class
        Challenge(InputManager *input_manager);
        virtual ~Challenge();

        virtual void start() = 0;
        virtual void finish() = 0;

    	InputManager *input_manager;
};

#endif