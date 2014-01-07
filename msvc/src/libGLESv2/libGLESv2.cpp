#include <Windows.h>
#include <stdint.h>
#include "Context.h"

#define GL_VERSION                        0x1F02

#define  JMP_OPENGL_PARAM_0 void
#define  JMP_OPENGL_PARAM_1 uintptr_t p1
#define  JMP_OPENGL_PARAM_2 JMP_OPENGL_PARAM_1, uintptr_t p2
#define  JMP_OPENGL_PARAM_3 JMP_OPENGL_PARAM_2, uintptr_t p3
#define  JMP_OPENGL_PARAM_4 JMP_OPENGL_PARAM_3, uintptr_t p4
#define  JMP_OPENGL_PARAM_5 JMP_OPENGL_PARAM_4, uintptr_t p5
#define  JMP_OPENGL_PARAM_6 JMP_OPENGL_PARAM_5, uintptr_t p6
#define  JMP_OPENGL_PARAM_7 JMP_OPENGL_PARAM_6, uintptr_t p7
#define  JMP_OPENGL_PARAM_8 JMP_OPENGL_PARAM_7, uintptr_t p8
#define  JMP_OPENGL_PARAM_9 JMP_OPENGL_PARAM_8, uintptr_t p9

#define  JMP_OPENGL_PUSH_0
#define  JMP_OPENGL_PUSH_1 \
	__asm push p1
#define  JMP_OPENGL_PUSH_2 \
	__asm push p2 \
	JMP_OPENGL_PUSH_1
#define  JMP_OPENGL_PUSH_3 \
	__asm push p3 \
	JMP_OPENGL_PUSH_2
#define  JMP_OPENGL_PUSH_4 \
	__asm push p4 \
	JMP_OPENGL_PUSH_3
#define  JMP_OPENGL_PUSH_5 \
	__asm push p5 \
	JMP_OPENGL_PUSH_4
#define  JMP_OPENGL_PUSH_6 \
	__asm push p6 \
	JMP_OPENGL_PUSH_5
#define  JMP_OPENGL_PUSH_7 \
	__asm push p7 \
	JMP_OPENGL_PUSH_6
#define  JMP_OPENGL_PUSH_8 \
	__asm push p8 \
	JMP_OPENGL_PUSH_7
#define  JMP_OPENGL_PUSH_9 \
	__asm push p9 \
	JMP_OPENGL_PUSH_8

#define JMP_OPENGL_FUNC_V(n, func) \
	static uintptr_t p ## func = 0; \
	extern "C" void __stdcall \
	func(JMP_OPENGL_PARAM_ ## n) { \
		JMP_OPENGL_PUSH_ ## n \
		__asm call p ## func \
	}

#define JMP_OPENGL_FUNC_R(n, func) \
	static uintptr_t p ## func = 0; \
	extern "C" uintptr_t __stdcall \
	func(JMP_OPENGL_PARAM_ ## n) { \
		uintptr_t retval = 0; \
		{ \
			JMP_OPENGL_PUSH_ ## n \
			__asm call p ## func \
			__asm mov retval, eax \
		} \
		return retval; \
	}

