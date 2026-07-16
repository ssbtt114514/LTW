/**
 * Created by: artDev
 * Copyright (c) 2025 artDev, SerpentSpirale, CADIndie.
 * For use under LGPL-3.0
 */

#include <proc.h>
#include <egl.h>
#include "basevertex.h"
void glMultiDrawArrays( GLenum mode, GLint *first, GLsizei *count, GLsizei primcount )
{
    if(!current_context) return;
    // glMultiDrawArraysEXT draws every primitive from the currently bound VBO in a single driver
    // call, with the same semantics as looping glDrawArrays (a count of 0 is a no-op per the spec).
    // Prefer it when available to avoid per-draw call overhead; fall back to the loop otherwise.
    if(es3_functions.glMultiDrawArraysEXT) {
        es3_functions.glMultiDrawArraysEXT(mode, first, count, primcount);
        return;
    }
    for (int i = 0; i < primcount; i++) {
        if (count[i] > 0)
            es3_functions.glDrawArrays(mode, first[i], count[i]);
    }
}

void glMultiDrawElements( GLenum mode, GLsizei *count, GLenum type, const void * const *indices, GLsizei primcount )
{
    if(!current_context) return;
    GLint elementbuffer;
    es3_functions.glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementbuffer);
    es3_functions.glBindBuffer(GL_COPY_WRITE_BUFFER, current_context->multidraw_element_buffer);
    GLsizei total = 0, offset = 0, typebytes = type_bytes(type);
    for (GLsizei i = 0; i < primcount; i++) {
        total += count[i];
    }
    es3_functions.glBufferData(GL_COPY_WRITE_BUFFER, total*typebytes, NULL, GL_STREAM_DRAW);
    for (GLsizei i = 0; i < primcount; i++) {
        GLsizei icount = count[i];
        if(icount == 0) continue;
        icount *= typebytes;
        if(elementbuffer != 0) {
            es3_functions.glCopyBufferSubData(GL_ELEMENT_ARRAY_BUFFER, GL_COPY_WRITE_BUFFER, (GLintptr)indices[i], offset, icount);
        }else {
            es3_functions.glBufferSubData(GL_COPY_WRITE_BUFFER, offset, icount, indices[i]);
        }
        offset += icount;
    }
    es3_functions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current_context->multidraw_element_buffer);
    es3_functions.glDrawElements(mode, total, type, 0);
    if(elementbuffer != 0) es3_functions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

}