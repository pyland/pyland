// Needed to prevent warnings despite not being used AFAIK
#define GLM_FORCE_RADIANS

#include <algorithm>
#include <cmath>
#include <glog/logging.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <memory>
#include <ostream>
#include <utility>
#include <vector>

#include "engine.hpp"
#include "game_window.hpp"
#include "gui_manager.hpp"
#include "layer.hpp"
#include "map.hpp"
#include "map_object.hpp"
#include "map_viewer.hpp"
#include "object_manager.hpp"
#include "renderable_component.hpp"
#include "shader.hpp"

#include "open_gl.hpp"


MapViewer::MapViewer(GameWindow *window, GUIManager *gui_manager):
    gui_manager(CHECK_NOTNULL(gui_manager)),
    window(CHECK_NOTNULL(window)) {

        resize();

        // Set background color and clear buffers
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);
        // Disable writing to the alpha channel.
        // Fixes a bug where EGL layer becomes transparent.
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
        // Leave this here!!!
        //Disable back face culling.
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_SCISSOR_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


MapViewer::~MapViewer() {
    LOG(INFO) << "MapViewer DESTROYED";
}

void MapViewer::resize() {
    LOG(INFO) << "Map resizing";
    std::pair<int, int> size(window->get_size());

    // Set the viewable fragments
    glScissor(0, 0, 848, 480);
    glViewport(0, 0, size.first, size.second);

    //glScissor(0, 0, 480, 1000);
    //glViewport(0, 0, 848, 480);

    if (get_map()) {
        // Readjust the map focus
        refocus_map();
    }
}

void MapViewer::render() {
    CHECK_NOTNULL(map);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    render_map();
    render_objects();
    render_gui();
}

void MapViewer::render_map() {
    // Focus onto the player
    refocus_map();
    // Calculate the projection and modelview matrix for the map
    std::pair<int, int> size(window->get_size());
    glm::mat4 projection_matrix(glm::ortho(0.0f, float(size.first), 0.0f, float(size.second), 0.0f, 1.0f));

    glm::mat4 model(glm::mat4(1.0f));
    model = glm::scale    (model, glm::vec3(Engine::get_actual_tile_size()));
    model = glm::translate(model, glm::vec3(-get_display_x(), -get_display_y(), 0.0f));

    // Draw all the layers, from base to top to get the correct draw order
    int layer_num = 0;
    for (int layer_id: map->get_layers()) {
        auto layer(ObjectManager::get_instance().get_object<Layer>(layer_id));
        if (!layer) {
            continue;
        }

        if (!layer->is_renderable()) {
            continue;
        }

        std::shared_ptr<RenderableComponent> layer_render_component(layer->get_renderable_component());
        Shader *layer_shader(layer_render_component->get_shader().get());

        //Set the matrices
        layer_render_component->set_projection_matrix(projection_matrix);
        layer_render_component->set_modelview_matrix(model);

        layer_render_component->bind_shader();

        //TODO: I don't want to actually expose the shader, put these into wrappers in the shader object
        glUniformMatrix4fv(glGetUniformLocation(layer_shader->get_program(), "mat_projection"),
                           1,
                           GL_FALSE,
                           glm::value_ptr(layer_render_component->get_projection_matrix()));

        glUniformMatrix4fv(glGetUniformLocation(layer_shader->get_program(), "mat_modelview"),
                           1,
                           GL_FALSE,
                           glm::value_ptr(layer_render_component->get_modelview_matrix()));


        layer_render_component->bind_vbos();

        layer_render_component->bind_textures();

        glDrawArrays(GL_TRIANGLES, 0, layer_render_component->get_num_vertices_render());

        //Release the vertex buffers and texppptures
        layer_render_component->release_textures();
        layer_render_component->release_vbos();

        layer_render_component->release_shader();

        //next layer
        layer_num ++;
    }
}

