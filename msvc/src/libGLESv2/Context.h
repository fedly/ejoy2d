#ifndef LIBGLESV2_CONTEXT_H
#define LIBGLESV2_CONTEXT_H

#include <Windows.h>

struct glContext {
	HMODULE lib;
};

struct glContext *__stdcall glCreateContext();
void              __stdcall glDestroyContext(struct glContext *glc);

#endif