JMP_OPENGL_FUNC_V(1, glActiveTexture)
JMP_OPENGL_FUNC_V(2, glAttachShader)
JMP_OPENGL_FUNC_V(3, glBindAttribLocation)
JMP_OPENGL_FUNC_V(2, glBindBuffer)
JMP_OPENGL_FUNC_V(2, glBindFramebuffer)
JMP_OPENGL_FUNC_V(2, glBindRenderbuffer)
JMP_OPENGL_FUNC_V(4, glBlendColor)
JMP_OPENGL_FUNC_V(1, glBlendEquation)
JMP_OPENGL_FUNC_V(2, glBlendEquationSeparate)
JMP_OPENGL_FUNC_V(4, glBlendFuncSeparate)
JMP_OPENGL_FUNC_V(4, glBufferData)
JMP_OPENGL_FUNC_V(4, glBufferSubData)
JMP_OPENGL_FUNC_R(1, glCheckFramebufferStatus)
JMP_OPENGL_FUNC_V(1, glClearDepthf)
JMP_OPENGL_FUNC_V(1, glCompileShader)
JMP_OPENGL_FUNC_V(8, glCompressedTexImage2D)
JMP_OPENGL_FUNC_V(9, glCompressedTexSubImage2D)
JMP_OPENGL_FUNC_R(0, glCreateProgram)
JMP_OPENGL_FUNC_R(1, glCreateShader)
JMP_OPENGL_FUNC_V(2, glDeleteBuffers)
JMP_OPENGL_FUNC_V(2, glDeleteFramebuffers)
JMP_OPENGL_FUNC_V(1, glDeleteProgram)
JMP_OPENGL_FUNC_V(2, glDeleteRenderbuffers)
JMP_OPENGL_FUNC_V(1, glDeleteShader)
JMP_OPENGL_FUNC_V(2, glDepthRangef)
JMP_OPENGL_FUNC_V(2, glDetachShader)
JMP_OPENGL_FUNC_V(1, glDisableVertexAttribArray)
JMP_OPENGL_FUNC_V(1, glEnableVertexAttribArray)
JMP_OPENGL_FUNC_V(4, glFramebufferRenderbuffer)
JMP_OPENGL_FUNC_V(5, glFramebufferTexture2D)
JMP_OPENGL_FUNC_V(2, glGenBuffers)
JMP_OPENGL_FUNC_V(2, glGenFramebuffers)
JMP_OPENGL_FUNC_V(2, glGenRenderbuffers)
JMP_OPENGL_FUNC_V(1, glGenerateMipmap)
JMP_OPENGL_FUNC_V(7, glGetActiveAttrib)
JMP_OPENGL_FUNC_V(7, glGetActiveUniform)
JMP_OPENGL_FUNC_V(4, glGetAttachedShaders)
JMP_OPENGL_FUNC_R(2, glGetAttribLocation)
JMP_OPENGL_FUNC_V(3, glGetBufferParameteriv)
JMP_OPENGL_FUNC_V(4, glGetFramebufferAttachmentParameteriv)
JMP_OPENGL_FUNC_V(1, glGetProcAddress)
JMP_OPENGL_FUNC_V(4, glGetProgramInfoLog)
JMP_OPENGL_FUNC_V(3, glGetProgramiv)
JMP_OPENGL_FUNC_V(3, glGetRenderbufferParameteriv)
JMP_OPENGL_FUNC_V(4, glGetShaderInfoLog)
JMP_OPENGL_FUNC_V(4, glGetShaderPrecisionFormat)
JMP_OPENGL_FUNC_V(4, glGetShaderSource)
JMP_OPENGL_FUNC_V(3, glGetShaderiv)
JMP_OPENGL_FUNC_R(2, glGetUniformLocation)
JMP_OPENGL_FUNC_V(3, glGetUniformfv)
JMP_OPENGL_FUNC_V(3, glGetUniformiv)
JMP_OPENGL_FUNC_V(3, glGetVertexAttribPointerv)
JMP_OPENGL_FUNC_V(3, glGetVertexAttribfv)
JMP_OPENGL_FUNC_V(3, glGetVertexAttribiv)
JMP_OPENGL_FUNC_R(1, glIsBuffer)
JMP_OPENGL_FUNC_R(1, glIsFramebuffer)
JMP_OPENGL_FUNC_R(1, glIsProgram)
JMP_OPENGL_FUNC_R(1, glIsRenderbuffer)
JMP_OPENGL_FUNC_R(1, glIsShader)
JMP_OPENGL_FUNC_V(1, glLinkProgram)
JMP_OPENGL_FUNC_V(0, glReleaseShaderCompiler)
JMP_OPENGL_FUNC_V(4, glRenderbufferStorage)
JMP_OPENGL_FUNC_V(2, glSampleCoverage)
JMP_OPENGL_FUNC_V(5, glShaderBinary)
JMP_OPENGL_FUNC_V(4, glShaderSource)
JMP_OPENGL_FUNC_V(4, glStencilFuncSeparate)
JMP_OPENGL_FUNC_V(2, glStencilMaskSeparate)
JMP_OPENGL_FUNC_V(4, glStencilOpSeparate)
JMP_OPENGL_FUNC_V(2, glUniform1f)
JMP_OPENGL_FUNC_V(3, glUniform1fv)
JMP_OPENGL_FUNC_V(2, glUniform1i)
JMP_OPENGL_FUNC_V(3, glUniform1iv)
JMP_OPENGL_FUNC_V(3, glUniform2f)
JMP_OPENGL_FUNC_V(3, glUniform2fv)
JMP_OPENGL_FUNC_V(3, glUniform2i)
JMP_OPENGL_FUNC_V(3, glUniform2iv)
JMP_OPENGL_FUNC_V(4, glUniform3f)
JMP_OPENGL_FUNC_V(3, glUniform3fv)
JMP_OPENGL_FUNC_V(4, glUniform3i)
JMP_OPENGL_FUNC_V(3, glUniform3iv)
JMP_OPENGL_FUNC_V(5, glUniform4f)
JMP_OPENGL_FUNC_V(3, glUniform4fv)
JMP_OPENGL_FUNC_V(5, glUniform4i)
JMP_OPENGL_FUNC_V(3, glUniform4iv)
JMP_OPENGL_FUNC_V(4, glUniformMatrix2fv)
JMP_OPENGL_FUNC_V(4, glUniformMatrix3fv)
JMP_OPENGL_FUNC_V(4, glUniformMatrix4fv)
JMP_OPENGL_FUNC_V(1, glUseProgram)
JMP_OPENGL_FUNC_V(1, glValidateProgram)
JMP_OPENGL_FUNC_V(2, glVertexAttrib1f)
JMP_OPENGL_FUNC_V(2, glVertexAttrib1fv)
JMP_OPENGL_FUNC_V(3, glVertexAttrib2f)
JMP_OPENGL_FUNC_V(2, glVertexAttrib2fv)
JMP_OPENGL_FUNC_V(4, glVertexAttrib3f)
JMP_OPENGL_FUNC_V(2, glVertexAttrib3fv)
JMP_OPENGL_FUNC_V(5, glVertexAttrib4f)
JMP_OPENGL_FUNC_V(2, glVertexAttrib4fv)
JMP_OPENGL_FUNC_V(6, glVertexAttribPointer)

