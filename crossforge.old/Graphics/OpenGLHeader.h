/*****************************************************************************\
*                                                                           *
* File(s): OpenGLHeader.h                                                   *
*                                                                           *
* Content: OpenGL header that distinct between platforms.                   *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_OPENGLHEADER_H__
#define __CFORGE_OPENGLHEADER_H__

#ifdef __EMSCRIPTEN__
#define GLFW_INCLUDE_ES3
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#else
#include <glad/glad.h>
#endif

#endif