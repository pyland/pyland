#ifndef SHADER_H
#define SHADER_H

#include <glm/vec2.hpp>
#include <map>
#include <memory>
#include <stdexcept>

#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif

#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

#include <string>



class GraphicsContext;



class Shader {
private:
    ///
    /// Contains shaders asscociated with a common GL context.
    ///
    class ShaderCache {
    private:
        ///
        /// Map from program paths to shaders.
        ///
        std::map<std::string, std::weak_ptr<Shader>> shaders;
    public:
        ShaderCache();
        ~ShaderCache();
        ///
        /// Get a shader from a program path.
        ///
        /// If already loaded, retrieves a shader from the cache.
        /// Otherwise, the shader is loaded.
        ///
        /// @param program_path Filename of a shader program description
        ///        file.
        /// @return A shared pointer to the relevant Shader.
        ///
        std::shared_ptr<Shader> get_shader(const std::string program_name);

        ///
        /// Removes a shader from the cache. Does not destroy it.
        ///
        void remove_shader(const std::string program_name);
    };

    friend class ShaderCache;

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

    ///
    /// The shader cache associated with the shader (if any)
    ///
    ShaderCache* cache;

    ///
    /// Map of graphics contexts to shader caches.
    ///
    static std::map<GraphicsContext*, std::shared_ptr<ShaderCache>> shader_caches;

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
        LoadException(const char *message);
        LoadException(const std::string &message);
    };

    // TODO Joshua: Comment
    // TODO Joshua: Consider integer-ness.
    void set_state_on_moving_start(glm::ivec2 target);
    void set_state_on_moving_finish();

    ///
    /// Get a commonly used Shader configuration.
    ///
    /// This function is used to share shaders between separate
    /// graphical components. On first run with given parameters it will
    /// load the shader, on other calls it will retrieve it from a
    /// cache. There is a separate cache for each GL context.
    ///
    /// @param program_name Base path of a shader program description
    ///        file, or platform's shader files.
    /// @return A shared pointer to the relevant Shader.
    ///
    static std::shared_ptr<Shader> get_shared_shader(const std::string program_name);

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
