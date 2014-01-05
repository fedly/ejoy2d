#include <EGL/egl.h>

#include <Windows.h>
#include <assert.h>
#include <malloc.h>
#include <stdlib.h>

static PIXELFORMATDESCRIPTOR* pixelformats_array = NULL;
static EGLint pixelformats_count = 0;
static EGLint pixelformats_vaild_count = 0;
static EGLint current_error = EGL_SUCCESS;

#define GLAPIENTRY _stdcall
#define GLAPIENTRYP _stdcall *

#ifndef PROC
	typedef int (WINAPI *PROC)();
#endif		

DECLARE_HANDLE(HPBUFFERARB);
typedef HPBUFFERARB GL_Surface;

#define WGL_DRAW_TO_PBUFFER_ARB        0x202D
#define WGL_MAX_PBUFFER_PIXELS_ARB     0x202E
#define WGL_MAX_PBUFFER_WIDTH_ARB      0x202F
#define WGL_MAX_PBUFFER_HEIGHT_ARB     0x2030
#define WGL_PBUFFER_LARGEST_ARB        0x2033
#define WGL_PBUFFER_WIDTH_ARB          0x2034
#define WGL_PBUFFER_HEIGHT_ARB         0x2035
#define WGL_PBUFFER_LOST_ARB           0x2036

#define WGL_PIXEL_TYPE_ARB             0x2013
#define WGL_COLOR_BITS_ARB             0x2014
#define WGL_RED_BITS_ARB               0x2015
#define WGL_RED_SHIFT_ARB              0x2016
#define WGL_GREEN_BITS_ARB             0x2017
#define WGL_GREEN_SHIFT_ARB            0x2018
#define WGL_BLUE_BITS_ARB              0x2019
#define WGL_BLUE_SHIFT_ARB             0x201A
#define WGL_ALPHA_BITS_ARB             0x201B
#define WGL_ALPHA_SHIFT_ARB            0x201C
#define WGL_DEPTH_BITS_ARB             0x2022
#define WGL_STENCIL_BITS_ARB           0x2023
#define WGL_BIND_TO_TEXTURE_RGB_ARB    0x2070
#define WGL_BIND_TO_TEXTURE_RGBA_ARB   0x2071
#define WGL_TEXTURE_FORMAT_ARB         0x2072
#define WGL_TEXTURE_TARGET_ARB         0x2073
#define WGL_MIPMAP_TEXTURE_ARB         0x2074
#define WGL_TEXTURE_RGB_ARB            0x2075
#define WGL_TEXTURE_RGBA_ARB           0x2076
#define WGL_NO_TEXTURE_ARB             0x2077
#define WGL_TEXTURE_CUBE_MAP_ARB       0x2078
#define WGL_TEXTURE_1D_ARB             0x2079
#define WGL_TEXTURE_2D_ARB             0x207A
#define WGL_AUX_BUFFERS_ARB            0x2024
#define WGL_DOUBLE_BUFFER_ARB          0x2011
#define	WGL_SAMPLES_ARB				   0x2042

#define WGL_FRONT_LEFT_ARB             0x2083
#define WGL_FRONT_RIGHT_ARB            0x2084
#define WGL_BACK_LEFT_ARB              0x2085
#define WGL_BACK_RIGHT_ARB             0x2086

typedef const char *( GLAPIENTRYP PFNWGLGETEXTENSIONSSTRINGARBPROC )( HDC hdc );
typedef BOOL( GLAPIENTRYP PFNWGLCHOOSEPIXELFORMATARBPROC )( HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats );
typedef HPBUFFERARB( GLAPIENTRYP PFNWGLCREATEPBUFFERARBPROC )( HDC hDC, int iPixelFormat, int iWidth, int iHeight, const int *piAttribList );
typedef HDC( GLAPIENTRYP PFNWGLGETPBUFFERDCARBPROC )( HPBUFFERARB hPbuffer );
typedef BOOL( GLAPIENTRYP PFNWGLQUERYPBUFFERARBPROC )( HPBUFFERARB hPbuffer, int iAttribute, int *piValue );

PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = 0x0;
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = 0x0;
PFNWGLCREATEPBUFFERARBPROC wglCreatePbufferARB = 0x0;
PFNWGLGETPBUFFERDCARBPROC wglGetPbufferDCARB = 0x0;
PFNWGLQUERYPBUFFERARBPROC wglQueryPbufferARB = 0x0;

