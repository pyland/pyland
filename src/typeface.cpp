#include <stdexcept>
#include <iostream>

#include <glog/logging.h>

extern "C" {
#include <SDL2/SDL_ttf.h>
}

#include "typeface.hpp"



// Need to inherit constructors manually.
// NOTE: This will, and are required to, copy the message.
Typeface::LoadException::LoadException(const char *message): std::runtime_error(message) {}
Typeface::LoadException::LoadException(const std::string &message): std::runtime_error(message) {}


Typeface::Typeface(std::string filename):
    filename(filename) {
    if (!TTF_WasInit() && TTF_Init() == -1) {
        LOG(ERROR) << "Failure initializing SDL_ttf: " << TTF_GetError();
        throw Typeface::LoadException("Failure initializing SDL_ttf");
    }
}


Typeface::~Typeface() {
}
