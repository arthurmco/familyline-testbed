#pragma once

/**
 * Contains all opengl headers Familyline will need.
 * Also an include. If those headers were not available, OpenGL support will not be there.
 * (Renderers will be optional, but you will need to choose one.)
 *
 */

#if __has_include(<SDL2/SDL_opengl.h>) && __has_include(<GL/glew.h>)
#define RENDERER_OPENGL (FLINE_RENDERER == "opengl")

#include <client/config.h>

#ifdef FLINE_USE_GLES
#define USE_GLES FLINE_USE_GLES
#endif

#if !(defined(__gl_h_) || defined(__GL_H__) || defined(_GL_H) || defined(__X_GL_H))
#ifndef USE_GLES
/// GLEW does not support OpenGL ES, and the functions signatures will
/// conflict if we include both the GL ES headers and GLEW header
#include <GL/glew.h>
#endif
#endif

#include <SDL2/SDL.h>

#ifdef USE_GLES
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>

// This requires support for the following extensions, in GLES mode:
//  - https://www.khronos.org/registry/OpenGL/extensions/EXT/EXT_sRGB.txt
//  - https://www.khronos.org/registry/OpenGL/extensions/EXT/EXT_texture_format_BGRA8888.txt

//#define GL_BGR GL_BGR_EXT
#define GL_BGRA GL_BGRA_EXT
//#define GL_SRGB GL_SRGB_EXT
#define GL_SRGB_ALPHA GL_SRGB_ALPHA_EXT

#define GL_DEBUG_SOURCE_API GL_DEBUG_SOURCE_API_KHR
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM GL_DEBUG_SOURCE_WINDOW_SYSTEM_KHR
#define GL_DEBUG_SOURCE_SHADER_COMPILER GL_DEBUG_SOURCE_SHADER_COMPILER_KHR
#define GL_DEBUG_SOURCE_THIRD_PARTY GL_DEBUG_SOURCE_THIRD_PARTY_KHR
#define GL_DEBUG_SOURCE_APPLICATION GL_DEBUG_SOURCE_APPLICATION_KHR
#define GL_DEBUG_SOURCE_OTHER GL_DEBUG_SOURCE_OTHER_KHR

#define GL_DEBUG_TYPE_ERROR GL_DEBUG_TYPE_ERROR_KHR
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_KHR
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_KHR
#define GL_DEBUG_TYPE_PORTABILITY GL_DEBUG_TYPE_PORTABILITY_KHR
#define GL_DEBUG_TYPE_PERFORMANCE GL_DEBUG_TYPE_PERFORMANCE_KHR
#define GL_DEBUG_TYPE_OTHER GL_DEBUG_TYPE_OTHER_KHR

#define GL_DEBUG_SEVERITY_HIGH GL_DEBUG_SEVERITY_HIGH_KHR
#define GL_DEBUG_SEVERITY_MEDIUM GL_DEBUG_SEVERITY_MEDIUM_KHR
#define GL_DEBUG_SEVERITY_LOW GL_DEBUG_SEVERITY_LOW_KHR
#define GL_DEBUG_SEVERITY_NOTIFICATION GL_DEBUG_SEVERITY_NOTIFICATION_KHR


#else
#include <SDL2/SDL_opengl.h>
#endif

#if defined(__linux__) && !defined(USE_GLES)
#include <GL/glxew.h>
#endif

#else
#undef RENDERER_OPENGL
#endif


