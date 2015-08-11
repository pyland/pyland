#ifndef SPRITE_MANAGER_H
#define SPRITE_MANAGER_H

#include <memory>
#include <unordered_map>

#include "renderable_component.hpp"

class SpriteManager{

private:
	static std::unordered_map<std::string, std::shared_ptr<RenderableComponent>> sprite_map;

public:
	static std::shared_ptr<RenderableComponent> get_component(std::string filepath);

};

#endif
