#ifndef SHADER_H
#define SHADER_H

#include <stdexcept>

#ifdef USE_GLES

#include <GLES2/gl2.h>

#endif

#ifdef USE_GL

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#endif
#include <string>

class Shader {
    ///
    /// Indicate if the shader loaded correctly
    ///
    bool loaded = false;

    ///
    /// The Opengl object identifier for this shader program
    ///
    GLuint program_obj = 0;

    /// 
    /// The Opengl object identifier for the fragment shader
    ///
    GLuint fragment_shader = 0;

    ///
    /// The Opengl object identifier for the vertex shader
    ///
    GLuint vertex_shader = 0;

    ///
    /// This function loads the shaders
    /// @param type The type of the shader: fragment or vertex
    /// @param src The source file for the shader's source
    /// @return The id of the shader
    /// 
    GLuint load_shader(GLenum type, const std::string src);

public:
    ///
    /// Represents a failure when loading the shader.
    ///
    class LoadException: public std::runtime_error {
    public:
        LoadException(const char  *message);
        LoadException(const std::string &message);
    };
    
    /// 
    /// This function creates the Opengl program
    /// @param vs The source file for the vertex source
    /// @param fs The source file for the fragment source
    /// @return The program id that Opengl is using for this shader program
    ///
    Shader(const std::string vs, const std::string fs);
    ~Shader();

    ///
    /// Find out if the shader loaded correctly
    /// @return Boolean indicating if the operation succeeded or not
    ///
    bool is_loaded() { return loaded; }

    ///
    /// Return the program object identifier
    /// @return the Opengl program object identifier
    ///
    GLuint get_program() { return program_obj; }

    ///
    /// Wrapper around glBindAttribLocation
    ///
    /// @param location The location index.
    /// @param variable The variable name to bind the location to.
    ///
    void bind_location_to_attribute(GLuint location, const char* variable);

    ///
    /// Wrapper around glLinkProgram
    ///
    void link();
};
#endif
