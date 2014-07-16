#include "map_viewer.hpp"
#include "renderable_component.hpp"
#include "map.hpp"
#include "character.hpp"

#include <map>
#include <iostream>
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
    glViewport(0, 0,  size.first, size.second);
    glm::mat4 projection_matrix = glm::ortho(0.0f, (float)(size.first), 0.0f, (float)(size.second), 0.0f, 1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 translate = glm::vec3(map->get_display_x(), map->get_display_y(), 0.0f);
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
    std::map<int, Character*> characters = *(map->get_characters_map());
    for(auto it = characters.begin(); it != characters.end(); ++it) {
        Character* sprite = it->second;
    
        RenderableComponent* character_render_component = sprite->get_renderable_component();
    
        //Move sprite to the required position
        glm::mat4 model1 = glm::mat4(1.0f);
        glm::vec3 translate1 = glm::vec3(map->get_display_x() + sprite->get_x_position(), sprite->get_y_position() + map->get_display_y(), 0.0f);
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

        glDrawArrays(GL_TRIANGLES, 0, character_render_component->get_num_vertices_render());

        character_render_component->release_textures();
        character_render_component->release_vbos();
        character_render_component->release_shader();

    }

    window->swap_buffers();
}


void MapViewer::update_map(float dt ) {
    /*    float map_scroll_speed = 16.0f;
    // animate map
    float map_display_right_x = map->get_display_x() + map->get_display_width();
    float map_display_top_y = map->get_display_y() + map->get_display_height();

    // scroll the map towards the top right
    map->set_display_x(map->get_display_x()+map_scroll_speed*dt);
    map->set_display_y(map->get_display_y()+map_scroll_speed*dt);

    // perform wrapping
    if(map_display_right_x > map->get_width()*16.0f) {
        map->set_display_x(0.0f);//wrap round
    }

    if(map_display_top_y > map->get_height()*16.0f) {
        map->set_display_y(0.0f); //wrap round
    }
    */

    map->update_map(dt);
}

void MapViewer::set_map(Map* new_map) {
    map = new_map;
}


Map* MapViewer::get_map() {
    return map;
}
