#ifndef TEXT_FONT_H
#define TEXT_FONT_H

#include <stdexcept>
#include <string>

extern "C" {
#include <SDL2/SDL_ttf.h>
}

#include "lifeline.hpp"



class Typeface;

///
/// Wrapper for SDL_ttf font.
///
class TextFont {
private:
    friend class Text;
    ///
    /// Destroy font when there are no more instances left.
    ///
    Lifeline font_lifeline;
    ///
    /// Underlying SDL font.
    ///
    TTF_Font* font;
public:
    ///
    /// Represents a failure in loading
    ///
    class LoadException: public std::runtime_error {
    public:
        LoadException(const char  *message);
        LoadException(const std::string &message);
    };
    
    TextFont(Typeface face, int size);
    ~TextFont();
};

#endif
