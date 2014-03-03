#ifdef _FX_OPENGLES_SUPPORTED_
#include "../../../include/fxge/fx_ge.h"

#include "./include/GLES2/gl2.h"
#include "./include/EGL/egl.h"
//#include "/home/project/soft/android-ndk-r8/platforms/android-9/arch-arm/usr/include/EGL/egl.h"
#include "./fxgl_int.h"
#include "../../../include/fxge/fx_ge_opengles.h"

//#define _FOXIT_DEBUG_

#include "../agg/fx_agg_driver.h"
#ifndef _FX_NO_NAMESPACE_
using namespace agg;
#endif


static GLuint _positionSlot;
static GLuint _colorSlot;
static void create_shaders()
{
	FOXIT_DEBUG1("Into create_shaders");
	static const char *fragShaderText =
		"varying vec4 v_color;\n"
		"void main() {\n"
		"   gl_FragColor = v_color;\n"
		"}\n";
	static const char *vertShaderText =
		"attribute vec4 pos;\n"
		"attribute vec4 color;\n"
		"varying vec4 v_color;\n"
		"void main() {\n"
		"   gl_Position = pos;\n"
		"   v_color = color;\n"
		"}\n";
	GLuint fragShader, vertShader, program;
	GLint stat;
	FOXIT_DEBUG1("Into create_shaders:glCreateShader");
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	FOXIT_DEBUG1("Into create_shaders:glShaderSource");
	glShaderSource(fragShader, 1, (const char **) &fragShaderText, NULL);
	FOXIT_DEBUG1("Into create_shaders:glCompileShader");
	glCompileShader(fragShader);
	FOXIT_DEBUG1("Into create_shaders:glGetShaderiv");
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &stat);
	if (!stat) {
		FOXIT_DEBUG1("Error: fragment shader did not compile!\n");
		return;
	}
	FOXIT_DEBUG1("Into create_shaders:glCreateShader");
	vertShader = glCreateShader(GL_VERTEX_SHADER);
	FOXIT_DEBUG1("Into create_shaders:glShaderSource");
	glShaderSource(vertShader, 1, (const char **) &vertShaderText, NULL);
	FOXIT_DEBUG1("Into create_shaders:glCompileShader");
	glCompileShader(vertShader);
	FOXIT_DEBUG1("Into create_shaders:glGetShaderiv");
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &stat);
	if (!stat) {
		FOXIT_DEBUG1("Error: vertex shader did not compile!\n");
		return;
	}
	FOXIT_DEBUG1("Into create_shaders:glCreateProgram");
	program = glCreateProgram();
	FOXIT_DEBUG1("Into create_shaders:glAttachShader");
	glAttachShader(program, fragShader);
	FOXIT_DEBUG1("Into create_shaders:glAttachShader");
	glAttachShader(program, vertShader);
	FOXIT_DEBUG1("Into create_shaders:glLinkProgram");
	glLinkProgram(program);
	FOXIT_DEBUG1("Into create_shaders:glGetProgramiv");
	glGetProgramiv(program, GL_LINK_STATUS, &stat);
	if (!stat) {
		char log[1000];
		GLsizei len;
		FOXIT_DEBUG1("Into create_shaders:glGetProgramInfoLog");
		glGetProgramInfoLog(program, 1000, &len, log);
		FOXIT_DEBUG2("Error: linking:\n%s\n", log);
		return;
	}
	FOXIT_DEBUG1("Into create_shaders:glUseProgram");
	glUseProgram(program);
	
	/* test automatic attrib locations */
	FOXIT_DEBUG1("Into create_shaders:glGetAttribLocation");
	_positionSlot = glGetAttribLocation(program, "pos");
	FOXIT_DEBUG1("Into create_shaders:glGetAttribLocation");
	_colorSlot = glGetAttribLocation(program, "color");
	FOXIT_DEBUG1("Into create_shaders:glEnableVertexAttribArray");
	 glEnableVertexAttribArray(_positionSlot); 
	FOXIT_DEBUG1("Into create_shaders:glEnableVertexAttribArray");   
	 glEnableVertexAttribArray(_colorSlot);
}

