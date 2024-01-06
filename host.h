#pragma once

#include "afx/types.h"

namespace afx {
    extern "C" void* afxhost_malloc(usz size);
    extern "C" void* afxhost_realloc(void* mem, usz size);
    extern "C" void  afxhost_free(void* mem);
    extern "C" usz   afxhost_strlen(const char* str);
    extern "C" int   afxhost_strcmp(const char* a, const char* b);
}