#undef JMP_OPENGL_FUNC

static unsigned int
gles2_str_find(const unsigned char *s, unsigned char c) {
	unsigned int i = 0;
	if (s == NULL) return 0;
	while (s[i] != '\0' && s[i] != c) i++;
	return (s[i] == '\0' || s[i] == c) ? i : 0;
}

#define EXPORT_OPENGL_FUNC(func) __pragma(comment(linker, "/export:" ## #func ## "=opengl32." ## #func ))

EXPORT_OPENGL_FUNC(glBindTexture)
EXPORT_OPENGL_FUNC(glBlendFunc)
EXPORT_OPENGL_FUNC(glClear)
EXPORT_OPENGL_FUNC(glClearColor)
EXPORT_OPENGL_FUNC(glClearStencil)
EXPORT_OPENGL_FUNC(glColorMask)
EXPORT_OPENGL_FUNC(glCopyTexImage2D)
EXPORT_OPENGL_FUNC(glCopyTexSubImage2D)
EXPORT_OPENGL_FUNC(glCullFace)
EXPORT_OPENGL_FUNC(glDeleteTextures)
EXPORT_OPENGL_FUNC(glDepthFunc)
EXPORT_OPENGL_FUNC(glDepthMask)
EXPORT_OPENGL_FUNC(glDisable)
EXPORT_OPENGL_FUNC(glDrawArrays)
EXPORT_OPENGL_FUNC(glDrawElements)
EXPORT_OPENGL_FUNC(glEnable)
EXPORT_OPENGL_FUNC(glFinish)
EXPORT_OPENGL_FUNC(glFlush)
EXPORT_OPENGL_FUNC(glFrontFace)
EXPORT_OPENGL_FUNC(glGenTextures)
EXPORT_OPENGL_FUNC(glGetBooleanv)
EXPORT_OPENGL_FUNC(glGetError)
EXPORT_OPENGL_FUNC(glGetFloatv)
EXPORT_OPENGL_FUNC(glGetIntegerv)
EXPORT_OPENGL_FUNC(glGetTexParameterfv)
EXPORT_OPENGL_FUNC(glGetTexParameteriv)
EXPORT_OPENGL_FUNC(glHint)
EXPORT_OPENGL_FUNC(glGetString)
EXPORT_OPENGL_FUNC(glIsEnabled)
EXPORT_OPENGL_FUNC(glIsTexture)
EXPORT_OPENGL_FUNC(glLineWidth)
EXPORT_OPENGL_FUNC(glPixelStorei)
EXPORT_OPENGL_FUNC(glPolygonOffset)
EXPORT_OPENGL_FUNC(glReadPixels)
EXPORT_OPENGL_FUNC(glScissor)
EXPORT_OPENGL_FUNC(glStencilFunc)
EXPORT_OPENGL_FUNC(glStencilMask)
EXPORT_OPENGL_FUNC(glStencilOp)
EXPORT_OPENGL_FUNC(glTexImage2D)
EXPORT_OPENGL_FUNC(glTexParameterf)
EXPORT_OPENGL_FUNC(glTexParameterfv)
EXPORT_OPENGL_FUNC(glTexParameteri)
EXPORT_OPENGL_FUNC(glTexParameteriv)
EXPORT_OPENGL_FUNC(glTexSubImage2D)
EXPORT_OPENGL_FUNC(glViewport)

