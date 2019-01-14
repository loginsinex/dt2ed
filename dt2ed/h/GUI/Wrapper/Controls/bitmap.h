/*
    A NES Duck Tales 2 Level Editor [public version]
    Copyright (C) 2015 ALXR aka loginsin
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// CLASS:			CBitmap
// DESCRIPTION:		Bitmaps helper. Replaces some GDI API functions (because its works very ugly)
// REVISION:		Created Mar-24-2007 by ALXR

//	---------------------------------------------------------------------------------------------

#ifndef _DEBUG
#define DebugInfo(x, y)			(y)
#endif

#define ALPHA_COLOR(front,back,alpha)	(alpha*(front-back)/255+back)
#define XCHGCOLORREF(_x)              ((((_x)&0xFF00FF00L)|(((_x)&0x00FF0000L)>>16)|(((_x)&0x000000FFL)<<16)))

#define CBMP_ALPHA_RENDER				0
#define CBMP_TRANSPARENT				1
#define CBMP_ALPHA_CHANNEL_FROM_SRC		2

#define GRADIENT_FILL_RECT_D			0x100

#define ALIGN(p, s)						(p + ( (p%s) > 0 ? s-(p%s) : 0 ))

#define PTINSIZE(x, y, w, h)			( (x) >= 0 && (y) >= 0 && (x) < (w) && (y) < (h) )

typedef struct _WRGBQUAD
{
	BYTE rgbRed;
	BYTE rgbGreen;
	BYTE rgbBlue;
	BYTE rgbAlpha;
} WRGBQUAD, *LPWRGBQUAD;

class CBitmap;

class CDCBuffer
{
	BOOL			m_fCreated;
	struct
	{
		HDC			hDCScreen;
		HDC			hDC;
		HBITMAP		hBmp;
		HBITMAP		hOldBmp;
		SIZE		sz;
	} m_Buffer;

	BOOL			Initialize();

public:
	CDCBuffer();
	~CDCBuffer();
	CDCBuffer(INT x, INT y);
	BOOL			Create(INT x, INT y);
	BOOL			Resize(INT x, INT y);

	BOOL			BitBltSrc(HDC hDÑ, int x, int y, int cx, int cy, int x1, int y1, DWORD rop);
	BOOL			BitBltDst(int x, int y, int cx, int cy, HDC hDC, int x1, int y1, DWORD rop);
	HDC				Handle();
	HDC				SrcHandle();
	HBITMAP			Bitmap();
	BOOL			CreateBitmap(CBitmap & bm);
	operator HDC();
};

class CBitmap
{
	DWORD				*m_pvBits;		// bitmap bits
	INT					m_szX, m_szY;	// bitmap size
	UINT				m_uSize;		// bitmap size in byts
	mutable CRITICAL_SECTION	m_lpcrs;		// critical section for reallocations
	BOOL				m_bLocked;		// bitmap bits are locked

	HDC					m_hdesktopDC;	// hDC of desktop
	HDC					m_hbufferDC;	// device context in memory
	HBITMAP				m_hbufferBm;	// bitmap for text render
	HBITMAP				m_holdBm;		// previous bitmap from m_hbufferDC

	void				InitBuffer();

#ifdef _DEBUG
	void				DebugInfo(LPSTR pszFuncName, UINT funcRet);
#endif

public:
	HBRUSH				CreateBrush();
	BOOL				LoadBitmap(HINSTANCE hInstance, LPCTSTR lpBitmapName);
	BOOL				LoadIcon(HICON hIcon);
	BOOL				Create(INT x, INT y);
	BOOL				Resize(INT x, INT y);
	VOID				Destroy();
	BOOL				RenderToDevice(HDC hDC, UINT xDest, UINT yDest, UINT cxDest, UINT cyDest, UINT xSrc, UINT ySrc);
	HBITMAP				ConvertToHandle(INT y = 0);
	VOID				GradientFill(INT x, INT y, INT cx, INT cy, COLORREF crColor1, COLORREF crColor2, DWORD dwFlags);
	VOID				GradientFillEx(INT x, INT y, INT cx, INT cy, COLORREF crColor1, COLORREF crColor2, BYTE bAlpha1, BYTE bAlpha2, DWORD dwFlags);
	VOID				RenderRawBitmap(CONST WRGBQUAD* pvBits, INT src_width, INT src_height, BYTE bAlphaConstant, BOOL bUseSourceAlphaChannel, INT x, INT y, INT cx, INT cy, INT dest_x, INT dest_y, DWORD dwFlags);
	VOID				CopyRawRect(CONST WRGBQUAD * pvBits, INT src_cx, INT src_cy, INT x, INT y, INT cx, INT cy, INT dest_x, INT dest_y);
	VOID				RenderBitmap(HBITMAP hSourceBmp, BYTE bAlphaConstant, INT x, INT y, INT cx, INT cy, INT dest_x, INT dest_y, COLORREF crTransparent, DWORD dwFlags);
	VOID				RenderRawTransparentBitmap(CONST WRGBQUAD *pvBits, INT src_width, INT src_height, INT x, INT y, INT cx, INT cy, INT dest_x, INT dest_y, COLORREF crTransparent, DWORD dwFlags);
	VOID				FillRect(INT x, INT y, INT cx, INT cy, COLORREF crColor, COLORREF crBorder, INT iWidth, BYTE fInnerAlpha = 255);
	VOID				FillRectTransparentBits(WRGBQUAD * pvBits, INT src_width, INT src_height, INT chunk_X, INT chunk_Y, INT chunk_CX, INT chunk_CY, INT start_X, INT start_Y, BOOL bVertical, INT count, COLORREF crTransparent);
	VOID				FillRectBits(WRGBQUAD * pvBits, INT src_width, INT src_height, INT chunk_X, INT chunk_Y, INT chunk_CX, INT chunk_CY, INT start_X, INT start_Y, BOOL bVertical, INT count);
	VOID				CopyRect(HBITMAP hSrcBitmap, INT x, INT y, INT cx, INT cy, INT dest_x, INT dest_y);
	VOID				RenderText(INT x, INT y, INT cx, INT cy, LPCTSTR pszText, HFONT hFont, BOOL bTransparent, COLORREF crTextColor, COLORREF crBackColor, DWORD dwFlags);
	VOID				Copy(CBitmap * pSource);
	VOID				CreateFromBitmap(HBITMAP hBmp);
	HDC					LockDC();
	void				FreeDC();
	INT					Width()
	{
		return m_szX;
	}

	INT					Height()
	{
		return m_szY;
	}

	WRGBQUAD *			LockRawBits();

	VOID				UnlockRawBits()
	{
		//if (!m_uSize) return;
		//m_bLocked = false;
		//LocalUnlock(m_pvBits);
	}

	CBitmap();

	~CBitmap();
	std::map<int,HGDIOBJ>	m_vObjects;

};