#ifdef _FX_RGB565_DEVICE_SUPPORT_

static const EGLint CONFIG_ATTRIBS[] =   
{
	EGL_RED_SIZE, 5,
	EGL_GREEN_SIZE, 6,
	EGL_BLUE_SIZE, 5,	
	EGL_ALPHA_SIZE, 0,	
	EGL_RENDERABLE_TYPE,
	EGL_OPENGL_ES2_BIT,
	EGL_SURFACE_TYPE,
	EGL_WINDOW_BIT,EGL_PIXMAP_BIT | EGL_PBUFFER_BIT,
	EGL_SAMPLE_BUFFERS, 
	GL_FALSE,
	EGL_NONE
};
#else
static const EGLint CONFIG_ATTRIBS[] =
{
	EGL_RED_SIZE,		8,	//bits of Red in the color buffer
	EGL_GREEN_SIZE, 	8,	//bits of Green in the color buffer
	EGL_BLUE_SIZE,		8,	//bits of Blue in the color buffer
	EGL_ALPHA_SIZE, 	8,	//bits of Alpha in the color buffer
	EGL_RENDERABLE_TYPE,
	EGL_OPENGL_ES2_BIT,
	EGL_SURFACE_TYPE,	
	EGL_WINDOW_BIT|EGL_PIXMAP_BIT|EGL_PBUFFER_BIT,//bitmask which types of EGL surfaces are supported. (always EGL_WINDOW_BIT | EGL_PIXMAP_BIT | EGL_PBUFFER_BIT | EGL_VG_COLORSPACE_LINEAR_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT)
	EGL_SAMPLES,1,	//integer number of samples per pixel
	EGL_NONE		//attributes list terminal
};
#endif

