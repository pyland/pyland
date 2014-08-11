#ifndef SHADER_H
#define SHADER_H

#include <glm/vec2.hpp>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif

#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

#include "resource_cache.hpp"
#include "cacheable_resource.hpp"



class GraphicsContext;
class Shader;



class Shader : public CacheableResource<Shader> {
private:
    friend class ResourceCache<Shader>;

    ///
    /// Program base name (if constructed with program name).
    ///
    std::string program_name;

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
        LoadException(const char *message);
        LoadException(const std::string &message);
    };

    ///
    /// Create a new shared shader from a resource name.
    ///
    /// @param program_name Base path of a shader program description
    ///        file, or platform's shader files.
    /// @return A shared pointer to the relevant Shader.
    ///

    static std::shared_ptr<Shader> new_shared(const std::string resource_name);

    ///
    /// This function creates the Opengl program from a program base
    /// name.
    /// @param program_name The base name/path of the shader files.
    /// @return A shader ready to be used.
    ///
    Shader(const std::string program_name);
    ///
    /// This function creates the Opengl program
    /// @param vs The source file for the vertex source
    /// @param fs The source file for the fragment source
    /// @return A shader ready to be used.
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