static void
set_current_error(EGLint error) {
	current_error = error;
}

static EGLint
get_current_error() {
	return current_error;
}

#define EGL_RETURN_ERROR(ec, ret) (set_current_error(ec), (ret))
#define EGL_RETURN_SUCCESS(ret) (set_current_error(EGL_SUCCESS), (ret))

static bool 
get_extension(const char* name, const char* extensions) {
	const char *pos = 0;
	while ((pos = strstr(extensions, name)) != 0) {
		char c = *(pos + strlen(name));
		if (c == ' ' || c == '\0') {
			return true;
		}
		pos += strlen(name);
	}

	return false;
}

static EGLint
pixelformats_get_hardware_support(PIXELFORMATDESCRIPTOR *ppfd) {
	if ((ppfd->dwFlags & PFD_GENERIC_FORMAT) && !(ppfd->dwFlags & PFD_GENERIC_ACCELERATED)) {
		return 1;
	}
	else if ((ppfd->dwFlags & PFD_GENERIC_FORMAT) && (ppfd->dwFlags & PFD_GENERIC_ACCELERATED)) {
		return 2;
	}
	else if (!(ppfd->dwFlags & PFD_GENERIC_FORMAT) && !(ppfd->dwFlags & PFD_GENERIC_ACCELERATED)) {
		return 3;
	}

	return 0;
}

static EGLint
pixelformats_get_surface_type(PIXELFORMATDESCRIPTOR *ppfd) {
	EGLint value = 0;

	if (ppfd->dwFlags & PFD_DRAW_TO_BITMAP) {
		value |= EGL_PIXMAP_BIT;
	}

	if (ppfd->dwFlags & PFD_DRAW_TO_WINDOW) {
		value |= EGL_WINDOW_BIT;
	}

	return value;
}

struct pfsort_t {
	PIXELFORMATDESCRIPTOR** array;
	EGLint                  size;
	EGLint                  current;
	bool                    want_red;
	bool                    want_green;
	bool                    want_blue;
	bool                    want_alpha;
};

static struct pfsort_t *
pfsort_create(unsigned int size) {
	struct pfsort_t *pf = (struct pfsort_t *)malloc(sizeof(struct pfsort_t));
	if (!pf) {
		return NULL;
	}

	memset(pf, 0, sizeof(struct pfsort_t));
	pf->array = (PIXELFORMATDESCRIPTOR**)malloc(size * sizeof(PIXELFORMATDESCRIPTOR*));
	if (!pf->array) {
		free(pf);
		return NULL;
	}

	memset(pf->array, 0, size * sizeof(PIXELFORMATDESCRIPTOR*));
	pf->size = size;
	return pf;
}

static void
pfsort_destory(struct pfsort_t *pf) {
	if (pf) {
		free(pf->array);
		free(pf);
	}
}

static void
pfsort_push(struct pfsort_t *pf, PIXELFORMATDESCRIPTOR* node) {
	assert(pf && pf->array && node);
	assert(pf->current <= pf->size);

	if (pf->current < pf->size) {
		pf->array[pf->current] = node;
		pf->current++;
	}
}