void GlesInit(int left, int top, int width, int height)
{
	glClearColor(0.2f, 7.0f, 0.8f, 0.7f);
	glEnable(GL_CULL_FACE);
	glViewport(left, top, width, height);
	glClear(GL_COLOR_BUFFER_BIT);
	//glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	
}
void CFX_GEModule::InitPlatform()
{
	EGLDisplay	hEglDisplay = NULL;
	EGLConfig	hEglConfig = NULL;
	hEglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(hEglDisplay, NULL, NULL);
	if (eglGetError() != EGL_SUCCESS)
		return;
	eglBindAPI(EGL_OPENGL_ES_API);
	if (eglGetError() != EGL_SUCCESS)
		return;
	EGLint nMaxMatchConfigs = 0, nNumConfigs = 0;
	if (eglGetConfigs(hEglDisplay, NULL, 0, &nNumConfigs) == EGL_FALSE)
		return;
	if (nNumConfigs <= 0)  
		return;
	EGLConfig* configs = NULL;
	COpenglEsPlatform* pPlatformData = NULL;

	configs = (EGLConfig*)FX_Alloc(FX_BYTE, sizeof(EGLConfig)*nNumConfigs);
	if (configs == NULL)
	    return;
	if (!eglGetConfigs(hEglDisplay, NULL, 0, &nMaxMatchConfigs)) {
		FX_Free(configs);
		return;
	}
	if (nMaxMatchConfigs == 0) {
		FX_Free(configs);
		return;
	}
	if (!eglGetConfigs(hEglDisplay, configs, nMaxMatchConfigs, &nMaxMatchConfigs)) {
		FX_Free(configs);
		configs = NULL;
		return;
	}
	EGLint configs_value;
	int i;
	for (i = 0;i < nMaxMatchConfigs; i++) {
		eglGetConfigAttrib(hEglDisplay, configs[i], EGL_RED_SIZE, &configs_value);
		if (configs_value != 8) continue;
		eglGetConfigAttrib(hEglDisplay, configs[i], EGL_GREEN_SIZE, &configs_value);
		if (configs_value != 8) continue;
		eglGetConfigAttrib(hEglDisplay, configs[i], EGL_BLUE_SIZE, &configs_value);
		if (configs_value != 8) continue;
		eglGetConfigAttrib(hEglDisplay, configs[i], EGL_SURFACE_TYPE, &configs_value);
		int flag = configs_value&EGL_PIXMAP_BIT;
		if (!flag) continue;
		FOXIT_DEBUG2("CFX_GEModule::InitPlatform:eglGetConfigs:flag = %d", flag);
		//eglGetConfigAttrib(hEglDisplay, configs[i], EGL_ALPHA_SIZE, &configs_value);
		//if (configs_value != 8) continue;
		eglGetConfigAttrib(hEglDisplay, configs[i], EGL_RENDERABLE_TYPE, &configs_value);  
		//if (configs_value != EGL_OPENGL_ES2_BIT) continue;
		//eglGetConfigAttrib(hEglDisplay, configs[i], EGL_SAMPLES, &configs_value);
		//if (configs_value != 1) continue;
		hEglConfig = configs[i];
		break;
	}
	FX_Free(configs);
	configs = NULL;
	if (eglGetError() != EGL_SUCCESS)
		return;
	pPlatformData = FX_NEW COpenglEsPlatform;
	pPlatformData->m_hEglDisplay = hEglDisplay;
	pPlatformData->m_hEglConfig = hEglConfig;
	pPlatformData->m_nMaxMatchConfigs = nMaxMatchConfigs;

#ifdef _USE_EXTERNAL_NV_
	// We need get external string 
	// CFX_ByteString external(eglGetString(GL_EXTENSIONS));
	pPlatformData->m_OpenglEsNvExt.Load();
#endif

	m_pPlatformData = pPlatformData;
}
void CFX_GEModule::DestroyPlatform()
{
	if (m_pPlatformData) {
		COpenglEsPlatform* pPlatformData = (COpenglEsPlatform*)m_pPlatformData;
		eglMakeCurrent(pPlatformData->m_hEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		FXSYS_assert(eglGetError() == EGL_SUCCESS);
		eglTerminate(pPlatformData->m_hEglDisplay);
		FXSYS_assert(eglGetError() == EGL_SUCCESS);
		eglReleaseThread();
		delete pPlatformData;
	}
	m_pPlatformData = NULL;
}
CFX_OpenGlesDevice::CFX_OpenGlesDevice()
{
	m_bOwnedBitmap = FALSE;
}
FX_BOOL CFX_OpenGlesDevice::Attach(CFX_DIBitmap* pBitmap, int dither_bits, FX_BOOL bRgbByteOrder, CFX_DIBitmap* pOriDevice, FX_BOOL bGroupKnockout)
{
	if (pBitmap == NULL) 
		return FALSE;
	SetBitmap(pBitmap);
	FX_BOOL ret = TRUE;
	CFX_OpenGLDeviceDriver* pDriver = NULL;

	pDriver	= FX_NEW CFX_OpenGLDeviceDriver;
	if (!pDriver->Create(pBitmap, dither_bits, bRgbByteOrder, pOriDevice, bGroupKnockout))
	{
		delete pDriver;
		return FALSE;
	}

	SetDeviceDriver(pDriver);
	return TRUE;
}
FX_BOOL CFX_OpenGlesDevice::Create(int width, int height, FXDIB_Format format, int dither_bits, CFX_DIBitmap* pOriDevice)
{
	m_bOwnedBitmap = TRUE;
	FX_BOOL ret = TRUE;
	CFX_DIBitmap* pBitmap = NULL;
	CFX_OpenGLDeviceDriver* pDriver = NULL;

	pBitmap = FX_NEW CFX_DIBitmap;
	if (!pBitmap->Create(width, height, format))
	{
		delete pBitmap;
		return FALSE;
	}
	SetBitmap(pBitmap);
	pDriver = FX_NEW CFX_OpenGLDeviceDriver;
	ret = pDriver->Create(pBitmap, dither_bits, FALSE, pOriDevice, FALSE);

	if (!ret) {
		if (pBitmap)
			delete pBitmap;
		if (pDriver)
			delete pDriver;
		return FALSE;
	}
	SetDeviceDriver(pDriver);
	return TRUE;
}
FX_BOOL CFX_OpenGlesDevice::Attach(EGLNativeWindowType pWindowHandle)
{
	CFX_OpenGLDeviceDriver* pDriver = NULL;
	FX_BOOL ret = TRUE;

	pDriver = FX_NEW CFX_OpenGLDeviceDriver;
	ret = pDriver->Create(pWindowHandle);
	if (!ret) {
		delete pDriver;
		return FALSE;
	}

	if (!ret) {
		if (pDriver)
			delete pDriver;
		return FALSE;
	}
	SetDeviceDriver(pDriver);
	return TRUE;
}
CFX_OpenGlesDevice::~CFX_OpenGlesDevice()
{
	if (m_bOwnedBitmap && GetBitmap()) delete GetBitmap();
}
FX_BOOL CFX_OpenGLDeviceDriver::Create(CFX_DIBitmap* pBitmap, int dither_bits, FX_BOOL bRgbByteOrder, CFX_DIBitmap* pOriDevice, FX_BOOL bGroupKnockout)
{
	FXSYS_assert(pBitmap->GetBPP() >= 8);
	m_pNativePixmap = (EGLNativePixmapType)FX_Alloc(FX_BYTE, sizeof(egl_native_pixmap_t));
	m_pBitmap = pBitmap;
	m_DitherBits = dither_bits;
	m_pClipRgn = NULL;
	m_bRgbByteOrder = bRgbByteOrder;
	m_pOriDevice = pOriDevice;
	m_bGroupKnockout = bGroupKnockout;
	switch(pBitmap->GetFormat())
	{
	case FXDIB_Rgb:
		m_pNativePixmap->format = GL_RGB;
		break;
	case FXDIB_Rgb32:
		//pBitmap->Clear();
		m_pNativePixmap->format = GL_RGBA;
		break;
	case FXDIB_Argb:
		m_pNativePixmap->format = GL_RGBA;
		break;
	default:
		m_pNativePixmap->format = GL_LUMINANCE;// 8bpp gray-scale surface
	}
	m_pNativePixmap->width = pBitmap->GetWidth();
	m_pNativePixmap->height = pBitmap->GetHeight();
	m_pNativePixmap->stride = (int)pBitmap->GetPitch();
	m_pNativePixmap->data = pBitmap->GetBuffer();
	m_pNativePixmap->version = 32;
	m_hEglDisplay = ((COpenglEsPlatform*)CFX_GEModule::Get()->GetPlatformData())->m_hEglDisplay;
	m_hEglConfig = ((COpenglEsPlatform*)CFX_GEModule::Get()->GetPlatformData())->m_hEglConfig;
	m_hOldEglSurface = eglGetCurrentSurface(EGL_DRAW);
	m_hOldEglContext = eglGetCurrentContext();
	m_hCurEglContext = eglCreateContext(m_hEglDisplay, m_hEglConfig, NULL, NULL);
	FXSYS_assert(eglGetError() == EGL_SUCCESS);
	m_hCurEglSurface = eglCreatePixmapSurface(m_hEglDisplay,m_hEglConfig,m_pNativePixmap,NULL);
	//FXSYS_assert(eglGetError() == EGL_SUCCESS);
	if (eglGetError() != EGL_SUCCESS)
		return FALSE;
	eglMakeCurrent(m_hEglDisplay, m_hCurEglSurface, m_hCurEglSurface, m_hCurEglContext);
	FXSYS_assert(eglGetError() == EGL_SUCCESS);
	// Init GLES.
	GlesInit(0, 0, pBitmap->GetWidth(), pBitmap->GetHeight());
	//Test
	int width = GetDeviceCaps(FXDC_PIXEL_WIDTH);
	int height = GetDeviceCaps(FXDC_PIXEL_HEIGHT);
	FOXIT_DEBUG3("CFX_OpenGLDeviceDriver::CFX_OpenGLDeviceDriver, width = %d, height = %d", width, height);
	return TRUE;
}
FX_BOOL CFX_OpenGLDeviceDriver::Create(EGLNativeWindowType pWindowHandle)
{
	m_pNativePixmap = NULL;
	m_pClipRgn = NULL;
	m_pBitmap = NULL;
	m_DitherBits = NULL;
	m_pClipRgn = NULL;
	m_bRgbByteOrder = FALSE;
	m_pOriDevice = NULL;
	m_bGroupKnockout = FALSE;
	m_hEglDisplay = ((COpenglEsPlatform*)CFX_GEModule::Get()->GetPlatformData())->m_hEglDisplay;
	m_hEglConfig = ((COpenglEsPlatform*)CFX_GEModule::Get()->GetPlatformData())->m_hEglConfig;
	
	// Create surface and context
	m_hOldEglSurface = eglGetCurrentSurface(EGL_DRAW);
	m_hOldEglContext = eglGetCurrentContext();
	
	m_hCurEglContext = eglCreateContext(m_hEglDisplay, m_hEglConfig, NULL, NULL);
	FXSYS_assert(eglGetError() == EGL_SUCCESS);
	m_hCurEglSurface = eglCreateWindowSurface(m_hEglDisplay, m_hEglConfig, pWindowHandle, NULL);
	FXSYS_assert(eglGetError() == EGL_SUCCESS);
	eglMakeCurrent(m_hEglDisplay, m_hCurEglSurface, m_hCurEglSurface, m_hCurEglContext);
	FXSYS_assert(eglGetError() == EGL_SUCCESS);
	eglQuerySurface(m_hEglDisplay, m_hCurEglSurface, EGL_WIDTH, (EGLint*)&m_Width);
	eglQuerySurface(m_hEglDisplay, m_hCurEglSurface, EGL_HEIGHT,(EGLint*)&m_Height);
	// Init GLES.
	GlesInit(0, 0, m_Width, m_Height);
	return TRUE;
}
CFX_OpenGLDeviceDriver::~CFX_OpenGLDeviceDriver()
{
	eglDestroySurface(m_hEglDisplay, m_hCurEglSurface);
	eglDestroyContext(m_hEglDisplay, m_hCurEglContext);
	
	eglMakeCurrent(m_hEglDisplay, m_hOldEglSurface, m_hOldEglSurface, m_hOldEglContext);
	FXSYS_assert(eglGetError() == EGL_SUCCESS);
	
	if(m_pNativePixmap) {
		FX_Free(m_pNativePixmap);
		m_pNativePixmap = NULL;
	}
	if (m_pClipRgn)
		delete m_pClipRgn;
	for (int i = 0; i < m_StateStack.GetSize(); i ++)
		if (m_StateStack[i]) delete (CFX_ClipRgn*)m_StateStack[i];
}
int	CFX_OpenGLDeviceDriver::GetDeviceCaps(int caps_id)
{
	EGLint ret = EGL_UNKNOWN;
	switch (caps_id) {
	case FXDC_HORZ_SIZE:
		eglQuerySurface(m_hEglDisplay,m_hCurEglSurface, EGL_HORIZONTAL_RESOLUTION, &ret);
		return ret == EGL_UNKNOWN ? 0 : ret;
	case FXDC_VERT_SIZE:
		eglQuerySurface(m_hEglDisplay,m_hCurEglSurface, EGL_VERTICAL_RESOLUTION, &ret);
		return ret == EGL_UNKNOWN ? 0 : ret;
	case FXDC_DEVICE_CLASS:
		return FXDC_DISPLAY;
	case FXDC_PIXEL_WIDTH:
		eglQuerySurface(m_hEglDisplay, m_hCurEglSurface, EGL_WIDTH, &ret);
		return ret == EGL_UNKNOWN ? 0 : ret;
	case FXDC_PIXEL_HEIGHT:
		eglQuerySurface(m_hEglDisplay, m_hCurEglSurface, EGL_HEIGHT, &ret);
		return ret == EGL_UNKNOWN ? 0 : ret;
	case FXDC_BITS_PIXEL:
		{
			if (m_pBitmap) {
#ifndef _FX_RGB565_DEVICE_SUPPORT_
				return 32;
#else 
				return 16;
#endif
			} else {
				return 0;
			}
		}
	case FXDC_RENDER_CAPS:{
		int flags = FXRC_ALPHA_PATH | FXRC_ALPHA_IMAGE;
		if (m_pBitmap&&m_pBitmap->HasAlpha()) flags |= FXRC_ALPHA_OUTPUT|FXRC_SOFT_CLIP|FXRC_BLEND_MODE|FXRC_GET_BITS;
		return flags;
		
	}		
	case FXDC_DITHER_BITS:
		return m_DitherBits;
	}
	return 0;
}
void CFX_OpenGLDeviceDriver::SaveState()
{
	// the only thing we need to save is the clipping
	void* pClip = NULL;
	if (m_pClipRgn)
		pClip = FX_NEW CFX_ClipRgn(*m_pClipRgn);

	m_StateStack.Add(pClip);

}
void CFX_OpenGLDeviceDriver::RestoreState(FX_BOOL bKeepSaved)
{
	if (m_StateStack.GetSize() == 0) {
		if (m_pClipRgn) {
			delete m_pClipRgn;
			m_pClipRgn = NULL;
		}
		return;
	}
	CFX_ClipRgn* pSavedClip = (CFX_ClipRgn*)m_StateStack[m_StateStack.GetSize()-1];
	if (m_pClipRgn) {
		delete m_pClipRgn;
		m_pClipRgn = NULL;
	}
	if (bKeepSaved) {
		if (pSavedClip)
			m_pClipRgn = FX_NEW CFX_ClipRgn(*pSavedClip);
	} else {
		m_StateStack.RemoveAt(m_StateStack.GetSize()-1);
		m_pClipRgn = pSavedClip;
	}
}
/*
GLuint CFX_OpenGLDeviceDriver::GL_SetupTexture(const CFX_DIBSource* pBitmap, FX_DWORD color, const FX_RECT* pSrcRect, 
												int dest_left, int dest_top, int blend_type, 
												int alpha_flag, void* pIccTransform)
{
	GLuint texName;   
	int width = pSrcRect->Width();
	int height = pSrcRect->Height();
	glGenTextures(1, &texName);    
	glBindTexture(GL_TEXTURE_2D, texName);     
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);      
	glTexImage2D(GL_TEXTURE_2D, 0, GL_BGR_EXT, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBitmap->GetBuffer());
	return texName;

}
*/
FX_BOOL CFX_OpenGLDeviceDriver::GL_SetClipMask(const CFX_PathData* pPathData, 
											   const CFX_AffineMatrix* pObject2Device, 
											   const CFX_GraphStateData* pGraphState, 
											   FX_ARGB fill_argb, FX_ARGB stroke_argb, 
											   int fill_mode)
{
	// Get render device dimension
	CFX_FloatRect path_rect_f = pPathData->GetBoundingBox();
	if (pObject2Device) pObject2Device->TransformRect(path_rect_f);
	FX_RECT path_rect = path_rect_f.GetOutterRect();
	path_rect.Intersect(0, 0, (FX_FIXFLOAT)GetDeviceCaps(FXDC_PIXEL_WIDTH), (FX_FIXFLOAT)GetDeviceCaps(FXDC_PIXEL_HEIGHT));
	
	// Create a gray scale buffer as render device
	CFX_DIBitmapRef mask;
	CFX_DIBitmap* pThisLayer = mask.New();
	pThisLayer->Create(path_rect.Width(), path_rect.Height(), FXDIB_8bppMask);
	pThisLayer->Clear(0);
	
	// Create render driver
	CFX_OpenGLDeviceDriver clipRenderer;
	clipRenderer.Create(pThisLayer, 0, 0, 0, 0);
	
	// Get render matrix
	CFX_AffineMatrix matrix;
	if (pObject2Device){
		matrix.Copy(*pObject2Device);
	}
	matrix.TranslateI(-path_rect.left, -path_rect.top);
	
	// Render clip
	FX_BOOL ret = clipRenderer.DrawPath(pPathData, &matrix, pGraphState, fill_argb, stroke_argb, fill_mode, 0, NULL);

	// If the path is too complex to the OpenGL can afford, we use agg to deal with it.
	if (!ret) {
		CFX_AggDeviceDriver aggDriver(pThisLayer, 0, 0, 0, 0);
		aggDriver.DrawPath(pPathData, &matrix, pGraphState, fill_argb, stroke_argb, fill_mode, 0, NULL, 0);
	}
	
	// Finally, we have got the mask that we need, intersect with current clip region
	m_pClipRgn->IntersectMaskF(path_rect.left, path_rect.top, mask);
	
	return ret;
}

FX_BOOL	CFX_OpenGLDeviceDriver::SetClip_PathFill(const CFX_PathData* pPathData,
								 const CFX_AffineMatrix* pObject2Device,
								 int fill_mode
								 )
{
	if (m_pClipRgn == NULL)
		m_pClipRgn = FX_NEW CFX_ClipRgn(GetDeviceCaps(FXDC_PIXEL_WIDTH), GetDeviceCaps(FXDC_PIXEL_HEIGHT));

	if (pPathData->GetPointCount() == 5 || pPathData->GetPointCount() == 4) {
		CFX_FloatRect rectf;
		if (pPathData->IsRect(pObject2Device, &rectf)) {
			rectf.Intersect(CFX_FloatRect(0, 0, (FX_FIXFLOAT)GetDeviceCaps(FXDC_PIXEL_WIDTH), (FX_FIXFLOAT)GetDeviceCaps(FXDC_PIXEL_HEIGHT)));
			FX_RECT rect = rectf.GetOutterRect();
			m_pClipRgn->IntersectRect(rect);
			return TRUE;
		}
	}
	return GL_SetClipMask(pPathData, pObject2Device, NULL, 0xffffffff, 0, fill_mode);
}
FX_BOOL	CFX_OpenGLDeviceDriver::SetClip_PathStroke(const CFX_PathData* pPathData,
								   const CFX_AffineMatrix* pObject2Device,
								   const CFX_GraphStateData* pGraphState
								   )
{
	if (m_pClipRgn == NULL)
		m_pClipRgn = FX_NEW CFX_ClipRgn(GetDeviceCaps(FXDC_PIXEL_WIDTH), GetDeviceCaps(FXDC_PIXEL_HEIGHT));
	
	return GL_SetClipMask(pPathData, pObject2Device, pGraphState, 0, 0xffffffff, 0);
}
FX_BOOL CFX_OpenGLDeviceDriver::GetClipBox(FX_RECT* pRect)
{
	if (m_pClipRgn == NULL) {
		pRect->left = pRect->top = 0;
		pRect->right = GetDeviceCaps(FXDC_PIXEL_WIDTH);
		pRect->bottom = GetDeviceCaps(FXDC_PIXEL_HEIGHT);
		return TRUE;
	}
	*pRect = m_pClipRgn->GetBox();
	return TRUE;
}
FX_BOOL CFX_OpenGLDeviceDriver::FillRect(const FX_RECT* pRect, FX_DWORD fill_color, 
						int alpha_flag, void* pIccTransform, int blend_type)
{
	return FALSE;
}
FX_BOOL	CFX_OpenGLDeviceDriver::DrawCosmeticLine(FX_FIXFLOAT x1, FX_FIXFLOAT y1, FX_FIXFLOAT x2, FX_FIXFLOAT y2, FX_DWORD color,
						int alpha_flag, void* pIccTransform, int blend_type) 
{
	return FALSE; 
}
FX_BOOL	CFX_OpenGLDeviceDriver::DrawPath(const CFX_PathData* pPathData,
						 const CFX_AffineMatrix* pObject2Device,
						 const CFX_GraphStateData* pGraphState,
						 FX_DWORD fill_color,
						 FX_DWORD stroke_color,
						 int fill_mode,
						 int alpha_flag, 
						 void* pIccTransform,
						 int blend_type
						 )
{

	FOXIT_DEBUG1("Into CFX_OpenGLDeviceDriver::DrawPath");
	if (blend_type != FXDIB_BLEND_NORMAL) return FALSE;

	create_shaders();
	typedef struct {    
		float Position[3];    
		float Color[4];
	} Vertex; 
	const Vertex Vertices[] = {    
		{{1, -1, 0}, {1, 0, 0, 1}},    
		{{1, 1, 0}, {0, 1, 0, 1}},    
		{{-1, 1, 0}, {0, 0, 1, 1}},    
		{{-1, -1, 0}, {0, 0, 0, 1}}}; 
	const GLubyte Indices[] = {0, 1, 2, 2, 3, 0};
	GLuint vertexBuffer;    
	glGenBuffers(1, &vertexBuffer);    
	FOXIT_DEBUG1("Into CFX_OpenGLDeviceDriver::DrawPath:glGenBuffers");
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);    
	FOXIT_DEBUG1("Into CFX_OpenGLDeviceDriver::DrawPath:glBindBuffer");	
glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);  
FOXIT_DEBUG1("Into CFX_OpenGLDeviceDriver::DrawPath:glBufferData");   
	GLuint indexBuffer;    
	glGenBuffers(1, &indexBuffer);  
	FOXIT_DEBUG1("Into CFX_OpenGLDeviceDriver::DrawPath:glGenBuffers");    
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);    
FOXIT_DEBUG1("Into CFX_OpenGLDeviceDriver::DrawPath:glBindBuffer");  
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
FOXIT_DEBUG1("Into CFX_OpenGLDeviceDriver::DrawPath:glBufferData");  
	glVertexAttribPointer(_positionSlot, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0); 