static bool 
gles2_initialize_13() {
	bool r = false;
	r = ((pglActiveTexture = (uintptr_t)wglGetProcAddress("glActiveTexture")) == 0) || r;
	r = ((pglCompressedTexImage2D = (uintptr_t)wglGetProcAddress("glCompressedTexImage2D")) == 0) || r;
	r = ((pglCompressedTexSubImage2D = (uintptr_t)wglGetProcAddress("glCompressedTexSubImage2D")) == 0) || r;
	r = ((pglSampleCoverage = (uintptr_t)wglGetProcAddress("glSampleCoverage")) == 0) || r;
	return r;
}

static bool 
gles2_initialize_14() {
	bool r = false;
	r = ((pglBlendColor = (uintptr_t)wglGetProcAddress("glBlendColor")) == 0) || r;
	r = ((pglBlendEquation = (uintptr_t)wglGetProcAddress("glBlendEquation")) == 0) || r;
	r = ((pglBlendFuncSeparate = (uintptr_t)wglGetProcAddress("glBlendFuncSeparate")) == 0) || r;
	return r;
}

static bool 
gles2_initialize_15() {
	bool r = false;
	r = ((pglBindBuffer = (uintptr_t)wglGetProcAddress("glBindBuffer")) == 0) || r;
	r = ((pglBufferData = (uintptr_t)wglGetProcAddress("glBufferData")) == 0) || r;
	r = ((pglBufferSubData = (uintptr_t)wglGetProcAddress("glBufferSubData")) == 0) || r;
	r = ((pglDeleteBuffers = (uintptr_t)wglGetProcAddress("glDeleteBuffers")) == 0) || r;
	r = ((pglGenBuffers = (uintptr_t)wglGetProcAddress("glGenBuffers")) == 0) || r;
	r = ((pglGetBufferParameteriv = (uintptr_t)wglGetProcAddress("glGetBufferParameteriv")) == 0) || r;
	r = ((pglIsBuffer = (uintptr_t)wglGetProcAddress("glIsBuffer")) == 0) || r;
	return r;
}