static int
#if defined(_MSC_VER)
pfsort_compare(void* arg, const void *lft, const void *rht) {
#else
pfsort_compare(const void *lft, const void *rht, void* arg) {
#endif
	struct pfsort_t *pf       = (struct pfsort_t *)arg;
	PIXELFORMATDESCRIPTOR* l  = *(PIXELFORMATDESCRIPTOR**)lft;
	PIXELFORMATDESCRIPTOR* r  = *(PIXELFORMATDESCRIPTOR**)rht;

#define EGL_SORT(l, r) if ((l) != (r)) { return ((l) < (r))? 1: -1; }

	EGLint l_size = 0;
	EGLint r_size = 0;
	if (pf->want_red) {
		l_size += l->cRedBits;
		r_size += r->cRedBits;
	}
	if (pf->want_green) {
		l_size += l->cGreenBits;
		r_size += r->cGreenBits;
	}
	if (pf->want_blue) {
		l_size += l->cBlueBits;
		r_size += r->cBlueBits;
	}
	if (pf->want_alpha) {
		l_size += l->cAlphaBits;
		r_size += r->cAlphaBits;
	}

	EGL_SORT(pixelformats_get_hardware_support(l), pixelformats_get_hardware_support(r));
	EGL_SORT(l_size, r_size);
	EGL_SORT(l->cColorBits, r->cColorBits);
	EGL_SORT(l->cDepthBits, r->cDepthBits);
	EGL_SORT(l->cStencilBits, r->cStencilBits);
	EGL_SORT(l - pixelformats_array, r - pixelformats_array);

#undef EGL_SORT

	return 0;
}

static void
pfsort_sort(struct pfsort_t *pf, const EGLint *attrib_list) {
	assert(pf && pf->array && attrib_list);

	pf->want_red   = 0;
	pf->want_green = 0;
	pf->want_blue  = 0;
	pf->want_alpha = 0;
	for (const EGLint* attr = attrib_list; attr[0] != EGL_NONE; attr += 2) {
		if (attr[1] != 0 && attr[1] != EGL_DONT_CARE) {
			switch (attr[0]) {
			case EGL_RED_SIZE:   pf->want_red   = 1; break;
			case EGL_GREEN_SIZE: pf->want_green = 1; break;
			case EGL_BLUE_SIZE:  pf->want_blue  = 1; break;
			case EGL_ALPHA_SIZE: pf->want_alpha = 1; break;
			}
		}
	}

#if defined(_MSC_VER)
	qsort_s(pf->array, pf->current, sizeof(PIXELFORMATDESCRIPTOR*), pfsort_compare, pf);
#else
	qsort_r(pf->array, pf->current, sizeof(PIXELFORMATDESCRIPTOR*), pfsort_compare, pf);
#endif	
}

static unsigned int
pfsort_copy(struct pfsort_t *pf, PIXELFORMATDESCRIPTOR **target, unsigned int target_size) {
	assert(pf && pf->array && target);

	if (pf->current <= 0 || target_size <= 0) {
		return 0;
	}

	if (pf->current > target_size) {
		memcpy(target, pf->array, target_size * sizeof(PIXELFORMATDESCRIPTOR *));
		return target_size;
	}
	else {
		memcpy(target, pf->array, pf->current * sizeof(PIXELFORMATDESCRIPTOR *));
		return pf->current;
	}
}

EGLDisplay EGLAPIENTRY 
eglGetDisplay(EGLNativeDisplayType display_id) {
	return (EGLDisplay)display_id;
}

EGLBoolean EGLAPIENTRY 
eglInitialize(EGLDisplay dpy, EGLint *major, EGLint *minor) {
	if (major) { 
		*major = 1; 
	}
	if (minor) {
		*minor = 1;
	}

	if (dpy) {
		wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
		if (wglGetExtensionsStringARB) {
			const char * wglExtensions = wglGetExtensionsStringARB((HDC)dpy);
			if (get_extension("WGL_ARB_pixel_format", wglExtensions)) {
				wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
			}
			if (get_extension("WGL_ARB_pbuffer", wglExtensions)) {
				wglCreatePbufferARB = (PFNWGLCREATEPBUFFERARBPROC)wglGetProcAddress("wglCreatePbufferARB");
				wglGetPbufferDCARB = (PFNWGLGETPBUFFERDCARBPROC)wglGetProcAddress("wglGetPbufferDCARB");
				wglQueryPbufferARB = (PFNWGLQUERYPBUFFERARBPROC)wglGetProcAddress("wglQueryPbufferARB");
			}
		}
	}

	return EGL_RETURN_SUCCESS(EGL_TRUE);
}

EGLBoolean EGLAPIENTRY 
eglGetConfigs(EGLDisplay dpy, EGLConfig *configs, EGLint config_size, EGLint *num_config) {
	HDC  hdc = (HDC)dpy;
	PIXELFORMATDESCRIPTOR pfd;
	EGLint total = 0;
	EGLint non_support = 0;
	struct pfsort_t *pf = NULL;
	const EGLint attrib_list[] = { EGL_NONE };

	if (!num_config) {
		return EGL_RETURN_ERROR(EGL_BAD_PARAMETER, EGL_FALSE);
	}

	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	total = DescribePixelFormat(hdc, 1, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	if (total <= 0) {
		return EGL_RETURN_ERROR(EGL_BAD_DISPLAY, EGL_FALSE);
	}

	if (!pixelformats_array || (pixelformats_array && total > pixelformats_count)) {
		free(pixelformats_array);
		pixelformats_array = (PIXELFORMATDESCRIPTOR*)malloc(sizeof(PIXELFORMATDESCRIPTOR)* (total));
		if (!pixelformats_array) {
			return EGL_RETURN_ERROR(EGL_BAD_ALLOC, EGL_FALSE);
		}
	}

	if (NULL == (pf = pfsort_create(total))) {
		return EGL_RETURN_ERROR(EGL_BAD_ALLOC, EGL_FALSE);
	}

	for (EGLint n = 1, i = 0; n <= total; ++n) {
		memset(&pixelformats_array[i], 0, sizeof(PIXELFORMATDESCRIPTOR));
		pixelformats_array[i].nSize = sizeof(PIXELFORMATDESCRIPTOR);
		DescribePixelFormat(hdc, n, sizeof(PIXELFORMATDESCRIPTOR), &pixelformats_array[i]);
		if ((pixelformats_array[i].dwFlags & PFD_SUPPORT_OPENGL) && (pixelformats_array[i].dwFlags & PFD_DOUBLEBUFFER)){
			pfsort_push(pf, &pixelformats_array[i]);
			i++;
		}
		else {
			non_support++;
		}
	}

	pixelformats_count = total;
	pixelformats_vaild_count = total - non_support;

	if (configs) {
		pfsort_sort(pf, attrib_list);
		*num_config = pfsort_copy(pf, (PIXELFORMATDESCRIPTOR**)configs, config_size);
	}
	else {
		*num_config = pf->current;
	}
	pfsort_destory(pf);
	return EGL_RETURN_SUCCESS(EGL_TRUE);
}

EGLBoolean EGLAPIENTRY 
eglGetConfigAttrib(EGLDisplay /*dpy*/, EGLConfig config, EGLint attribute, EGLint *value) {
	PIXELFORMATDESCRIPTOR* ppfd = (PIXELFORMATDESCRIPTOR*)config;

	if (!config || !value) {
		return EGL_RETURN_ERROR(EGL_BAD_PARAMETER, EGL_FALSE);
	}

	switch (attribute) {
	case EGL_BUFFER_SIZE:
		*value = ppfd->cColorBits;
		return EGL_RETURN_SUCCESS(EGL_TRUE);
	case EGL_RED_SIZE:
		*value = ppfd->cRedBits;
		return EGL_RETURN_SUCCESS(EGL_TRUE);
	case EGL_GREEN_SIZE:
		*value = ppfd->cGreenBits;
		return EGL_RETURN_SUCCESS(EGL_TRUE);
	case EGL_BLUE_SIZE:
		*value = ppfd->cBlueBits;
		return EGL_RETURN_SUCCESS(EGL_TRUE);
	case EGL_ALPHA_SIZE:
		*value = ppfd->cAlphaBits;
		return EGL_RETURN_SUCCESS(EGL_TRUE);
	case EGL_CONFIG_CAVEAT:
		*value = (ppfd->dwFlags & (PFD_GENERIC_ACCELERATED | PFD_SUPPORT_OPENGL)) == 0 ? EGL_SLOW_CONFIG : EGL_NONE;
		return EGL_RETURN_SUCCESS(EGL_TRUE);
	case EGL_CONFIG_ID:
		*value = ((PIXELFORMATDESCRIPTOR*)config) - pixelformats_array;
		return (pixelformats_array && *value > 0) ? EGL_TRUE : EGL_FALSE;
	case EGL_DEPTH_SIZE:
		*value = ppfd->cDepthBits;
		return EGL_RETURN_SUCCESS(EGL_TRUE);
	case EGL_LEVEL:
	case EGL_MAX_PBUFFER_HEIGHT:
	case EGL_MAX_PBUFFER_WIDTH:
	case EGL_MAX_PBUFFER_PIXELS:
	case EGL_NATIVE_RENDERABLE:
		*value = (ppfd->dwFlags & PFD_DRAW_TO_WINDOW) != 0 || (ppfd->dwFlags & PFD_DRAW_TO_BITMAP) != 0;
		return EGL_RETURN_SUCCESS(EGL_TRUE);
	case EGL_NATIVE_VISUAL_ID:
		*value = ((PIXELFORMATDESCRIPTOR*)config) - pixelformats_array;
		return (pixelformats_array && *value > 0) ? EGL_TRUE : EGL_FALSE;		
	case EGL_NATIVE_VISUAL_TYPE:
		return EGL_FALSE;
	case EGL_SAMPLE_BUFFERS:
		return EGL_FALSE;
	case EGL_SAMPLES:
		return EGL_FALSE;
	case EGL_STENCIL_SIZE:
		*value = ppfd->cStencilBits;
		return EGL_TRUE;
	case EGL_SURFACE_TYPE:
		*value = pixelformats_get_surface_type(ppfd);
		return EGL_TRUE;
	case EGL_TRANSPARENT_TYPE:
		return EGL_FALSE;
	case EGL_TRANSPARENT_RED_VALUE:
		return EGL_FALSE;
	case EGL_TRANSPARENT_GREEN_VALUE:
		return EGL_FALSE;
	case EGL_TRANSPARENT_BLUE_VALUE:
		return EGL_FALSE;
	}
	return EGL_FALSE;
}

EGLBoolean EGLAPIENTRY 
eglChooseConfig(EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config) {
	const EGLint def_attrib_list[] = { EGL_NONE };
	struct pfsort_t *pf = NULL;

	if (!num_config) {
		return EGL_RETURN_ERROR(EGL_BAD_PARAMETER, EGL_FALSE);
	}

	if (!pixelformats_array && !eglGetConfigs(dpy, 0, 0, &pixelformats_vaild_count)) {
		return EGL_FALSE;
	}
	
	if (!attrib_list) {
		attrib_list = def_attrib_list;
	}

	if (NULL == (pf = pfsort_create(pixelformats_vaild_count))) {
		return EGL_RETURN_ERROR(EGL_BAD_ALLOC, EGL_FALSE);
	}

	for (EGLint i = 0; i < pixelformats_count; ++i) {
		if (!(pixelformats_array[i].dwFlags & PFD_SUPPORT_OPENGL)
			|| !(pixelformats_array[i].dwFlags & PFD_DOUBLEBUFFER)){
			continue;
		}

		const EGLint *attribute = attrib_list;
		bool match = true;
		while (attribute[0] != EGL_NONE) {
			switch (attribute[0]) {
			case EGL_BUFFER_SIZE:  match = pixelformats_array[i].cColorBits >= attribute[1];                                       break;
			case EGL_DEPTH_SIZE:   match = pixelformats_array[i].cDepthBits >= attribute[1];                                       break;
			case EGL_SURFACE_TYPE: match = (pixelformats_get_surface_type(&pixelformats_array[i]) & attribute[1]) == attribute[1]; break;
			case EGL_STENCIL_SIZE: match = pixelformats_array[i].cStencilBits >= attribute[1];                                     break;
			case EGL_RED_SIZE:     match = pixelformats_array[i].cRedBits >= attribute[1];                                         break;
			case EGL_GREEN_SIZE:   match = pixelformats_array[i].cGreenBits >= attribute[1];                                       break;
			case EGL_BLUE_SIZE:    match = pixelformats_array[i].cBlueBits >= attribute[1];                                        break;
			case EGL_ALPHA_SIZE:   match = pixelformats_array[i].cAlphaBits >= attribute[1];                                       break;
			}
			if (!match) {
				break;
			}
			attribute += 2;
		}

		if (match) {
			pfsort_push(pf, &pixelformats_array[i]);
		}
	}

	if (configs) {
		pfsort_sort(pf, attrib_list);
		*num_config = pfsort_copy(pf, (PIXELFORMATDESCRIPTOR**)configs, config_size);
	}
	else {
		*num_config = pf->current;
	}

	pfsort_destory(pf);
	return EGL_RETURN_SUCCESS(EGL_TRUE);
}

EGLSurface EGLAPIENTRY 
eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config, EGLNativeWindowType /*win*/, const EGLint * /*attrib_list*/) {
	HDC  hdc = (HDC)dpy; 
	PIXELFORMATDESCRIPTOR *ppfd = (PIXELFORMATDESCRIPTOR *)config;
	int pixelFormat = 0;

	if (!hdc) {
		return EGL_RETURN_ERROR(EGL_BAD_DISPLAY, EGL_NO_SURFACE);
	}
	
	if (!ppfd) {
		return EGL_RETURN_ERROR(EGL_BAD_CONFIG, EGL_NO_SURFACE);
	}

	pixelFormat = ChoosePixelFormat(hdc, ppfd);
	if (pixelFormat == 0) {
		return EGL_RETURN_ERROR(EGL_BAD_CONFIG, EGL_NO_SURFACE);
	}

	if (!SetPixelFormat(hdc, pixelFormat, ppfd)) {
		return EGL_RETURN_ERROR(EGL_BAD_CONFIG, EGL_NO_SURFACE);
	}
	return EGL_RETURN_SUCCESS((EGLSurface)hdc);
}

EGLContext EGLAPIENTRY 
eglGetCurrentContext() {
	return (EGLContext)wglGetCurrentContext();
}

EGLBoolean EGLAPIENTRY 
eglMakeCurrent(EGLDisplay dpy, EGLSurface /*draw*/, EGLSurface /*read*/, EGLContext ctx) {
	if (wglMakeCurrent((HDC)dpy, (HGLRC)ctx)) {
		return EGL_RETURN_SUCCESS(EGL_TRUE);
	}
	return EGL_RETURN_ERROR(EGL_BAD_CONTEXT, EGL_FALSE);
}

EGLSurface EGLAPIENTRY 
eglCreatePbufferSurface(EGLDisplay dpy, EGLConfig /*config*/, const EGLint *attrib_list) {
	HDC hdc = (HDC)dpy;

	const float fAttribList[] = { 0 };

	EGLint iAttribList[] = {
		WGL_DRAW_TO_PBUFFER_ARB, TRUE,
		WGL_BIND_TO_TEXTURE_RGBA_ARB, TRUE,
		WGL_DOUBLE_BUFFER_ARB, FALSE,
		0,
	};

	EGLint pbufferFlags[] = {
		WGL_TEXTURE_FORMAT_ARB, EGL_NO_TEXTURE,
		WGL_TEXTURE_TARGET_ARB, EGL_NO_TEXTURE,
		0
	};

	EGLint width = 0, height = 0;
	EGLenum texture_format = EGL_NO_TEXTURE;
	EGLenum texture_target = EGL_NO_TEXTURE;

	int pixelFormat;
	unsigned int numFormats;
	HPBUFFERARB hPBuffer = NULL;


	if (attrib_list) {
		while (attrib_list[0] != EGL_NONE) {
			switch (attrib_list[0]) {
			case EGL_WIDTH:
				width = attrib_list[1];
				break;
			case EGL_HEIGHT:
				height = attrib_list[1];
				break;
			case EGL_LARGEST_PBUFFER:
				break;
			case EGL_TEXTURE_FORMAT:
				switch (attrib_list[1]) {
				case EGL_NO_TEXTURE:
				case EGL_TEXTURE_RGB:
				case EGL_TEXTURE_RGBA:
					texture_format = attrib_list[1];
					break;
				default:
					return EGL_RETURN_ERROR(EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE);
				}
				break;
			case EGL_TEXTURE_TARGET:
				switch (attrib_list[1]) {
				case EGL_NO_TEXTURE:
				case EGL_TEXTURE_2D:
					texture_target = attrib_list[1];
					break;
				default:
					return EGL_RETURN_ERROR(EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE);
				}
				break;
			case EGL_MIPMAP_TEXTURE:
				if (attrib_list[1] != EGL_FALSE)
					return EGL_RETURN_ERROR(EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE);
				break;
			case EGL_VG_COLORSPACE:
				return EGL_RETURN_ERROR(EGL_BAD_MATCH, EGL_NO_SURFACE);
			case EGL_VG_ALPHA_FORMAT:
				return EGL_RETURN_ERROR(EGL_BAD_MATCH, EGL_NO_SURFACE);
			default:
				return EGL_RETURN_ERROR(EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE);
			}

			attrib_list += 2;
		}
	}

	if (width < 0 || height < 0) {
		return EGL_RETURN_ERROR(EGL_BAD_PARAMETER, EGL_NO_SURFACE);
	}

	if (width == 0 || height == 0) {
		return EGL_RETURN_ERROR(EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE);
	}

	if ((texture_format != EGL_NO_TEXTURE && texture_target == EGL_NO_TEXTURE) ||
		(texture_format == EGL_NO_TEXTURE && texture_target != EGL_NO_TEXTURE)) {
		return EGL_RETURN_ERROR(EGL_BAD_MATCH, EGL_NO_SURFACE);
	}

	if (!wglChoosePixelFormatARB || !wglCreatePbufferARB || !wglGetPbufferDCARB || !wglQueryPbufferARB) {
		return EGL_RETURN_ERROR(EGL_BAD_PARAMETER, EGL_NO_SURFACE);
	}

	pbufferFlags[0] = texture_format;
	pbufferFlags[2] = texture_target;
	if (texture_format == EGL_TEXTURE_RGB) {
		iAttribList[3] = WGL_BIND_TO_TEXTURE_RGB_ARB;
	}
	else if (texture_format == EGL_TEXTURE_RGBA) {
		iAttribList[3] = WGL_BIND_TO_TEXTURE_RGBA_ARB;
	}

	if (!hdc) {
		return EGL_RETURN_ERROR(EGL_BAD_DISPLAY, EGL_NO_SURFACE);
	}

	if (!wglChoosePixelFormatARB(hdc, iAttribList, fAttribList, 1, &pixelFormat, &numFormats)) {
		return EGL_RETURN_ERROR(EGL_BAD_CONFIG, EGL_NO_SURFACE);
	}

	hPBuffer = wglCreatePbufferARB(hdc, pixelFormat, width, height, pbufferFlags);
	if (!hPBuffer) {
		return EGL_RETURN_ERROR(EGL_BAD_SURFACE, EGL_NO_SURFACE);
	}

	hdc = wglGetPbufferDCARB(hPBuffer);
	if (!hdc) {
		return EGL_RETURN_ERROR(EGL_BAD_SURFACE, EGL_NO_SURFACE);
	}
	return EGL_RETURN_SUCCESS((EGLSurface)hdc);
}

EGLContext EGLAPIENTRY 
eglCreateContext(EGLDisplay dpy, EGLConfig /*config*/, EGLContext share_context, const EGLint * /*attrib_list*/) {
	HDC hDC = (HDC)dpy;

	HGLRC hGLRC = wglCreateContext(hDC);
	if (!hGLRC) {
		return EGL_RETURN_ERROR(EGL_BAD_DISPLAY, EGL_NO_SURFACE);
	}

	if (!wglMakeCurrent(hDC, hGLRC)) {
		wglDeleteContext(hGLRC);
		return EGL_RETURN_ERROR(EGL_BAD_CONTEXT, EGL_NO_SURFACE);
	}

	if (share_context != EGL_NO_CONTEXT) {
		wglShareLists((HGLRC)share_context, hGLRC);
	}

	return EGL_RETURN_SUCCESS((EGLContext)hGLRC);
}

EGLBoolean EGLAPIENTRY 
eglSwapBuffers(EGLDisplay dpy, EGLSurface /*surface*/) {
	return SwapBuffers((HDC)dpy) == TRUE ? EGL_TRUE : EGL_FALSE;
}

EGLBoolean EGLAPIENTRY 
eglDestroyContext(EGLDisplay /*dpy*/, EGLContext ctx) {
	return wglDeleteContext((HGLRC)ctx) == TRUE ? EGL_TRUE : EGL_FALSE;
}

EGLBoolean EGLAPIENTRY 
eglDestroySurface(EGLDisplay /*dpy*/, EGLSurface /*surface*/) {
	return EGL_TRUE;
}

EGLBoolean EGLAPIENTRY 
eglTerminate(EGLDisplay /*dpy*/) {
	free(pixelformats_array);
	pixelformats_array = NULL;
	pixelformats_count = 0;

	return EGL_TRUE;
}

EGLint EGLAPIENTRY 
eglGetError(void) {
	EGLint error = get_current_error();
	if (error != EGL_SUCCESS) {
		set_current_error(EGL_SUCCESS);
	}
	return error;
}
