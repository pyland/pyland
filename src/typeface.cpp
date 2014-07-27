#include <exception>
#include <iostream>

extern "C" {
#include <SDL2/SDL_ttf.h>
}

#include "typeface.hpp"



Typeface::LoadException::LoadException(const char* message) {
    this->message = message;
}


const char* Typeface::LoadException::what() const noexcept {
    return message;
};



Typeface::Typeface(std::string filename):
    filename(filename) {
    if (!TTF_WasInit() && TTF_Init() == -1) {
        std::cerr << "Failure initializing SDL_ttf: " << TTF_GetError() << std::endl;
        throw Typeface::LoadException("Failure initializing SDL_ttf");
    }
}


Typeface::~Typeface() {
}
