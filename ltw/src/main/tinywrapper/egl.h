/**
 * Created by: artDev
 * Copyright (c) 2025 artDev, SerpentSpirale, CADIndie.
 * For use under LGPL-3.0
 */

#ifndef POJAVLAUNCHER_EGL_H
#define POJAVLAUNCHER_EGL_H

#include <stdbool.h>
#include <EGL/egl.h>
#include "proc.h"
#include "unordered_map/unordered_map.h"

#define MAX_BOUND_BUFFERS 9
#define MAX_BOUND_BASEBUFFERS 4
#define MAX_DRAWBUFFERS 8
#define MAX_FBTARGETS 8
#define MAX_TMUS 8
#define MAX_TEXTARGETS 8
#define FORMAT_CACHE_SIZE 64

typedef struct {
    bool ready;
    GLuint indirectRenderBuffer;
} basevertex_renderer_t;

typedef struct {
    bool available;
    PFNGLBLENDEQUATIONIPROC blendequationi;
    PFNGLBLENDEQUATIONSEPARATEIPROC blendequationseparatei;
    PFNGLBLENDFUNCIPROC blendfunci;
    PFNGLBLENDFUNCSEPARATEIPROC blendfuncseparatei;
    PFNGLCOLORMASKIPROC colormaski;
} blending_functions_t;

typedef struct {
    GLuint index;
    GLuint buffer;
    bool ranged;
    GLintptr  offset;
    GLintptr  size;
} basebuffer_binding_t;

typedef struct {
    GLuint color_targets[MAX_FBTARGETS];
    GLuint color_objects[MAX_FBTARGETS];
    GLuint color_levels[MAX_FBTARGETS];
    GLuint color_layers[MAX_FBTARGETS];
    GLenum virt_drawbuffers[MAX_DRAWBUFFERS];
    GLenum phys_drawbuffers[MAX_DRAWBUFFERS];
    GLsizei nbuffers;
} framebuffer_t;

typedef struct {
    bool ready;
    GLuint temp_texture;
    GLuint tempfb;
    GLuint destfb;
    void* depthData;
    GLsizei depthWidth, depthHeight;
} framebuffer_copier_t;

typedef struct {
    GLenum original_swizzle[4];
    GLenum applied_swizzle[4];
    GLenum pending_swizzle[4];
    GLboolean goofy_byte_order;
    GLboolean upload_bgra;
    GLboolean has_pending_update;
    GLenum texture_target;
} texture_swizzle_track_t;

typedef struct {
    GLint internalformat;
    GLenum type;
    GLenum format;
    bool valid;
} format_cache_entry_t;

typedef struct mempool mempool_t;

typedef struct {
    EGLContext phys_context;
    bool context_rdy;
    bool es31, es32, buffer_storage, buffer_texture_ext, multidraw_indirect, timer_query;
    bool framebuffer_no_attachments, vertex_attrib_binding;
    GLint shader_version;
    basevertex_renderer_t basevertex;
    PFNGLDRAWELEMENTSBASEVERTEXPROC drawelementsbasevertex;
    blending_functions_t blending;
    GLuint multidraw_element_buffer;
    framebuffer_copier_t framebuffer_copier;
    unordered_map* shader_map;
    unordered_map* program_map;
    unordered_map* framebuffer_map;
    unordered_map* texture_swztrack_map;
    unordered_map* bound_basebuffers[MAX_BOUND_BASEBUFFERS];
    int proxy_width, proxy_height, proxy_intformat, maxTextureSize;
    GLint max_drawbuffers;
    GLuint bound_buffers[MAX_BOUND_BUFFERS];
    GLuint program;
    GLuint draw_framebuffer;
    GLuint read_framebuffer;
    char* extensions_string;
    size_t extensions_capacity;
    size_t nextras;
    int nextensions_es;
    char** extra_extensions_array;
    format_cache_entry_t format_cache[FORMAT_CACHE_SIZE];
    int format_cache_index;
    GLsizei multidraw_buffer_size;
    mempool_t* shader_info_pool;
    mempool_t* program_info_pool;
    mempool_t* framebuffer_pool;
    mempool_t* swizzle_track_pool;
    GLuint pending_swizzle_textures[64];
    int pending_swizzle_count;
    bool swizzle_batch_mode;
    struct {
        void (*glDrawArrays)(GLenum, GLint, GLsizei);
        void (*glDrawElements)(GLenum, GLsizei, GLenum, const void*);
        void (*glBindBuffer)(GLenum, GLuint);
        void (*glBindTexture)(GLenum, GLuint);
        void (*glTexParameteri)(GLenum, GLenum, GLint);
        void (*glGetTexParameteriv)(GLenum, GLenum, GLint*);
        void (*glGetIntegerv)(GLenum, GLint*);
        void (*glBufferData)(GLenum, GLsizeiptr, const void*, GLenum);
        void (*glBufferSubData)(GLenum, GLintptr, GLsizeiptr, const void*);
        void (*glCopyBufferSubData)(GLenum, GLenum, GLintptr, GLintptr, GLsizeiptr);
        void* (*glMapBufferRange)(GLenum, GLintptr, GLsizeiptr, GLbitfield);
        unsigned char (*glUnmapBuffer)(GLenum);
        void (*glFlushMappedBufferRange)(GLenum, GLintptr, GLsizeiptr);
    } fast_gl;
    size_t multidraw_ring_head;
    size_t multidraw_ring_tail;
    bool multidraw_ring_wrapped;
} context_t;

extern thread_local context_t *current_context;
extern void init_egl();
extern GLenum get_textarget_query_param(GLenum target);

#endif //POJAVLAUNCHER_EGL_H
