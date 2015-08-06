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

class TextBox: public ComponentGroup
{
private:
    TextStack text_stack;
    std::shared_ptr<GUIText> text;

    std::shared_ptr<Button> forward_button;
    std::shared_ptr<Button> backward_button;

    Lifeline text_box;

    void move_text(Direction direction);

    ///
    /// update buttons depending on the notification_stach flags
    ///
    void hide_buttons();

public:

    TextBox();
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

    void set_text(std::string _text);
    std::shared_ptr<Text> get_text();
    void set_text(std::shared_ptr<Text> _text);
    void resize_text(float width, float height);
    void move_text(float x_offset, float y_offset);
    void traverse_text(Direction direction);

    ///
    ///Methods to be defined when inheriting properties from ComponentGroup
    ///These don't do anything as such because no image is to be displayed in the textbox
    ///
    std::vector<std::pair<GLfloat*, int>> generate_this_vertex_data() override;
    std::vector<std::pair<GLfloat*, int>> generate_this_texture_data() override;
    std::vector<std::shared_ptr<GUIText>> generate_this_text_data() override;
};

#endif //TEXT_BOX_H
