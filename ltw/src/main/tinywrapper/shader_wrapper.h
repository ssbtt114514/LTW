/**
 * Created by: artDev
 * Copyright (c) 2025 artDev, SerpentSpirale, CADIndie.
 * For use under LGPL-3.0
 */

#ifndef GL4ES_WRAPPER_SHADER_WRAPPER_H
#define GL4ES_WRAPPER_SHADER_WRAPPER_H

#include "egl.h"
#include <GLES3/gl3.h>

/* The shader_info_t struct holds the per-shader metadata for the LTW
 * shader wrapper. It is allocated in glCreateShader() (see
 * shader_wrapper.c) and used for caching the shader source. */
typedef struct {
    GLenum shader_type;
    const GLchar* source;
} shader_info_t;

/* The program_info_t struct holds the per-program metadata for the LTW
 * shader wrapper. It is allocated in glCreateProgram() (see
 * shader_wrapper.c) and used for storing the bound fragment shader id
 * and per-attachment color bindings. */
typedef struct {
    GLuint frag_shader;
    GLchar* colorbindings[MAX_DRAWBUFFERS];
} program_info_t;

#endif //GL4ES_WRAPPER_SHADER_WRAPPER_H
