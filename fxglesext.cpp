#ifdef _FX_OPENGLES_SUPPORTED_
#include "../../../include/fxge/fx_ge.h"
#include "./include/GLES2/gl2.h"
//#define _USE_EXTERNAL_NV_
#ifdef _USE_EXTERNAL_NV_
#include "./include/GLES2/gl2extnv.h"
#endif
#include "./include/EGL/egl.h"
#include "./fxgl_int.h"

enum {
	FuncId_glFramebufferTextureLayerEXT,
	FuncId_glCoverageMaskNV,
	FuncId_glCoverageOperationNV,
	FuncId_glCreatePathNV,
	FuncId_glDeletePathNV,
	FuncId_glPathVerticesNV,
	FuncId_glPathParameterfNV,
	FuncId_glPathParameteriNV,
	FuncId_glCreatePathProgramNV,
	FuncId_glPathMatrixNV,
	FuncId_glDrawPathNV,
	FuncId_glCreatePathbufferNV,
	FuncId_glDeletePathbufferNV,
	FuncId_glPathbufferPathNV,
	FuncId_glPathbufferPositionNV,
	FuncId_glDrawPathbufferNV,
	FuncId_glGetTexImageNV,
	FuncId_glGetCompressedTexImageNV,
	FuncId_glGetTexLevelParameterfvNV,
	FuncId_glGetTexLevelParameterivNV,
	FuncId_glFramebufferVertexAttribArrayNV,
	FuncId_glDrawBuffersARB,
	FuncId_glReadBufferNV,
};

// List all exported nv external function names we use here:
static FX_LPCSTR g_OpenglEs2NvFuncNames[] = {
	"glFramebufferTextureLayerEXT",
	"glCoverageMaskNV",
	"glCoverageOperationNV",
	"glCreatePathNV",
	"glDeletePathNV",
	"glPathVerticesNV",
	"glPathParameterfNV",
	"glPathParameteriNV",
	"glCreatePathProgramNV",
	"glPathMatrixNV",
	"glDrawPathNV",
	"glCreatePathbufferNV",
	"glDeletePathbufferNV",
	"glPathbufferPathNV",
	"glPathbufferPositionNV",
	"glDrawPathbufferNV",
	"glGetTexImageNV",
	"glGetCompressedTexImageNV",
	"glGetTexLevelParameterfvNV",
	"glGetTexLevelParameterivNV",
	"glFramebufferVertexAttribArrayNV",
	"glDrawBuffersARB",
	"glReadBufferNV",
};
// List all nv external prototypes
typedef void (GL_APIENTRY FuncType_glFramebufferTextureLayerEXT) (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer); 
typedef void (GL_APIENTRY FuncType_glCoverageMaskNV) (GLboolean mask); 
typedef void (GL_APIENTRY FuncType_glCoverageOperationNV) (GLenum operation);
typedef GLuint (GL_APIENTRY FuncType_glCreatePathNV) (GLenum datatype, GLsizei numCommands, const GLubyte* commands); 
typedef void (GL_APIENTRY FuncType_glDeletePathNV) (GLuint path); 
typedef void (GL_APIENTRY FuncType_glPathVerticesNV) (GLuint path, const void* vertices); 
typedef void (GL_APIENTRY FuncType_glPathParameterfNV) (GLuint path, GLenum paramType, GLfloat param); 
typedef void (GL_APIENTRY FuncType_glPathParameteriNV) (GLuint path, GLenum paramType, GLint param); 
typedef GLuint (GL_APIENTRY FuncType_glCreatePathProgramNV) (void); 
typedef void (GL_APIENTRY FuncType_glPathMatrixNV) (GLenum target, const GLfloat* value); 
typedef void (GL_APIENTRY FuncType_glDrawPathNV) (GLuint path, GLenum mode); 
typedef GLuint (GL_APIENTRY FuncType_glCreatePathbufferNV) (GLsizei capacity); 
typedef void (GL_APIENTRY FuncType_glDeletePathbufferNV) (GLuint buffer); 
typedef void (GL_APIENTRY FuncType_glPathbufferPathNV) (GLuint buffer, GLint index, GLuint path); 
typedef void (GL_APIENTRY FuncType_glPathbufferPositionNV) (GLuint buffer, GLint index, GLfloat x, GLfloat y); 
typedef void (GL_APIENTRY FuncType_glDrawPathbufferNV) (GLuint buffer, GLenum mode); 
typedef void (GL_APIENTRY FuncType_glGetTexImageNV) (GLenum target, GLint level, GLenum format, GLenum type, GLvoid* img); 
typedef void (GL_APIENTRY FuncType_glGetCompressedTexImageNV) (GLenum target, GLint level, GLvoid* img); 
typedef void (GL_APIENTRY FuncType_glGetTexLevelParameterfvNV) (GLenum target, GLint level, GLenum pname, GLfloat* params); 
typedef void (GL_APIENTRY FuncType_glGetTexLevelParameterivNV) (GLenum target, GLint level, GLenum pname, GLint* params); 
typedef void (GL_APIENTRY FuncType_glFramebufferVertexAttribArrayNV) (GLenum target, GLenum attachment, GLenum buffertarget, GLuint bufferobject, GLint size, GLenum type, GLboolean normalized, GLintptr offset, GLsizeiptr width, GLsizeiptr height, GLsizei stride); 
typedef void (GL_APIENTRY FuncType_glDrawBuffersARB) (GLsizei n, const GLenum *bufs); 
typedef void (GL_APIENTRY FuncType_glReadBufferNV) (GLenum mode);

