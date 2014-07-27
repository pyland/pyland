#include <iostream>

#include <glog/logging.h>

extern "C" {
#include "SDL2/SDL_ttf.h"
}

#include "text_font.hpp"
#include "typeface.hpp"



TextFont::LoadException::LoadException(const char* message) {
    this->message = message;
}


const char* TextFont::LoadException::what() const noexcept {
    return message;
};



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
