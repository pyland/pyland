#ifndef TEXT_BOX_H
#define TEXT_BOX_H

#include <memory>
#include <string>

#include "component_group.hpp"
#include "button.hpp"
#include "lifeline.hpp"
#include "text_stack.hpp"

class Text;

enum class Direction {NEXT, PREVIOUS};
enum class TextBoxType {Bar, Display};

class TextBox: public ComponentGroup
{
private:
    TextStack text_stack;
    std::shared_ptr<GUIText> text;

    std::shared_ptr<Button> forward_button;
    std::shared_ptr<Button> backward_button;

    Lifeline text_box;

    void move_text(Direction direction);
    TextBoxType type;
    unsigned int buffer_size;

public:

    TextBox(TextBoxType _type);
    ~TextBox();

    ///
    /// Clears the texts
    ///
    void clear_text();

    ///
    /// Add a message to the text box
    /// @param text_to_display the notification
    ///
    void add_message(std::string text_to_display);

    ///
    /// Add text of arbitrary length
    /// the functioin breaks it down into many messages
    void add_text(std::string text_to_display);

    void set_text(std::string _text);
    std::shared_ptr<Text> get_text();
    void set_text(std::shared_ptr<Text> _text);
    void resize_text(float width, float height);
    void move_text(float x_offset, float y_offset);
    void resize_buttons(float width, float height);
    void move_buttons(float x_offset, float y_offset, float spacing = 0.0f);
    void traverse_text(Direction direction);

    void set_buffer_size(unsigned int _size){buffer_size = _size;}

    void proceed();

    void open();
    void close();

    void hide_buttons();
    ///
    ///Methods to be defined when inheriting properties from ComponentGroup
    ///
    int generate_vertex_coords_element(GLfloat* data, int offset, std::tuple<float,float,float,float> bounds);
    int generate_texture_coords_element(GLfloat* data, int offset, std::tuple<float,float,float,float> bounds);

    int generate_tile_element_vertex_coords(GLfloat* data, int offset, std::tuple<float,float,float,float> bounds, float element_width, float element_height);
    int generate_tile_element_texture_coords(GLfloat* data, int offset, std::tuple<float,float,float,float>vertex_bounds, float element_width, float element_height, std::tuple<float,float,float,float> texture_bounds);
    int calculate_num_tile_elements(std::tuple<float,float,float,float> bounds, float element_width, float element_height);

    std::vector<std::pair<GLfloat*, int>> generate_this_vertex_data() override;

    std::vector<std::pair<GLfloat*, int>> generate_this_texture_data() override;

    std::vector<std::shared_ptr<GUIText>> generate_this_text_data() override;

};

#endif //TEXT_BOX_H
