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
        virtual ~Challenge() = default;

        virtual void start() = 0;
        virtual void finish() = 0;
};

#endif