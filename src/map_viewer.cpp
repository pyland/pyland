#include "character.hpp"
#include "map.hpp"
#include "map_viewer.hpp"
#include "object_manager.hpp"
#include "renderable_component.hpp"


#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

MapViewer::MapViewer(GameWindow* new_window) {
    if(new_window == nullptr) {
        std::cerr << "INVALID PASSING NULL GameWindow" << std::endl;
        assert(new_window != nullptr);
    }
    window = new_window;

    // Set background color and clear buffers
    glClearColor(0.15f, 0.25f, 0.35f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);
    // L./eave this here!!!
    //Disable back face culling.
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_SCISSOR_TEST);
};

MapViewer::~MapViewer() {

}

void MapViewer::render_map() {
    if(map == nullptr) {
        std::cerr << "ERROR: MAP is NULL in MapViewer::render_map" << std::endl;
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderableComponent* map_render_component = map->get_renderable_component();
    if(map_render_component == nullptr) {
        std::cerr << "ERROR: RenderComponent is NULL in MapViewer::render_map" << std::endl;
        return;
    }

    Shader* map_shader = map_render_component->get_shader();
    if(map_shader == nullptr) {
        std::cerr << "ERROR: Shader is NULL in MapViewer::render_map" << std::endl;
        return;
    }

    std::pair<int, int> size = window->get_size();
    //TODO, set the map view correctly
    glScissor(0, 0, size.first, size.second);
    map->set_display_width(size.first / 32);
    map->set_display_height(size.second / 32);
    glViewport(0, 0,  size.first, size.second);
    glm::mat4 projection_matrix = glm::ortho(0.0f, (float)(size.first), 0.0f, (float)(size.second), 0.0f, 1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 translate = glm::vec3(-map->get_display_x()*32.0f, -map->get_display_y()*32.0f, 0.0f);
    glm::mat4 translated = glm::translate(model, translate);
  


    map_render_component->set_projection_matrix(projection_matrix);
    map_render_component->set_modelview_matrix(translated);

    map_render_component->bind_shader();
  

    //TODO: I don't want to actually expose the shader, put these into wrappers in the shader object
    glUniformMatrix4fv(glGetUniformLocation(map_shader->get_program(), "mat_projection"), 1, GL_FALSE,glm::value_ptr(map_render_component->get_projection_matrix()));
    glUniformMatrix4fv(glGetUniformLocation(map_shader->get_program(), "mat_modelview"), 1, GL_FALSE, glm::value_ptr(map_render_component->get_modelview_matrix()));

    //Bind the vertex buffers and textures
    map_render_component->bind_vbos();
    map_render_component->bind_textures();


    glDrawArrays(GL_TRIANGLES, 0, map_render_component->get_num_vertices_render());
 
    //Release the vertex buffers and textures
    map_render_component->release_textures();
    map_render_component->release_vbos();

    map_render_component->release_shader();





    //Draw the characters
    const std::vector<int>& characters = map->get_characters();
    ObjectManager& object_manager = ObjectManager::get_instance();
    for(auto it = characters.begin(); it != characters.end(); ++it) {
        if(*it != 0) {
            std::shared_ptr<Object> sprite = object_manager.get_object(*it);
    
            RenderableComponent* character_render_component = sprite->get_renderable_component();
    
            //Move sprite to the required position
            glm::mat4 model1 = glm::mat4(1.0f);
            glm::vec3 translate1 = glm::vec3(
                (float(sprite->get_x_position()) - map->get_display_x()) * 32.0f,
                (float(sprite->get_y_position()) - map->get_display_y()) * 32.0f,
                0.0f
            );
            glm::mat4 translated1 = glm::translate(model1, translate1);
            character_render_component->set_modelview_matrix(translated1);
            character_render_component->set_projection_matrix(projection_matrix);

            character_render_component->bind_shader();

            Shader* shader = character_render_component->get_shader();
            if(shader == nullptr) {
                std::cerr << "ERROR: Shader is NULL in MapViewer::render_map" << std::endl;
                return;
            }

            //TODO: I don't want to actually expose the shader, put these into wrappers in the shader object
            glUniformMatrix4fv(glGetUniformLocation(shader->get_program(), "mat_projection"), 1, GL_FALSE,glm::value_ptr(character_render_component->get_projection_matrix()));

            glUniformMatrix4fv(glGetUniformLocation(shader->get_program(), "mat_modelview"), 1, GL_FALSE, glm::value_ptr(character_render_component->get_modelview_matrix()));

            character_render_component->bind_vbos();
            character_render_component->bind_textures();
            //            std::cout << " X " << sprite->get_x_position()*32.0f << " Y " << sprite->get_y_position()*32.0f<< std::endl;
            glDrawArrays(GL_TRIANGLES, 0, character_render_component->get_num_vertices_render());

            character_render_component->release_textures();
            character_render_component->release_vbos();
            character_render_component->release_shader();
        }
    }

    window->swap_buffers();
}

void MapViewer::refocus_map() {
    //Get the object
    ObjectManager& object_manager = ObjectManager::get_instance();

    if(map_focus_object == 0) {
        std::cout << "NULL" << std::endl;
        return;
    }
        
    std::shared_ptr<Object> object = object_manager.get_object(map_focus_object);

    //If such an object exists, move the map to it
    if(object) {
        float object_x = (float)object->get_x_position();
        float object_y = (float)object->get_y_position();
        //center the map on the object
        float map_width = (float)map->get_width();
        float map_height = (float)map->get_height();
        float map_display_width = (float)map->get_display_width();
        float map_display_height = (float)map->get_display_height();
        //Move the map to focus on the player
        //We wrap to stop the map moving off the view

        //TODO
        //need to handle odd and even width/ height

        float tile_offset = 32.0f / 2.0f;
        //if in scrolling part of map
        if(object_x - map_display_width/2.0f > 0) {
            //If in scrolling part
            if(object_x + map_display_width /2.0f < map->get_width()){ 
                map->set_display_x(object_x - map_display_width/ 2.0f);
           } 
            else {
                map->set_display_x(map_width - map_display_width + tile_offset);
            }
        }
        else {
            //not scrolling part
            map->set_display_x(0.0f);
        }

        if(object_y - map_display_height/2.0f > 0) {
            if(object_y + map_display_height /2.0f < map->get_height()){ 
                map->set_display_y(object_y - map_display_height/ 2.0f);
            } 
            else {
                map->set_display_y(map_height- map_display_height + tile_offset);
            }
        }
        else {
            map->set_display_y(0.0f);
        }
    }
}

void MapViewer::update_map(float dt ) {
    //move the map to the right position

    map->update_map(dt);
}

void MapViewer::set_map(Map* new_map) {
    map = new_map;
}

void MapViewer::set_map_focus_object(int object_id) {
    //Set the focus to the object if this is a valid object and it is on the map
    if(ObjectManager::is_valid_object_id(object_id)) {
        const std::vector<int>& characters = map->get_characters();
        map_focus_object = object_id;
        
        //If the object is on the map
        /*        if(std::find(characters.begin(), characters.end(),object_id) != characters.end()) {
            //focus on it
            map_focus_object = object_id;
            }*/
    }
    else {
        std::cout << "INVALID FOCUS OBJECT" << std::endl;
    }
}


Map* MapViewer::get_map() {
    return map;
}
