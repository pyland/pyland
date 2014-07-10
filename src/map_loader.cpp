#include "map_loader.hpp"

#include <Tmx.h>

#include <string>
#include <iostream>

void load_map(const std::string source) {
    Tmx::Map *map  = new Tmx::Map();
    map->ParseFile(source);

    if(map->HasError()) {
        std::cerr << "ERROR" << std::endl;
    }
}
