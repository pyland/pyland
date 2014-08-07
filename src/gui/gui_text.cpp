#include "gui_text.hpp"

#include <memory>

GUIText::GUIText() {
    text_data = make_shared<GUITextData>();
}

GUIText::~GUIText() {

}

std::vector<std::pair<GLfloat*, int>> GUIText::generate_vertex_data() {
    std::vector<std::pair<GLfloat*, int>> empty_data;
    return empty_data;
}

std::vector<std::pair<GLfloat*, int>> GUIText::generate_texture_data() {
    std::vector<std::pair<GLfloat*, int>> empty_data;
    return empty_data;    
}

std::vector<std::shared_ptr<GUITextData>> GUIText::generate_text_data() {
    std::vector<std::shared_ptr<GUITextData>> text_data_vec;    
    text_data_vec.push_back(text_data);
    return text_data_vec;
}
    
std::shared_ptr<Typeface> get_typeface() {
    return text_data->get_typeface();
} 
  
void set_typeface(std::shared_ptr<Typeface> _typeface) {
    text_data->set_typeface(_typeface);
}

std::shared_ptr<TextFont> get_text_font() {
    return text_data->get_text_font();
}

void set_text_font(std::shared_ptr<TextFont> _textfont) {
    text_data->set_text_font(_textfont);
}

std::shared_ptr<Text> get_text() {
    return text_data->get_text();
}

void set_text(std::shared_ptr<Text> _text) {
    text_data->set_text(_text);
}

