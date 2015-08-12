#include <glog/logging.h>
#include <unordered_map>

#include "renderable_component.hpp"
#include "sprite_manager.hpp"
#include "shader.hpp"
#include "texture_atlas.hpp"

std::unordered_map<std::string, std::shared_ptr<RenderableComponent>> SpriteManager::sprite_map;

std::shared_ptr<RenderableComponent> SpriteManager::get_component(std::string filepath){

	if(sprite_map.count(filepath) == 0){

		LOG(INFO) << "Creating a sprite in cache for " << filepath;

		std::shared_ptr<RenderableComponent> new_component = std::make_shared<RenderableComponent>();
		new_component->set_texture(TextureAtlas::get_shared(filepath));
		sprite_map[filepath] = new_component;

		std::shared_ptr<Shader> shader;
		try {
			shader = Shader::get_shared("tile_shader");
		}
		catch (std::exception e) {
			LOG(ERROR) << "Failed to create the shader";
		}

		//Set the shader
		new_component->set_shader(shader);
	}

	return sprite_map[filepath];

}