static bool 
gles2_initialize_20() {
	bool r = false;
	r = ((pglAttachShader = (uintptr_t)wglGetProcAddress("glAttachShader")) == 0) || r;
	r = ((pglBindAttribLocation = (uintptr_t)wglGetProcAddress("glBindAttribLocation")) == 0) || r;
	r = ((pglBlendEquationSeparate = (uintptr_t)wglGetProcAddress("glBlendEquationSeparate")) == 0) || r;
	r = ((pglCompileShader = (uintptr_t)wglGetProcAddress("glCompileShader")) == 0) || r;
	r = ((pglCreateProgram = (uintptr_t)wglGetProcAddress("glCreateProgram")) == 0) || r;
	r = ((pglCreateShader = (uintptr_t)wglGetProcAddress("glCreateShader")) == 0) || r;
	r = ((pglDeleteProgram = (uintptr_t)wglGetProcAddress("glDeleteProgram")) == 0) || r;
	r = ((pglDeleteShader = (uintptr_t)wglGetProcAddress("glDeleteShader")) == 0) || r;
	r = ((pglDetachShader = (uintptr_t)wglGetProcAddress("glDetachShader")) == 0) || r;
	r = ((pglDisableVertexAttribArray = (uintptr_t)wglGetProcAddress("glDisableVertexAttribArray")) == 0) || r;
	r = ((pglEnableVertexAttribArray = (uintptr_t)wglGetProcAddress("glEnableVertexAttribArray")) == 0) || r;
	r = ((pglGetActiveAttrib = (uintptr_t)wglGetProcAddress("glGetActiveAttrib")) == 0) || r;
	r = ((pglGetActiveUniform = (uintptr_t)wglGetProcAddress("glGetActiveUniform")) == 0) || r;
	r = ((pglGetAttachedShaders = (uintptr_t)wglGetProcAddress("glGetAttachedShaders")) == 0) || r;
	r = ((pglGetAttribLocation = (uintptr_t)wglGetProcAddress("glGetAttribLocation")) == 0) || r;
	r = ((pglGetProgramInfoLog = (uintptr_t)wglGetProcAddress("glGetProgramInfoLog")) == 0) || r;
	r = ((pglGetProgramiv = (uintptr_t)wglGetProcAddress("glGetProgramiv")) == 0) || r;
	r = ((pglGetShaderInfoLog = (uintptr_t)wglGetProcAddress("glGetShaderInfoLog")) == 0) || r;
	r = ((pglGetShaderSource = (uintptr_t)wglGetProcAddress("glGetShaderSource")) == 0) || r;
	r = ((pglGetShaderiv = (uintptr_t)wglGetProcAddress("glGetShaderiv")) == 0) || r;
	r = ((pglGetUniformLocation = (uintptr_t)wglGetProcAddress("glGetUniformLocation")) == 0) || r;
	r = ((pglGetUniformfv = (uintptr_t)wglGetProcAddress("glGetUniformfv")) == 0) || r;
	r = ((pglGetUniformiv = (uintptr_t)wglGetProcAddress("glGetUniformiv")) == 0) || r;
	r = ((pglGetVertexAttribPointerv = (uintptr_t)wglGetProcAddress("glGetVertexAttribPointerv")) == 0) || r;
	r = ((pglGetVertexAttribfv = (uintptr_t)wglGetProcAddress("glGetVertexAttribfv")) == 0) || r;
	r = ((pglGetVertexAttribiv = (uintptr_t)wglGetProcAddress("glGetVertexAttribiv")) == 0) || r;
	r = ((pglIsProgram = (uintptr_t)wglGetProcAddress("glIsProgram")) == 0) || r;
	r = ((pglIsShader = (uintptr_t)wglGetProcAddress("glIsShader")) == 0) || r;
	r = ((pglLinkProgram = (uintptr_t)wglGetProcAddress("glLinkProgram")) == 0) || r;
	r = ((pglShaderSource = (uintptr_t)wglGetProcAddress("glShaderSource")) == 0) || r;
	r = ((pglStencilFuncSeparate = (uintptr_t)wglGetProcAddress("glStencilFuncSeparate")) == 0) || r;
	r = ((pglStencilMaskSeparate = (uintptr_t)wglGetProcAddress("glStencilMaskSeparate")) == 0) || r;
	r = ((pglStencilOpSeparate = (uintptr_t)wglGetProcAddress("glStencilOpSeparate")) == 0) || r;
	r = ((pglUniform1f = (uintptr_t)wglGetProcAddress("glUniform1f")) == 0) || r;
	r = ((pglUniform1fv = (uintptr_t)wglGetProcAddress("glUniform1fv")) == 0) || r;
	r = ((pglUniform1i = (uintptr_t)wglGetProcAddress("glUniform1i")) == 0) || r;
	r = ((pglUniform1iv = (uintptr_t)wglGetProcAddress("glUniform1iv")) == 0) || r;
	r = ((pglUniform2f = (uintptr_t)wglGetProcAddress("glUniform2f")) == 0) || r;
	r = ((pglUniform2fv = (uintptr_t)wglGetProcAddress("glUniform2fv")) == 0) || r;
	r = ((pglUniform2i = (uintptr_t)wglGetProcAddress("glUniform2i")) == 0) || r;
	r = ((pglUniform2iv = (uintptr_t)wglGetProcAddress("glUniform2iv")) == 0) || r;
	r = ((pglUniform3f = (uintptr_t)wglGetProcAddress("glUniform3f")) == 0) || r;
	r = ((pglUniform3fv = (uintptr_t)wglGetProcAddress("glUniform3fv")) == 0) || r;
	r = ((pglUniform3i = (uintptr_t)wglGetProcAddress("glUniform3i")) == 0) || r;
	r = ((pglUniform3iv = (uintptr_t)wglGetProcAddress("glUniform3iv")) == 0) || r;
	r = ((pglUniform4f = (uintptr_t)wglGetProcAddress("glUniform4f")) == 0) || r;
	r = ((pglUniform4fv = (uintptr_t)wglGetProcAddress("glUniform4fv")) == 0) || r;
	r = ((pglUniform4i = (uintptr_t)wglGetProcAddress("glUniform4i")) == 0) || r;
	r = ((pglUniform4iv = (uintptr_t)wglGetProcAddress("glUniform4iv")) == 0) || r;
	r = ((pglUniformMatrix2fv = (uintptr_t)wglGetProcAddress("glUniformMatrix2fv")) == 0) || r;
	r = ((pglUniformMatrix3fv = (uintptr_t)wglGetProcAddress("glUniformMatrix3fv")) == 0) || r;
	r = ((pglUniformMatrix4fv = (uintptr_t)wglGetProcAddress("glUniformMatrix4fv")) == 0) || r;
	r = ((pglUseProgram = (uintptr_t)wglGetProcAddress("glUseProgram")) == 0) || r;
	r = ((pglValidateProgram = (uintptr_t)wglGetProcAddress("glValidateProgram")) == 0) || r;
	r = ((pglVertexAttrib1f = (uintptr_t)wglGetProcAddress("glVertexAttrib1f")) == 0) || r;
	r = ((pglVertexAttrib1fv = (uintptr_t)wglGetProcAddress("glVertexAttrib1fv")) == 0) || r;
	r = ((pglVertexAttrib2f = (uintptr_t)wglGetProcAddress("glVertexAttrib2f")) == 0) || r;
	r = ((pglVertexAttrib2fv = (uintptr_t)wglGetProcAddress("glVertexAttrib2fv")) == 0) || r;
	r = ((pglVertexAttrib3f = (uintptr_t)wglGetProcAddress("glVertexAttrib3f")) == 0) || r;
	r = ((pglVertexAttrib3fv = (uintptr_t)wglGetProcAddress("glVertexAttrib3fv")) == 0) || r;
	r = ((pglVertexAttrib4f = (uintptr_t)wglGetProcAddress("glVertexAttrib4f")) == 0) || r;
	r = ((pglVertexAttrib4fv = (uintptr_t)wglGetProcAddress("glVertexAttrib4fv")) == 0) || r;
	r = ((pglVertexAttribPointer = (uintptr_t)wglGetProcAddress("glVertexAttribPointer")) == 0) || r;
	return r;
}

