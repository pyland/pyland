#ifndef BUTTON_H
#define BUTTON_H

#include "component_group.hpp"
#include "gui_text.hpp"
#include "gui_text_data.hpp"
#include "text.hpp"
#include <tuple>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "open_gl.hpp"

enum class ButtonType{
    Board,
    Single,
    NoPicture
};

enum class ButtonAlignment{
    TopLeft,
    TopRight,
    BottomLeft
};

class Button : public ComponentGroup {

    std::shared_ptr<GUIText> button_text;
    ButtonType type;
    std::string file_path;
    std::string picture_name;
    ButtonAlignment alignment;

public:
    Button(ButtonType _type);
    Button(ButtonType _type, std::shared_ptr<Text> _text, std::function<void (void)> on_click, float _width, float _height, float _x_offset, float _y_offset);

    void set_picture(std::string _name);
    std::shared_ptr<Text> get_text();

    void set_alignment(ButtonAlignment _alignment);
    ButtonAlignment get_alignment();

    void set_text(std::shared_ptr<Text> );
    void set_text(std::string);
    void set_text_colour(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    void resize_text(float width, float height);
    void move_text(float x_offset, float y_offset);

    int generate_vertex_coords_element(GLfloat* data, int offset, std::tuple<float,float,float,float> bounds);
    int generate_texture_coords_element(GLfloat* data, int offset, std::tuple<float,float,float,float> bounds);

    int generate_tile_element_vertex_coords(GLfloat* data, int offset, std::tuple<float,float,float,float> bounds, float element_width, float element_height);
    int generate_tile_element_texture_coords(GLfloat* data, int offset, std::tuple<float,float,float,float>vertex_bounds, float element_width, float element_height, std::tuple<float,float,float,float> texture_bounds);
    int calculate_num_tile_elements(std::tuple<float,float,float,float> bounds, float element_width, float element_height);

    std::vector<std::pair<GLfloat*, int>> generate_this_vertex_data() override;

    std::vector<std::pair<GLfloat*, int>> generate_this_texture_data() override;

    std::vector<std::shared_ptr<GUIText>> generate_this_text_data() override;

};

#endif
