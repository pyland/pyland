#include "gui_button.hpp"
#include "component_group.hpp"

GUIButton::GUIButton(std::string _image_source, std::string _text, std::function<void (void)> _on_click) :
ComponentGroup(on_click, 0, 0, 0, 0)
{
    image_source = _image_source;
    text = _text;
}