static struct glContext *glc = NULL;

struct glContext *__stdcall
glCreateContext() {
	if (glc) {
		return glc;
	}
	else {
		glc = (struct glContext *)malloc(sizeof(struct glContext));
		if (!glc) {
			return NULL;
		}
	}

	glc->lib = LoadLibraryW(L"opengl32.dll");
	if (!glc->lib) {
		free(glc);
		return NULL;
	}

	uintptr_t pglGetString = (uintptr_t)GetProcAddress(glc->lib, "glGetString");
	if (!pglGetString) {
		free(glc);
		return NULL;
	}

	const unsigned char *s = ((const unsigned char *(__stdcall*)(unsigned int))pglGetString)(GL_VERSION);
	unsigned int dot = gles2_str_find(s, '.');
	if (dot == 0) {
		free(glc);
		return NULL;
	}

	if ((s[dot - 1] - '0') < 2) {
		free(glc);
		return NULL;
	}

	bool r = false;
	r = gles2_initialize_13() || r;
	r = gles2_initialize_14() || r;
	r = gles2_initialize_15() || r;
	r = gles2_initialize_20() || r;

	if (!r) {
		free(glc);
		return NULL;
	}

	return glc;
}

void  __stdcall 
glDestroyContext(struct glContext * /*glc*/) {
	// TODO
}
