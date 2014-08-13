#include <glog/logging.h>
#include <iostream>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

#include "shader.hpp"
#include "graphics_context.hpp"



// Need to inherit constructors manually.
// NOTE: This will, and are required to, copy the message.
Shader::LoadException::LoadException(const char *message): std::runtime_error(message) {}
Shader::LoadException::LoadException(const std::string &message): std::runtime_error(message) {}



static std::string load_file(std::string filename) {
    std::ifstream file(filename);
    std::stringstream output;

    output << file.rdbuf();
    return output.str();
}



std::shared_ptr<Shader> Shader::new_resource(const std::string resource_name) {
    return std::make_shared<Shader>(resource_name);
}

Shader::Shader(const std::string program_name):
    Shader(
#ifdef USE_GLES
           program_name + ".glesv",
           program_name + ".glesf"
#endif
#ifdef USE_GL
           program_name + ".glv",
           program_name + ".glf"
#endif
           ) {
}

Shader::Shader(const std::string vs, const std::string fs): CacheableResource() {
    GLint linked;

    //Load the fragment and vertex shaders
    vertex_shader   = load_shader(GL_VERTEX_SHADER,   load_file(vs).c_str());
    fragment_shader = load_shader(GL_FRAGMENT_SHADER, load_file(fs).c_str());

    //Create the program object
    program_obj = glCreateProgram();

    if(program_obj == 0) {
        LOG(ERROR) << "Shader creation: Could not create program object.";
        LOG(ERROR) << "Flag: " << glGetError();
        throw Shader::LoadException("Unable to create shader program");
    }

    glAttachShader(program_obj, vertex_shader);
    glAttachShader(program_obj, fragment_shader);

    // Temporary hack before restructuring.
    // TODO: Untemparary-ify this.
    glBindAttribLocation(program_obj, /* VERTEX_POS_INDX       */ 0, "a_position");
    glBindAttribLocation(program_obj, /* VERTEX_TEXCOORD0_INDX */ 1, "a_texCoord");

    //Link the program
    glLinkProgram(program_obj);

    //Check to see if we have any log info
    glGetProgramiv(program_obj, GL_LINK_STATUS, &linked);

    if (!linked) {
        GLint info_len = 0;

        glGetProgramiv(program_obj, GL_INFO_LOG_LENGTH, &info_len);

        if(info_len > 1) {
            char* info_log = new char[sizeof(char)*info_len];

            glGetProgramInfoLog(program_obj, info_len, nullptr, info_log);
            LOG(ERROR) << "Program linking:\n" << info_log;
            delete []info_log;
        }
        glDeleteShader(fragment_shader);
        glDeleteShader(vertex_shader);
        glDeleteProgram(program_obj);
        throw Shader::LoadException("Unable to link shader program");
    }

    loaded = true;
}


Shader::~Shader() {
    glDeleteShader(fragment_shader);
    glDeleteShader(vertex_shader);
    glDeleteProgram(program_obj);
}


GLuint Shader::load_shader(GLenum type, const std::string src) {
    GLuint shader;
    GLint compiled = 0;

    // Create the shader object
    shader = glCreateShader(type);

    if(shader == 0) {
        // Couldn't create the shader
        return 0;
    }

    // Load shader source code
    const char* source = src.c_str();
    glShaderSource(shader, 1, &source, nullptr);

    // Compile the shader
    glCompileShader(shader);

    // Check for errors
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    // Handle the errors
    if(!compiled) {
        GLint info_len = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);

        if(info_len > 1) {
            char* info_log = new char[sizeof(char) * info_len];

            glGetShaderInfoLog(shader, info_len, nullptr, info_log);
            LOG(ERROR) << "Shader loading failed:\n"<< info_log;
            delete []info_log;
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;

}


void Shader::bind_location_to_attribute(GLuint location, const char* variable) {
    glBindAttribLocation(program_obj, location, variable);
}


void Shader::link() {
    glLinkProgram(program_obj);
}
