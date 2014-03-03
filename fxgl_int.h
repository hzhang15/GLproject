#ifndef _FX_GE_OPENGLES_DRIVER_H_
#define _FX_GE_OPENGLES_DRIVER_H_

class COpenglEsNvExt
{
public:
	COpenglEsNvExt();
	~COpenglEsNvExt();
	void			Load();
	FX_BOOL			IsAvailable() { return m_bAvailable; }
	FX_BOOL			StretchBitMask(const CFX_DIBitmap* pBitmap, int dest_left, int dest_top, 
		int dest_width, int dest_height, FX_DWORD argb, const FX_RECT* pClipRect);
	FX_BOOL			StretchDIBits(const CFX_DIBitmap* pBitmap, int dest_left, int dest_top,
		int dest_width, int dest_height, const FX_RECT* pClipRect);

	FX_BOOL			DrawPath(const CFX_PathData* pPathData,
		const CFX_AffineMatrix* pObject2Device,
		const CFX_GraphStateData* pGraphState,
		FX_DWORD fill_argb,
		FX_DWORD stroke_argb,
		int fill_mode
		);
	// Function points for all used nv external functions.
	void*			m_Functions[100];
protected:
	FX_BOOL m_bAvailable;

};

class COpenglEsPlatform : public CFX_Object
{
public:
	COpenglEsNvExt	m_OpenglEsNvExt;
	EGLDisplay		m_hEglDisplay;
	EGLConfig		m_hEglConfig;
	int				m_nMaxMatchConfigs;
};


class CFX_OpenGLDeviceDriver: public IFX_RenderDeviceDriver
{
public:
	CFX_OpenGLDeviceDriver();
	FX_BOOL Create(CFX_DIBitmap* pBitmap, int dither_bits, FX_BOOL bRgbByteOrder, CFX_DIBitmap* pOriDevice, FX_BOOL bGroupKnockout);
	FX_BOOL Create(EGLNativeWindowType pWindowHandle);
	virtual ~CFX_OpenGLDeviceDriver();
public:
	virtual int		GetDeviceCaps(int caps_id);
	virtual void	SaveState();
	virtual void	RestoreState(FX_BOOL bKeepSaved = FALSE);
	virtual FX_BOOL	SetClip_PathFill(const CFX_PathData* pPathData,
						const CFX_AffineMatrix* pObject2Device,
						int fill_mode
						);
	virtual FX_BOOL	SetClip_PathStroke(const CFX_PathData* pPathData,
						const CFX_AffineMatrix* pObject2Device,
						const CFX_GraphStateData* pGraphState
						);
	virtual FX_BOOL	DrawPath(const CFX_PathData* pPathData,
						const CFX_AffineMatrix* pObject2Device,
						const CFX_GraphStateData* pGraphState,
						FX_DWORD fill_color,
						FX_DWORD stroke_color,
						int fill_mode,
						int alpha_flag = 0, 
						void* pIccTransform = NULL,
						int blend_type = FXDIB_BLEND_NORMAL
						);
	virtual FX_BOOL	SetPixel(int x, int y, FX_DWORD color,
						int alpha_flag = 0, void* pIccTransform = NULL) { return FALSE; }
	virtual FX_BOOL FillRect(const FX_RECT* pRect, FX_DWORD fill_color, 
						int alpha_flag = 0, void* pIccTransform = NULL, int blend_type = FXDIB_BLEND_NORMAL);
	virtual FX_BOOL	DrawCosmeticLine(FX_FIXFLOAT x1, FX_FIXFLOAT y1, FX_FIXFLOAT x2, FX_FIXFLOAT y2, FX_DWORD color,
						int alpha_flag = 0, void* pIccTransform = NULL, int blend_type = FXDIB_BLEND_NORMAL);
	virtual FX_BOOL GetClipBox(FX_RECT* pRect);
	virtual CFX_DIBitmap*   GetBackDrop() { return m_pOriDevice; }
	virtual FX_BOOL	GetDIBits(CFX_DIBitmap* pBitmap, int left, int top, void* pIccTransform = NULL, FX_BOOL bDEdge = FALSE);

	virtual FX_BOOL	SetDIBits(const CFX_DIBSource* pBitmap, FX_DWORD color, const FX_RECT* pSrcRect, 
						int dest_left, int dest_top, int blend_type, 
						int alpha_flag = 0, void* pIccTransform = NULL);

	virtual FX_BOOL	StretchDIBits(const CFX_DIBSource* pBitmap, FX_DWORD color, int dest_left, int dest_top, 
				int dest_width, int dest_height, const FX_RECT* pClipRect, FX_DWORD flags, 
				int alpha_flag = 0, void* pIccTransform = NULL, int blend_type = FXDIB_BLEND_NORMAL);

	virtual FX_BOOL	StartDIBits(const CFX_DIBSource* pBitmap, int bitmap_alpha, FX_DWORD color, 
				const CFX_AffineMatrix* pMatrix, FX_DWORD flags, FX_LPVOID& handle, 
				int alpha_flag = 0, void* pIccTransform = NULL, int blend_type = FXDIB_BLEND_NORMAL) { return FALSE; }
	virtual FX_BOOL	ContinueDIBits(FX_LPVOID handle, IFX_Pause* pPause) { return FALSE; }
	virtual void	CancelDIBits(FX_LPVOID handle) {}
	virtual FX_BOOL DrawDeviceText(int nChars, const FXTEXT_CHARPOS* pCharPos, CFX_Font* pFont,
						CFX_FontCache* pCache, const CFX_AffineMatrix* pObject2Device, FX_FIXFLOAT font_size, FX_DWORD color, 
						int alpha_flag = 0, void* pIccTransform = NULL) 
	{ return FALSE; }
	virtual void*	GetPlatformSurface() {return NULL;}
protected:
	FX_BOOL			GL_SetClipMask(const CFX_PathData* pPathData, 
						const CFX_AffineMatrix* pObject2Device, 
						const CFX_GraphStateData* pGraphState, 
						FX_ARGB fill_argb, FX_ARGB stroke_argb, 
						int fill_mode);
private:
	// For window handle.		
	EGLDisplay			m_hEglDisplay;
	EGLConfig			m_hEglConfig;
	EGLSurface			m_hCurEglSurface;
	EGLSurface			m_hOldEglSurface;
	EGLContext			m_hCurEglContext;
	EGLContext			m_hOldEglContext;
	int					m_Width, m_Height, m_nBitsPerPixel;
	int					m_DeviceClass, m_RenderCaps;

	// For bitmap.
	EGLNativePixmapType m_pNativePixmap;
	CFX_DIBitmap*		m_pBitmap;
	CFX_PtrArray		m_StateStack;
	int					m_DitherBits;
	FX_BOOL				m_bRgbByteOrder;
	
	// Clip region
	CFX_ClipRgn*		m_pClipRgn;

	
	CFX_DIBitmap*       m_pOriDevice;
	FX_BOOL             m_bGroupKnockout;

	
};

#endif