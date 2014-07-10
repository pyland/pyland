#include "map_loader.hpp"

#include <array>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <Tmx.h>
    
///
/// See: https://github.com/bjorn/tiled/wiki/TMX-Map-Format
///      https://code.google.com/p/tmx-parser
///
/// NOTE
/// We don't support the TMX file format fully. The TmxParser library
/// can fully parse the files but we don't make use of all of these
/// attributes and have only parsed them if we need them.
///


bool MapLoader::load_map(const std::string source) {
    map.ParseFile(source);

    if (map.HasError()) {
        std::cerr << "ERROR" << map.GetErrorCode() << " " << map.GetErrorText().c_str() << std::endl;
        return false;
    }

    int map_width = map.GetWidth();
    int map_height = map.GetHeight();
    return true;
}

void MapLoader::load_layers() {
    for (int i = 0; i < map.GetNumLayers(); ++i) {
        //Get the layer
        const Tmx::Layer* layer = map.GetLayer(i);
        int num_tiles_x = layer->GetWidth();
        int num_tiles_y = layer->GetHeight();
        
        //Generate a new layer
        shared_ptr<Layer> layer_ptr = make_shared<Layer>(num_tiles_x, num_tiles_y);
        shared_ptr<int> layer_tiles_ptr = make_shared<std::array<int>>
        

        for (int y = 0; y < num_tiles_y; ++y) {
            for (int x = 0; x < num_tiles_x; ++x) {
                //Get the tile identifier
                int tile_id = layer->GetTileId(x, y);

                std::cout <<"TILEID: " << tile_id << std::endl;


                //Gets the tileset to use for this tile
                //TODO: We only support one tileset at the moment
                const Tmx::Tileset* tileset = map.FindTileset(tile_id);


            } 
        }
    }
}

void MapLoader::load_objects() {
    for (int i = 0; i < map.GetNumObjectGroups(); ++i) {
        //Get an object group: Effecitively a map layer but just for objects
        const Tmx::ObjectGroup* object_group = map.GetObjectGroup(i);
        
        //Get all the objects in this object group
        for (int j = 0; j < object_group->GetNumObjects(); ++j) {
            //Get an object
            const Tmx::Object* object = object_group->GetObject(j);
            
            //Get object properties
            const std::string name = object->GetName().c_str();

            int object_x = object->GetX();
            int object_y = object->GetY();
            
            std::cout << "OBJECT : " << object->GetName() << std::endl;
        }
    }
}

void MapLoader::load_tileset() {

    //For all the tilesets
    for (int i = 0; i < map.GetNumTilesets(); ++i) {

        //Get a tileset
        const Tmx::Tileset *tileset = map.GetTileset(i);
        
        //Get the image name. This is relative to the TMX file
        const std::string tileset_name(tileset->GetName().c_str());
        int tileset_width = tileset->GetImage()->GetWidth();
        int tileset_height = tileset->GetImage()->GetHeight();
        

        //Now load the image
        //TODO: Just using .raw at the moment. Use .png in the editor but then use 
        //      an image converter to convert this to .raw
        
        //We use the tileset properties to define collidable tiles for our collision 
        //detection
        if (tileset->GetTiles().size() > 0) {
            //Get a tile from the tileset
            const Tmx::Tile* tile = *(tileset->GetTiles().begin());
            
            //Get the properties
            std::map<std::string, std::string> list = tile->GetProperties().GetList();
            for (auto iter = list.begin(); iter != list.end(); ++iter){
                if (iter->first.c_str() == "Collidable") {
                    if (iter->second.c_str() == "True") {
                        std::cout << "COLLIDABLE : " << std::endl;
                    }
                }
            }
            
        }


    }

    
}
