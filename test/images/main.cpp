#include <iostream>

extern "C" {
#include <cstdio>
}

#include "image.hpp"



int main() {
    Image image("test.png", true);

    for (int y = 0; y < image.store_height; y++) {
        for (int x = 0; x < image.store_width; x++) {
            printf ("%x\t%x\t%x\t%x\t", image[y][x].r, image[y][x].g, image[y][x].b, image[y][x].a);
        }
        printf ("\n");
    }
    
    return 0;
}