#ifdef GL_GLEXT_PROTOTYPES
#define CallFunc(funcname) funcname
#else
#define CallFunc(funcname) ((FuncType_##funcname)OpenglEsNvExt.m_Functions[FuncId_##funcname])
#endif

COpenglEsNvExt::COpenglEsNvExt()
{
	m_bAvailable = FALSE;
	for (int i = 0; i < sizeof g_OpenglEs2NvFuncNames / sizeof(FX_LPCSTR); i ++)
		m_Functions[i] = NULL;
}
COpenglEsNvExt::~COpenglEsNvExt()
{
	// Do nothing here, let the system handles all the shutdown properly
}
void COpenglEsNvExt::Load()
{
#ifdef _USE_EXTERNAL_NV_
#ifdef GL_GLEXT_PROTOTYPES
	// Load all used  nv external functions from the DLL/SO
	for (int i = 0; i < sizeof g_OpenglEs2NvFuncNames / sizeof(FX_LPCSTR); i ++) {
		m_Functions[i] = eglGetProcAddress(g_OpenglEs2NvFuncNames[i]);
		if (m_Functions[i] == NULL) {
			m_bAvailable = FALSE;
			return;
		}
	}
	m_bAvailable = TRUE;
#endif
#endif
}
FX_BOOL	COpenglEsNvExt::StretchBitMask(const CFX_DIBitmap* pBitmap, int dest_left, int dest_top, 
							   int dest_width, int dest_height, FX_DWORD argb, const FX_RECT* pClipRect)
{
	return FALSE;
}
FX_BOOL	COpenglEsNvExt::StretchDIBits(const CFX_DIBitmap* pBitmap, int dest_left, int dest_top,
							  int dest_width, int dest_height, const FX_RECT* pClipRect)
{
	return FALSE;
}
FX_BOOL	COpenglEsNvExt::DrawPath(const CFX_PathData* pPathData,
						 const CFX_AffineMatrix* pObject2Device,
						 const CFX_GraphStateData* pGraphState,
						 FX_DWORD fill_argb,
						 FX_DWORD stroke_argb,
						 int fill_mode
		)
{
	return FALSE;
}
#endif //_FX_OPENGLES_SUPPORTED_
