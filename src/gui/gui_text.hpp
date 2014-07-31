#ifndef GUI_TEXT_H
#define GUI_TEXT_H

#include "typeface.hpp"
#include "text_font.hpp"
#include "text.hpp"

#include <memory>

///
/// Structure to hold the GUI text data that the GUI manager needs to render a component's text
///
class GUIText {
    ///
    /// The x position of the font in pixels. Initially in object space
    /// but is transformed into global space through the transforms
    ///
    int x_pos;

    ///
    /// The y position of the font in pixels. Initially in object space
    /// but is transformed into global space through the transforms
    ///
    int y_pos;

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
    ///
    /// Set the x position of the text
    /// @param x the xposition
    ///
    void set_x_pos(int x) { x_pos = x; }

    ///
    /// Get the x position of the text
    /// @return the x position
    ///
    int get_x_pos() { return x_pos; }

    ///
    /// Set the y position of the text
    /// @param y the xposition
    ///
    void set_y_pos(int y) { y_pos = y; }

    ///
    /// Get the y position of the text
    /// @return the y position
    ///
    int get_y_pos() { return y_pos; }
    
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
};

#endif
  
