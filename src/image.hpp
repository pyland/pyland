#ifndef IMAGE_H
#define IMAGE_H

#include <exception>

extern "C" {
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
}

#include "lifeline.hpp"



///
/// Mutable image which can be constructed from a file
///
struct Image {
private:
    Lifeline resource_lifeline;

    ///
    /// Allocate memory for the pixel array.
    ///
    void create_blank(int w, int h);
    
    ///
    /// Perform the loading of image data from a file.
    ///
    void load_file(const char* filename);
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
    
    ///
    /// An RGBA pixel
    ///
    struct Pixel {
    public:
        Pixel();
        Uint8 r;
        Uint8 g;
        Uint8 b;
        Uint8 a;
    };

    /*
    ///
    /// A row of pixels.
    ///
    struct PixelRow {
    private:
        Pixel* pixels;
    public:
        inline Pixel& operator[](int x) {
            return *(&pixels[x]);
        }
    };
    */
    
    ///
    /// Width of the used image area.
    ///
    int width;
    ///
    /// Height of the used image area.
    ///
    int height;
    ///
    /// Width of the allocated image area.
    ///
    int store_width;
    ///
    /// Height of the allocated image area.
    ///
    int store_height;

    ///
    /// RGBA Pixels of image
    ///
    Pixel* pixels;

    ///
    /// Whether the image width and height should be forced to powers
    /// of two.
    ///
    bool power_of_two;
    ///
    /// Flip the image (interchange high and low y values).
    ///
    bool flipped;

    Image(const char* filename, bool opengl = true);
    ~Image();

    ///
    /// Index a row in the image.
    ///
    /// Can use [y][x] to get a pixel.
    ///
    inline Pixel* operator[](int y) {
        return &pixels[y * store_width];
    }
    /*
    inline PixelRow& operator[](int y) {
        return *( (PixelRow*)&pixels[y * store_width] );
    }
    */
};



#endif
