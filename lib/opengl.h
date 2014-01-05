#ifndef ejoy2d_opengl_h
#define ejoy2d_opengl_h

#if defined( __APPLE__ ) && !defined(__MACOSX)

#define OPENGLES 2
#include <OpenGLES/ES2/gl.h>

#elif defined(_MSC_VER) && !defined(USE_GLEW)

#define OPENGLES 2
#include <GLES2/gl2.h>

#else

#define OPENGLES 0
#include <GL/glew.h>

#endif

#endif
