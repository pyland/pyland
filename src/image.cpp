#include <cstring>
#include <stdexcept>
#include <glog/logging.h>
#include <iostream>
#include <new>

extern "C" {
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
}

#include "image.hpp"
#include "lifeline.hpp"


// Need to inherit constructors manually.
// NOTE: This will, and are required to, copy the message.
Image::LoadException::LoadException(const char  *message): std::runtime_error(message) {}
Image::LoadException::LoadException(const std::string &message): std::runtime_error(message) {}



Image::Flipper::Flipper():
    jump(0),
    pixels(nullptr) {
}

Image::Flipper::Flipper(int w, int h, Pixel* p):
    jump(-w),
    pixels(&p[w*(h-1)]) {
}



Image::Pixel::Pixel():
    r(0),
    g(0),
    b(0),
    a(0) {
}



Image::Image():
    flipped_pixels(Flipper(0,0,nullptr)) {
}

Image::Image(const char* filename, bool opengl) {
    if (opengl) {
        power_of_two = true;
        flipped = true;
    }
    else {
        power_of_two = false;
        flipped = false;
    }

    load_file(filename);
}

Image::Image(int width, int height, bool opengl) {
    if (opengl) {
        power_of_two = true;
        flipped = true;
    }
    else {
        power_of_two = false;
        flipped = false;
    }

    create_blank(width, height);
}


Image::~Image() {
}


void Image::create_blank(int w, int h) {
    width = w;
    height = h;

    if (power_of_two) {
        for (store_width  = 1; store_width  < width ; store_width  <<= 1);
        for (store_height = 1; store_height < height; store_height <<= 1);
    }
    else {
        store_width  = width;
        store_height = height;
    }

    Pixel* pixels_local = new Pixel[store_width * store_height];
    this->pixels = pixels_local;
    flipped_pixels = Flipper(store_width, store_height, pixels);
    resource_lifeline = Lifeline([pixels_local] () {delete pixels_local;});
}


void Image::load_file(const char* filename) {
    // The image is loaded into this surface.
    SDL_Surface* loaded;
    // A surface with a known format to blit the loaded image on to.
    SDL_Surface* compatible;

    if ((IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG) == 0) {
        LOG(WARNING) << "Warning: Failure initialising image subsystem: " << IMG_GetError();
    }
    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == 0) {
        LOG(WARNING) << "Warning: Failure initialising image subsystem: " << IMG_GetError();
    }
    if ((IMG_Init(IMG_INIT_TIF) & IMG_INIT_TIF) == 0) {
        LOG(WARNING) << "Warning: Failure initialising image subsystem: " << IMG_GetError();
    }
    
    loaded = IMG_Load(filename);

    if (loaded == nullptr) {
        std::stringstream error_message;
        error_message << "Error loading image \"" << filename << "\" " << IMG_GetError();

        throw Image::LoadException(error_message.str());
    }
    
    // This surface has a known format.
    compatible = SDL_CreateRGBSurface(0, // Unsed
                                      loaded->w,
                                      loaded->h,
                                      32, // 32 bit
#if SDL_BYTE_ORDER == SDL_BIG_ENDIAN
                                      0xff000000,
                                      0x00ff0000,
                                      0x0000ff00,
                                      0x000000ff
#else
                                      0x000000ff,
                                      0x0000ff00,
                                      0x00ff0000,
                                      0xff000000
#endif
                                      );

    if (compatible == nullptr) {
        SDL_FreeSurface(loaded);
        pixels = nullptr;
        throw Image::LoadException("Failed to allocate space.");
    }
    
    SDL_BlitSurface(loaded, NULL, compatible, NULL);
    SDL_FreeSurface(loaded);
    
    try {
        create_blank(loaded->w, loaded->h);
    }
    catch (std::bad_alloc& e) {
        LOG(ERROR) << "Error loading image \"" << filename << "\": " << e.what();
        SDL_FreeSurface(compatible);
        throw e;
    }

    SDL_LockSurface(compatible);
    for (int y = 0; y < height; y++) {
        std::memcpy ((*this)[flipped ? (store_height-y-1) : y], &((Image::Pixel*)compatible->pixels)[y*compatible->w], 4 * compatible->w);
    }
    SDL_UnlockSurface(compatible);

    SDL_FreeSurface(compatible);

    // Errrrr... There isn't currently any logical place to put an
    // IMG_Quit()... It's not going to cause any problems, it's just a
    // bit unclean.
}


void Image::clear(Uint32 colour, Uint32 mask) {
    Uint32 invmask = ~mask;
    Uint8 ri = (invmask >> 24) & 0xff;
    Uint8 gi = (invmask >> 16) & 0xff;
    Uint8 bi = (invmask >>  8) & 0xff;
    Uint8 ai = (invmask >>  0) & 0xff;
    Uint8 rc = (colour  >> 24) & 0xff;
    Uint8 gc = (colour  >> 16) & 0xff;
    Uint8 bc = (colour  >>  8) & 0xff;
    Uint8 ac = (colour  >>  0) & 0xff;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            (*this)[y][x].r = rc | ((*this)[y][x].r & ri);
            (*this)[y][x].g = gc | ((*this)[y][x].g & gi);
            (*this)[y][x].b = bc | ((*this)[y][x].b & bi);
            (*this)[y][x].a = ac | ((*this)[y][x].a & ai);
        }
    }
}