void MapViewer::render_objects() {
    //Calculate the projection matrix
    std::pair<int, int> size = window->get_size();
    glm::mat4 projection_matrix = glm::ortho(0.0f, float(size.first), 0.0f, float(size.second), 0.0f, 1.0f);
    //Draw the objects
    const std::vector<int>& objects = map->get_objects();
    ObjectManager& object_manager = ObjectManager::get_instance();
    for (auto it = objects.begin(); it != objects.end(); ++it) {
        if (*it != 0) {
            std::shared_ptr<MapObject> object = object_manager.get_object<MapObject>(*it);

            if (!object) {
                continue;
            }

            if (!object->is_renderable()) {
                continue;
            }

            std::shared_ptr<RenderableComponent> object_render_component = object->get_renderable_component();

            //Move object to the required position
            glm::vec3 translator(
                object->get_position().x - get_display_x(),
                object->get_position().y - get_display_y(),
                0.0f
            );

            glm::mat4 model(glm::mat4(1.0f));
            model = glm::scale    (model, glm::vec3(Engine::get_actual_tile_size()));
            model = glm::translate(model, translator);

            object_render_component->set_modelview_matrix(model);
            object_render_component->set_projection_matrix(projection_matrix);

            object_render_component->bind_shader();

            Shader* shader = object_render_component->get_shader().get();

            if(shader == nullptr) {
                LOG(ERROR) << "MapViewer::render_map: Shader (object_render_component->get_shader()) should not be null";
                return;
            }

            //TODO: I don't want to actually expose the shader, put these into wrappers in the shader object
            glUniformMatrix4fv(glGetUniformLocation(shader->get_program(), "mat_projection"), 1, GL_FALSE,glm::value_ptr(object_render_component->get_projection_matrix()));

            glUniformMatrix4fv(glGetUniformLocation(shader->get_program(), "mat_modelview"), 1, GL_FALSE, glm::value_ptr(object_render_component->get_modelview_matrix()));

            object_render_component->bind_vbos();
            object_render_component->bind_textures();

            glDrawArrays(GL_TRIANGLES, 0, object_render_component->get_num_vertices_render());

            object_render_component->release_textures();
            object_render_component->release_vbos();
            object_render_component->release_shader();
        }
    }
}

void MapViewer::render_gui() {
    //Calculate the projection matrix
    std::pair<int, int> size = window->get_size();
    glm::mat4 projection_matrix = glm::ortho(0.0f, float(size.first), 0.0f, float(size.second), 0.0f, 1.0f);

    //TODO: Hacky method, clean it up
    std::shared_ptr<RenderableComponent> gui_render_component = gui_manager->get_renderable_component();

    //Move gui_manager to the required position
    glm::mat4 model2 = glm::mat4(1.0f);
    gui_render_component->set_modelview_matrix(model2);
    gui_render_component->set_projection_matrix(projection_matrix);

    gui_render_component->bind_shader();

    Shader* gui_shader = gui_render_component->get_shader().get();
    if(gui_shader == nullptr) {
        LOG(ERROR) << "ERROR: Shader is NULL in MapViewer::render_map";
        return;
    }

    //TODO: I don't want to actually expose the shader, put these into wrappers in the shader object
    glUniformMatrix4fv(glGetUniformLocation(gui_shader->get_program(), "mat_projection"), 1, GL_FALSE,glm::value_ptr(gui_render_component->get_projection_matrix()));

    glUniformMatrix4fv(glGetUniformLocation(gui_shader->get_program(), "mat_modelview"), 1, GL_FALSE, glm::value_ptr(gui_render_component->get_modelview_matrix()));

    gui_render_component->bind_vbos();
    gui_render_component->bind_textures();

    glDrawArrays(GL_TRIANGLES, 0, gui_render_component->get_num_vertices_render());

    gui_render_component->release_textures();
    gui_render_component->release_vbos();
    gui_render_component->release_shader();

    gui_manager->render_text();
}

///
/// Take a line of a given size (length) and a point offset on that line (point):
///
/// ← length    →
/// ├───────•───┤
/// ← point →
///
/// Also takes a display of a given size (bound):
///
/// ← bound→
/// ┼─────────────┼
///
/// If bound == length:
///
///     It places the boxes over eachother:
///
///     ┼─────────────┼
///     ├─────────•───┤
///
/// If length > bound:
///
///     It centres the box on the point:
///
///            ┼─────────────┼
///     ├─────────────•───┤
///
///     Then moves the box inside the bounds, if needed:
///
///         ┼─────────────┼
///     ├─────────────•───┤
///
/// If bound > length:
///
///     It centres the line inside the box:
///
///     ┼─────────────┼
///           |•────────┤
///
///     It then moves the line inside the box, if needed:
///
///     ┼─────────────┼
///         |•────────┤
///
/// Then it returns the distance from the start of length to the start of bound:
///
///     For example,
///
///         ────→
///             ┼─────────────┼
///         ├─────────────•───┤
///
///     which is positive, or
///
///         ←────
///         ┼─────────────┼
///             |•────────┤
///
///     which is negative.
///

