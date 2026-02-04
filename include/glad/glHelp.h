#pragma once

#include <Windows.h>
#include <stdio.h>

#define glCall(x) \
    while (glGetError() != GL_NO_ERROR); \
    x; \
    { \
        GLenum error = glGetError(); \
        if (error != GL_NO_ERROR) { \
            char buffer[256]; \
            snprintf(buffer, sizeof(buffer), \
                "OpenGL Error: 0x%X at line %d in %s\nFunction: %s\n", \
                error, __LINE__, __FILE__, #x); \
            OutputDebugString(buffer); \
            __debugbreak(); \
        } \
    }
