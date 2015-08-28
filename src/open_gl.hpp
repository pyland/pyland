//A simple file to include the correct version of the OpenGL library depending on the platform
//Mobile (Raspberry Pi)
#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif

//Desktop,(Apple vs Linux)
#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#endif