FOXIT_DEBUG1("Into CFX_OpenGLDeviceDriver::DrawPath:glVertexAttribPointer");   
	glVertexAttribPointer(_colorSlot, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) (sizeof(float) *3));
FOXIT_DEBUG1("Into CFX_OpenGLDeviceDriver::DrawPath:glVertexAttribPointer");  
	glDrawElements(GL_TRIANGLES, sizeof(Indices)/sizeof(Indices[0]), GL_UNSIGNED_BYTE, 0);
FOXIT_DEBUG1("Into CFX_OpenGLDeviceDriver::DrawPath:glDrawElements");  
	FOXIT_DEBUG1("Out CFX_OpenGLDeviceDriver::DrawPath");
return TRUE;
	
}
FX_BOOL	CFX_OpenGLDeviceDriver::GetDIBits(CFX_DIBitmap* pBitmap, int left, int top, void* pIccTransform, FX_BOOL bDEdge)
{
	return FALSE;
}
FX_BOOL	CFX_OpenGLDeviceDriver::SetDIBits(const CFX_DIBSource* pBitmap, FX_DWORD color, const FX_RECT* pSrcRect, 
						  int dest_left, int dest_top, int blend_type, 
						  int alpha_flag, void* pIccTransform)
{
	return FALSE;
}

FX_BOOL	CFX_OpenGLDeviceDriver::StretchDIBits(const CFX_DIBSource* pBitmap, FX_DWORD color, int dest_left, int dest_top, 
							  int dest_width, int dest_height, const FX_RECT* pClipRect, FX_DWORD flags, 
							  int alpha_flag, void* pIccTransform, int blend_type)
{
	return FALSE;
}



#endif //_FX_OPENGL_SUPPORTED_
