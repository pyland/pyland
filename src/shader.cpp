#include "shader.hpp"

#include <iostream>
#include <string>

Shader::Shader(const std::string vs, const std::string fs) {
    GLint linked;
    
    //Load the fragment and vertex shaders
    vertex_shader = load_shader(GL_VERTEX_SHADER, vs);
    fragment_shader = load_shader(GL_FRAGMENT_SHADER, fs);
    
    //Create the program object
    program_obj = glCreateProgram();
    
    if(program_obj == 0) {
      std::cerr << "ERROR FLAG: " << glGetError();
      std::cerr << "ERROR: SHADER PROGRAM CREATION. Could not create program object." << std::endl;
      return;
    }

    glAttachShader(program_obj, vertex_shader);
    glAttachShader(program_obj, fragment_shader);
    
    glBindAttribLocation(program_obj,0 /* VERTEX_POS_INDX */, "a_position");
    glBindAttribLocation(program_obj, 1/*VERTEX_TEXCOORD0_INDX*/, "a_texCoord");
    
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
            std::cerr << "ERROR: PROGRAM LINKING " << std::endl  << info_log << std::endl;
            delete []info_log;
        }
        glDeleteProgram(program_obj);
        return;
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
            std::cerr << "ERROR: SHADER LOADING " << std::endl  << info_log << std::endl;
            delete []info_log;
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;

} 

