#include <iostream>
#include <stdexcept>

#include <glog/logging.h>

extern "C" {
#include "SDL2/SDL_ttf.h"
}

#include "text_font.hpp"
#include "typeface.hpp"


// Need to inherit constructors manually.
// NOTE: This will, and are required to, copy the message.
TextFont::LoadException::LoadException(const char *message): std::runtime_error(message) {}
TextFont::LoadException::LoadException(const std::string &message): std::runtime_error(message) {}


TextFont::TextFont(Typeface face, int size) {
    TTF_Font* font = TTF_OpenFont(face.filename.c_str(), size);

    if (font == nullptr) {
        LOG(ERROR) << "Unable to open font from file \"" << face.filename << "\" at size " << size << ": " << TTF_GetError();
        throw TextFont::LoadException("Unable to open font");
    }

    this->font = font;

    font_lifeline = Lifeline([font] () {
            TTF_CloseFont(font);
        });
}

TextFont::~TextFont() {
}
