#ifndef FONT_H
#define FONT_H

#include <memory>
#include <exception>

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
    class LoadException: public std::exception {
    protected:
        const char* message;
    public:
        LoadException(const char* message);
        virtual const char* what() const noexcept;
    };
    
    TextFont(Typeface face, int size);
    ~TextFont();
};

#endif
