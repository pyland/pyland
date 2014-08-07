#ifndef GUI_TEXT_H
#define GUI_TEXT_H

#include "component.hpp"
#include "typeface.hpp"
#include "text_font.hpp"
#include "text.hpp"

#include <memory>

///
/// Structure to hold the GUI text data that the GUI manager needs to render a component's text
///
class GUIText : public Component {
    ///
    /// The transformed x pixel offset in to global space
    ///
    int transformed_x_pixel_offset;

    ///
    /// The transformed y pixels offset in to global space
    ///
    int transformed_y_pixel_offset;

    ///
    /// The typeface to use for the font
    ///
    std::shared_ptr<Typeface> typeface;

    ///
    /// The font to use
    ///
    std::shared_ptr<TextFont> font;

    ///
    /// The text to render
    ///
    std::shared_ptr<Text> text;

public:
    GUIText();
    ~GUIText();

    ///
    /// Get the transformed x pixel  position of the text
    /// @return the x position
    ///
    int get_transformed_x_offset() { return transformed_x_pixel_offset; }

    ///
    /// Get the transformed pixel y position of the text
    /// @return the y position
    ///
    int get_transformed_y_offset() { return transformed_y_pixel_offset; }

    ///
    /// Set the transformed x pixel  position of the text
    /// @param the x position
    ///
    void set_transformed_x_offset(int pixels) { transformed_x_pixel_offset = pixels; }

    ///
    /// Set the transformed pixel y position of the text
    /// @param the y position
    ///
    void set_transformed_y_offset(int pixels) { transformed_y_pixel_offset = pixels; }
    
    ///
    /// Get the typeface for the text
    /// @return the typeface
    ///
    std::shared_ptr<Typeface> get_typeface() { return typeface; }
  
    ///
    /// Set the typeface for the font
    /// @param _typeface the typeface to use
    ///
    void set_typeface(std::shared_ptr<Typeface> _typeface) { typeface = _typeface; }

    ///
    /// Get the text's font
    /// @return the text font
    ///
    std::shared_ptr<TextFont> get_text_font() { return font; }

    ///
    /// Set the text's font
    /// @param _textfont the text's font
    ///
    void set_text_font(std::shared_ptr<TextFont> _textfont) { font = _textfont; }

    ///
    /// Get the text 
    /// @return the text
    ///
    std::shared_ptr<Text> get_text() { return text; }

    ///
    /// Set the text
    /// @_text the new text
    ///
    void set_text(std::shared_ptr<Text> _text) { text = _text; }

    ///
    /// Display the text at the pixel x and y positions
    ///
    void display();

    //Overloads from Component
    std::vector<std::pair<GLfloat*, int>> generate_vertex_data();
    std::vector<std::pair<GLfloat*, int>> generate_texture_data();
    std::vector<std::shared_ptr<GUIText>> generate_text_data();
};

#endif
  