float centre_point_in_range(float point, float length, float bound) {
    // First case is a union of the other two
    // and both cases have same first step.
    float bound_offset = point - bound / 2.0f;

    // Crop to valid range: bound inside length or length inside bound
    // Note order of min/max
    if (length >= bound) {
        // bound_offset positive by no more than | length - bound |
        bound_offset = std::min(std::max(bound_offset, 0.0f), length - bound);
    }
    else if (bound >= length) {
        // bound_offset negative by no more than | length - bound |
        bound_offset = std::max(std::min(bound_offset, 0.0f), length - bound);
    }

    return bound_offset;
}

void MapViewer::refocus_map() {
    //Get the object
    ObjectManager& object_manager = ObjectManager::get_instance();

    if (map_focus_object == 0) {
        LOG(INFO) << "MapViewer::refocus_map: No focus.";
        return;
    }

    std::shared_ptr<MapObject> object = object_manager.get_object<MapObject>(map_focus_object);

    // If such an object exists, move the map to it
    if (object) {
        set_display_x(centre_point_in_range(
            // half-tile offset to take centre of object
            /* point  */ float(object->get_position().x) + 0.5f,
            /* length */ float(map->get_width()),
            /* bound  */ get_display_width()
        ));

        set_display_y(centre_point_in_range(
            // half-tile offset to take centre of object
            /* point  */ float(object->get_position().y) + 0.5f,
            /* length */ float(map->get_height()),
            /* bound  */ get_display_height()
        ));
    } else {
        LOG(INFO) << "MapViewer::refocus_map: No objects have focus.";
    }
    //Engine::text_updater(); TODO: BLEH, remove pernmantley if needed or re-add if not
}

void MapViewer::set_map(Map* new_map) {
    //Reset the map and associated data
    map = new_map;
    map_focus_object = 0;
    map_display_x = 0.0f;
    map_display_y = 0.0f;

    //Resize the map display
    resize();
}

void MapViewer::set_map_focus_object(int object_id) {
    //Set the focus to the object if this is a valid object and it is on the map
    if(ObjectManager::is_valid_object_id(object_id)) {
        //        const std::vector<int>& objects = map->get_objects();

        //moving in-focus icon
        if(ObjectManager::is_valid_object_id(map_focus_object)) {
            ObjectManager::get_instance().get_object<MapObject>(map_focus_object)->set_focus(false);
        }
        ObjectManager::get_instance().get_object<MapObject>(object_id)->set_focus(true);


        map_focus_object = object_id;
        refocus_map();


        //TODO: add this in again
        //If the object is on the map
        /*        if(std::find(objects.begin(), objects.end(),object_id) != objects.end()) {
            //focus on it
            map_focus_object = object_id;
            }*/
    }
    else {
        LOG(ERROR) << "MapViewer::set_map_focus_object: Invalid focus object";
    }
}


Map* MapViewer::get_map() {
    return map;
}


glm::ivec2 MapViewer::pixel_to_tile(glm::ivec2 pixel_location) {
    float scale(Engine::get_actual_tile_size());

    glm::vec2 display_position(get_display_x(), get_display_y());
    glm::vec2 float_result = (glm::vec2(pixel_location) / scale) + display_position;
    return (glm::ivec2)float_result;
}

glm::ivec2 MapViewer::tile_to_pixel(glm::vec2 tile_location) {
    float scale(Engine::get_actual_tile_size());

    glm::vec2 display_position(get_display_x(), get_display_y());

    // Screen offset is reduced by the offset of the display
    return (tile_location - display_position) * scale;
}

float MapViewer::get_display_width() {
    return float(window->get_size().first) / Engine::get_actual_tile_size();
}

float MapViewer::get_display_height() {
    return float(window->get_size().second) / Engine::get_actual_tile_size();
}

float MapViewer::get_display_x() {
    // Must be to nearest pixel for render accuracy
    return map_display_x;
}

float MapViewer::get_display_y() {
    // Must be to nearest pixel for render accuracy
    return map_display_y;
}
