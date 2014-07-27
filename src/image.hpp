#ifndef IMAGE_H
#define IMAGE_H

#include <exception>

extern "C" {
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
}

#include "lifeline.hpp"



///
/// Mutable RGBA image which can be constructed from a file
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
        Uint8 r;
        Uint8 g;
        Uint8 b;
        Uint8 a;
        
        Pixel();
    };
    
    ///
    /// Flipped pixel wrapper
    ///
    struct Flipper {
    public:
        ///
        /// Negative value. jump = - store_width.
        ///
        int jump;
        ///
        /// Pointer to bottom-left corner of pixel data.
        ///
        /// Equivilant to [(store_height - 1) * store_width].
        ///
        Pixel* pixels;
        Flipper();
        ///
        /// Initialises the pixel array and jump value.
        ///
        /// @param w store_width
        /// @param h store_height
        /// @param p non-flipped pixels
        Flipper(int w, int h, Pixel* p);
        ///
        /// Index a row in the flipped image.
        ///
        /// Note, that with increasing y, this goes back in the array.
        ///
        inline Pixel* operator[](int y) {
            return &pixels[y * jump];
        }
    };
    Flipper flipped_pixels;
    
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

    ///
    /// Create an empty image (everything initialised to defaults).
    ///
    Image();
    Image(const char* filename, bool opengl = true);
    Image(int width, int height, bool opengl = true);
    ~Image();

    void clear(Uint32 colour, Uint32 mask);

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
