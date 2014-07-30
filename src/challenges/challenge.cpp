#include "python_embed_headers.hpp"

#include "challenge.hpp"
#include "input_manager.hpp"

#include <string>
#include <iostream>

Challenge::Challenge(InputManager *input_manager): input_manager(input_manager) {}
Challenge::~Challenge() {}
