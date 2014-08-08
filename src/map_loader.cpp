#include "map_loader.hpp"

#include "layer.hpp"
#include "map_object.hpp"
#include "tileset.hpp"

#include <glog/logging.h>
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

    LOG(INFO) << "Loading map";
    map.ParseFile(source);

    if (map.HasError()) {
        LOG(ERROR) << map.GetErrorCode() << " " << map.GetErrorText();
        return false;
    }

    map_width = map.GetWidth();
    map_height = map.GetHeight();

    load_objects();
    load_layers();
    load_tileset();

    return true;
}

void MapLoader::load_layers() {
    for (int i = 0; i < map.GetNumLayers(); ++i) {
        //Get the layer
        const Tmx::Layer* layer = map.GetLayer(i);
        int num_tiles_x = layer->GetWidth();
        int num_tiles_y = layer->GetHeight();
        std::string name = layer->GetName();

        //Generate a new layer
        std::shared_ptr<Layer> layer_ptr = std::make_shared<Layer>(num_tiles_x, num_tiles_y, name);
        layers.push_back(layer_ptr);

        //Get the tiles
        //The TMX le has origin in top left, ours id bottom right
        for (int y = num_tiles_y - 1; y >= 0; --y) {
            for (int x = 0; x < num_tiles_x; ++x) {
                //Get the tile identifier
                int tile_id = layer->GetTileId(x, y);

                //Gets the tileset to use for this tile
                int tileset_index = layer->GetTileTilesetIndex(x, y);
                if(tileset_index == -1) {
                    //Add the default tile
                    layer_ptr->add_tile("", tile_id);
                    continue;
                }
                const Tmx::Tileset* tileset = map.GetTileset(tileset_index);

                //Add the tile to the layer
                layer_ptr->add_tile(tileset->GetName(), tile_id);
            }
        }
    }
}

void MapLoader::load_objects() {
    for (int i = 0; i < map.GetNumObjectGroups(); ++i) {
        //Get an object group: Effecitively a map layer but just for objects
        const Tmx::ObjectGroup *object_group = map.GetObjectGroup(i);

        //Get all the objects in this object group
        for (int j = 0; j < object_group->GetNumObjects(); ++j) {

            //Get an object
            const Tmx::Object* object = object_group->GetObject(j);

            //Get object properties
            const std::string name = object->GetName();

            LOG(INFO) << "Loading object: " << object->GetName();

            //Build a MapObject
            std::shared_ptr<MapObject> map_object = std::make_shared<MapObject>();
            map_object->set_name(name);
            map_object->set_position(glm::vec2(object->GetX(), object->GetY()));

            //Add it to the objects list
            objects.push_back(map_object);
        }
    }
}

void MapLoader::load_tileset() {

    //For all the tilesets
    for (int i = 0; i < map.GetNumTilesets(); ++i) {

        //Get a tileset
        const Tmx::Tileset *tileset = map.GetTileset(i);

        //Get the image name. This is the path relative to the TMX file
        const std::string tileset_name(tileset->GetName());
        int tileset_width = tileset->GetImage()->GetWidth();
        int tileset_height = tileset->GetImage()->GetHeight();

        //Create a new tileset and add it to the map
        std::shared_ptr<TileSet> map_tileset = std::make_shared<TileSet>(tileset_name, tileset_width, tileset_height);
        tilesets.push_back(map_tileset);

        //We use the tileset properties to define collidable tiles for our collision
        //detection
        if (tileset->GetTiles().size() > 0) {
            auto tile_list = tileset->GetTiles();
            for (auto tile_iter = tile_list.begin(); tile_iter != tile_list.end(); ++tile_iter) {
                //Get a tile from the tileset
                //const Tmx::Tile* tile = *tile_iter;


                //Get the properties
                //Used to handle collidable tiles
                // TODO: This
                // std::map<std::string, std::string> list = tile->GetProperties().GetList();
                // for (auto iter = list.begin(); iter != list.end(); ++iter){
                //     if (iter->first.c_str() == "Collidable") {
                //         if (iter->second.c_str() == "True") {
                //             LOG(INFO) << "Property " << iter->first.c_str() << " " << iter->second.c_str();
                //         }
                //     }
                // }
            }
        }
    }
}

