#ifndef GUI_BUTTON_H
#define GUI_BUTTON_H

#include <string>
#include <functional>
#include "component_group.hpp"

///A class for Buttons on the GUI

class GUIButton: public ComponentGroup{

private:

    ///Text to be displayed under the button
    std::shared_ptr<GUIText> text;

    ///File path to the image file
    std::string image_source;

    ///Attributes for the height and width of the button
    float height;
    float width;

    ///Where the buttton will be displayed on the screen
    float x_offset;
    float y_offset;

public:
    GUIButton::GUIButton(std::string _image_source, std::string _text, std::function<void (void)> _on_click);

    float get_height(){
        return height;
    }
    float get_width(){
        return width;
    }

    void set_height(float _height){
        height = _height;
    }

    void set_width(float _width){
        width = _width;
    }

}

#endif // GUI_BUTTON_H
