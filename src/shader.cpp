#include "shader.hpp"

#include <glog/logging.h>
#include <iostream>
#include <fstream>
#include <string>



static char* load_file(std::string filename) {
    char* content;
    std::ifstream file;
    
    file.open(filename);
    
    // If you even need [more than] 2GiB, you are doing something
    // seriously wrong.
    file.seekg(0, std::ios_base::end);
    int file_size = (int)file.tellg();
    file.seekg(0, std::ios_base::beg);
    
    content = new char[file_size+1];
    file.read(content, file_size);
    if (file.gcount() != file_size) {
        LOG(ERROR) << "Unable to load shader file \"" << filename << "\".";
        throw Shader::LoadException("Unable to load shader file");
    }
    
    file.close();

    // Add null terminator.
    content[file_size] = '\0';
    
    return content;
}



Shader::LoadException::LoadException(const char* message) {
    this->message = message;
}


const char* Shader::LoadException::what() const noexcept {
    return message;
}



Shader::Shader(const std::string vs, const std::string fs) {
    GLint linked;

    char* vs_src = load_file(vs);
    char* fs_src = load_file(fs);
    
    //Load the fragment and vertex shaders
    vertex_shader = load_shader(GL_VERTEX_SHADER, vs_src);
    fragment_shader = load_shader(GL_FRAGMENT_SHADER, fs_src);

    delete vs_src;
    delete fs_src;

    //Create the program object
    program_obj = glCreateProgram();
    
    if(program_obj == 0) {
        LOG(ERROR) << "Shader creation: Could not create program object.";
        LOG(ERROR) << "Flag: " << glGetError();
        throw Shader::LoadException("Unable to create shader program");
    }

    glAttachShader(program_obj, vertex_shader);
    glAttachShader(program_obj, fragment_shader);
    
    //Link the program
    glLinkProgram(program_obj);
    
    //Check to see if we have any log info
    glGetProgramiv(program_obj, GL_LINK_STATUS, &linked);
    
    if(!linked) {
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
