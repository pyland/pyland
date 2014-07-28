#ifndef CHALLENGE_H
#define CHALLENGE_H

#include <string>

#include "typeface.hpp"
#include "text_font.hpp"
#include "text.hpp"

class Challenge {
    public:
        // TODO: Make the editor part of the core
        // engine, not the challenge class
        Challenge(std::string editor, Text* text_displayer);
        virtual ~Challenge() = default;

        virtual void start() = 0;
        virtual void finish() = 0;

        // TODO: Make this function part of the core
        // engine, not the challenge class
        void dialogue (std::string name, std::string text_to_user);

    private:
        std::string editor;
        Text* text_displayer;
};

#endif