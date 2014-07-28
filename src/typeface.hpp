#ifndef TYPEFACE_H
#define TYPEFACE_H

#include <string>
#include <stdexcept>



///
/// Stores typeface information, used to create a font.
///
/// Due to the workings of SDL_ttf, this mearly contains a path to a
/// ttf file.
///
class Typeface {
private:
    friend class TextFont;
    ///
    /// Path to the ttf file.
    ///
    std::string filename;
public:
    ///
    /// Represents a failure in loading
    ///
    class LoadException: public std::runtime_error {
    public:
        LoadException(const char  *message);
        LoadException(const std::string &message);
    };
    
    Typeface(std::string filename);
    ~Typeface();
};



#endif
