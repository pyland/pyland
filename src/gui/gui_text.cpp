#include "gui_text.hpp"

#include <memory>

GUIText::GUIText() {

}

GUIText::~GUIText() {

}

void GUIText::display() {

}

std::vector<std::pair<GLfloat*, int>> GUIText::generate_vertex_data() {
    std::vector<std::pair<GLfloat*, int>> empty_data;
    return empty_data;
}

std::vector<std::pair<GLfloat*, int>> GUIText::generate_texture_data() {
    std::vector<std::pair<GLfloat*, int>> empty_data;
    return empty_data;    
}

std::vector<std::shared_ptr<GUIText>> GUIText::generate_text_data() {
    std::vector<std::shared_ptr<GUIText>> text_data;    
    text_data.push_back(std::make_shared(this));
    return text_data;
}
