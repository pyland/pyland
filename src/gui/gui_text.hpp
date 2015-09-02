#ifndef GUI_TEXT_H
#define GUI_TEXT_H

#include "gui_text_data.hpp"
#include "component.hpp"
#include <memory>
#include <string>
#include <vector>

class Text;
class TextFont;
class Typeface;

///
/// A text component
///
class GUIText : public Component {
    
    ///
    /// The text data
    ///
    std::shared_ptr<GUITextData> text_data;
 
public:
    GUIText();
    ~GUIText();
    
    ///
    /// Get the text 
    /// @return the text
    ///
    std::shared_ptr<Text> get_text();

    ///
    /// Set the text
    /// @_text the new text
    ///
    void set_text(std::shared_ptr<Text> _text);

    ///
    /// Get the text data component
    /// @return the gui text
    ///
    std::shared_ptr<GUITextData> get_gui_text();

    std::string get_text_as_string();
    //Overloads from Component
    std::vector<std::pair<GLfloat*, int>> generate_vertex_data();
    std::vector<std::pair<GLfloat*, int>> generate_texture_data();
    std::vector<std::shared_ptr<GUIText>> generate_text_data();
};

#endif
  
