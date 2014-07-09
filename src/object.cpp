#include "object.hpp"

#include <string>

Object::Object() {

}

Object::~Object() {

}


void Object::set_x_position(int x_pos) {
  x_position = x_pos;
}

void Object::set_y_position(int y_pos) {
  y_position = y_pos;
}

void Object::set_id(int new_id) {
  id = new_id;
}

void Object::set_name(std::string new_name) {
  name = new_name;
}

void Object::set_script(std::string new_script) {
  script = new_script;
}


