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

#include "precomp.h"

#define __EnterCriticalSection(x)		{ (x); }
#define __LeaveCriticalSection(x)		{ (x); }

#ifdef _DEBUG
#define PTHIS			//printf("%s:%d->this = 0x%08x;\n", __FUNCTION__, __LINE__, this)
#else
#define PTHIS
#endif

// CLASS:			CDCBuffer
// DESCRIPTION:		Memory DC buffer.
// REVISION:		Created Jan-29-2014 by ALXR

//	---------------------------------------------------------------------------------------------

CDCBuffer::CDCBuffer()
{
	ZeroMemory(this, sizeof(*this));
	if ( !Initialize() )
		throw TEXT("Failed to initialize buffer");
}

CDCBuffer::CDCBuffer(INT x, INT y)
{
	ZeroMemory(this, sizeof(*this));
	if ( !Initialize() && !Create(x, y) )
		throw TEXT("Failed to create buffer");
}

// FUNCTION:		Initialize			
// DESCRIPTION:		Initializes a device contexts for future use
// ARGUMENTS:	
//					(none)
// REVISION:		
//					Created Jan-29-2014 by ALXR

BOOL CDCBuffer::Initialize()
{
	m_Buffer.hDCScreen = GetDC(GetDesktopWindow());
	if ( !m_Buffer.hDCScreen )
		return FALSE;

	m_Buffer.hDC = CreateCompatibleDC(m_Buffer.hDCScreen);
	if ( !m_Buffer.hDC )
	{
		ReleaseDC(GetDesktopWindow(), m_Buffer.hDCScreen);
		return FALSE;
	}
	return TRUE;
}

CDCBuffer::~CDCBuffer()
{
	m_fCreated = FALSE;
	if ( m_Buffer.hBmp )
	{
		SelectObject(m_Buffer.hDC, m_Buffer.hOldBmp);
		DeleteObject(m_Buffer.hBmp);
		m_Buffer.hBmp = m_Buffer.hOldBmp = NULL;
	}

	DeleteDC(m_Buffer.hDC);
	ReleaseDC(GetDesktopWindow(), m_Buffer.hDCScreen);
}

void CBitmap::InitBuffer()
{
	if ( !m_hdesktopDC && !m_hbufferDC )
	{
		m_hdesktopDC = GetDC(NULL);
		m_hbufferDC = CreateCompatibleDC(m_hdesktopDC);

		// remember default DC objects
		for(int i = OBJ_PEN; i <= OBJ_COLORSPACE; ++i)
			m_vObjects[i] = GetCurrentObject(m_hbufferDC, i);
	}
}

HBRUSH CBitmap::CreateBrush()
{
	return CreatePatternBrush(m_hbufferBm);
}

// FUNCTION:		Create
// DESCRIPTION:		Creates or resizes bitmap buffer
// ARGUMENTS:	
//					x - width of buffer
//					y - height of buffer
// REVISION:		
//					Created Jan-29-2014 by ALXR

BOOL CDCBuffer::Create(INT x, INT y)
{
	m_fCreated = FALSE;
	if ( m_Buffer.hBmp )
	{
		SelectObject(m_Buffer.hDC, m_Buffer.hOldBmp);
		DeleteObject(m_Buffer.hBmp);
		m_Buffer.hBmp = m_Buffer.hOldBmp = NULL;
	}

	m_Buffer.hBmp = CreateCompatibleBitmap(m_Buffer.hDCScreen, x, y);
	if ( !m_Buffer.hBmp )
		return FALSE;

	m_Buffer.hOldBmp = (HBITMAP) SelectObject(m_Buffer.hDC, m_Buffer.hBmp);
	if ( !m_Buffer.hOldBmp )
	{
		DeleteObject(m_Buffer.hBmp);
		return FALSE;
	}
	m_Buffer.sz.cx = x;
	m_Buffer.sz.cy = y;
	m_fCreated = TRUE;
	return TRUE;
}

// FUNCTION:		Resize
// DESCRIPTION:		Just wrapper for create
// ARGUMENTS:	
//					x - new width of buffer
//					y - new height of buffer
// REVISION:		
//					Created Jan-29-2014 by ALXR

BOOL CDCBuffer::Resize(INT x, INT y)
{
	if ( x != m_Buffer.sz.cx || y != m_Buffer.sz.cy )
		return Create(x, y);

	return TRUE;
}

// FUNCTION:		BitBltSrc
// DESCRIPTION:		Blits bits from buffer to user device
// ARGUMENTS:	
//					(Same as BitBlt API, but hDCSrc)
// REVISION:		
//					Created Jan-29-2014 by ALXR

BOOL CDCBuffer::BitBltSrc(HDC hDC, int x, int y, int cx, int cy, int x1, int y1, DWORD rop)
{
	if ( !m_fCreated ) 
		return FALSE;

	return BitBlt(hDC, x, y, cx, cy, m_Buffer.hDC, x1, y1, rop);
}

// FUNCTION:		BitBltDst
// DESCRIPTION:		Blits bits from user device to buffer
// ARGUMENTS:	
//					(Same as BitBlt API, but hDCDst)
// REVISION:		
//					Created Jan-29-2014 by ALXR

BOOL CDCBuffer::BitBltDst(int x, int y, int cx, int cy, HDC hDC, int x1, int y1, DWORD rop)
{
	if ( !m_fCreated ) 
		return FALSE;

	return BitBlt(m_Buffer.hDC, x, y, cx, cy, hDC, x1, y1, rop);
}

// FUNCTION:		Handle
// DESCRIPTION:		returns buffer device context handle
// ARGUMENTS:	
//					(none)
// REVISION:		
//					Created Jan-29-2014 by ALXR

HDC CDCBuffer::Handle()
{
	if ( !m_fCreated )
		return NULL;

	return m_Buffer.hDC;
}

CDCBuffer::operator HDC()
{
	if ( !m_fCreated )
		return NULL;

	return m_Buffer.hDC;
}

// FUNCTION:		Bitmap
// DESCRIPTION:		returns buffer bitmap handle
// ARGUMENTS:	
//					(none)
// REVISION:		
//					Created Jan-29-2014 by ALXR

HBITMAP CDCBuffer::Bitmap()
{
	if ( !m_fCreated )
		return NULL;

	return m_Buffer.hBmp;
}

// FUNCTION:		CreateBitmap
// DESCRIPTION:		Creates a bitmap from buffer
// ARGUMENTS:	
//					bm - user bitmap
// REVISION:		
//					Created Jan-29-2014 by ALXR

BOOL CDCBuffer::CreateBitmap(CBitmap & bm)
{
	if ( !m_fCreated )
		return FALSE;

	bm.CreateFromBitmap(m_Buffer.hBmp);
	return TRUE;
}

// FUNCTION:		SrcHandle
// DESCRIPTION:		Returns a screen device context
// ARGUMENTS:	
//					(none)
// REVISION:		
//					Created Jan-31-2014 by ALXR

HDC CDCBuffer::SrcHandle()
{
	if ( !m_fCreated )
		return FALSE;

	return m_Buffer.hDCScreen;
}


// CLASS:			CBitmap
// DESCRIPTION:		Bitmaps helper. Replaces some GDI API functions (because its works very ugly)
// REVISION:		Created Mar-24-2007 by ALXR

//	---------------------------------------------------------------------------------------------

CBitmap::CBitmap()
	: m_hdesktopDC( NULL ), m_hbufferDC( NULL ), m_hbufferBm( NULL ), m_holdBm( NULL ),
	m_szX( 0 ), m_szY( 0 ), m_pvBits( NULL ), m_uSize( 0 ), m_bLocked( FALSE )
{
	InitializeCriticalSection(&m_lpcrs);
}

CBitmap::~CBitmap()
{
	Destroy();
	m_pvBits = NULL;

	for(int i = OBJ_PEN; i <= OBJ_COLORSPACE; ++i)
		if ( m_vObjects[i] )
			SelectObject(m_hbufferDC, m_vObjects[i]);

	if ( m_hbufferBm )
		DeleteObject( m_hbufferBm );

	if ( m_hbufferDC )
		DeleteDC(m_hbufferDC);

	if ( m_hdesktopDC )
		ReleaseDC(NULL, m_hdesktopDC);
		
	DeleteCriticalSection(&m_lpcrs);
}



// FUNCTION:		Create			
// DESCRIPTION:		Ñreates bitmap image in memory
// ARGUMENTS:
//					x			- width of bitmap
//					y			- height of bitmap
// REVISION:		
//					Created Mar-24-2007 by ALXR

BOOL CBitmap::Create(INT x, INT y)
{
	BOOL lResult = false;
	
	InitBuffer();

	PTHIS;

	if ( m_uSize )
		Destroy();

	if (x > 0 && y > 0 && !m_uSize)
	{
		__EnterCriticalSection(&m_lpcrs);
		m_uSize = sizeof(WRGBQUAD)*x*y;
		
		BITMAPINFO bmi = { 0 };
		bmi.bmiHeader.biSize = sizeof(bmi);
		bmi.bmiHeader.biWidth = x;
		bmi.bmiHeader.biHeight = -y;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = m_uSize;

		m_hbufferBm = CreateDIBSection( m_hbufferDC, &bmi, DIB_RGB_COLORS, (PVOID*) &m_pvBits, NULL, 0 );

		lResult = ( NULL != m_hbufferBm );

		if ( lResult )
		{
			m_szX = x;
			m_szY = y;
		}

		__LeaveCriticalSection(&m_lpcrs);
	}
	return lResult;
}

VOID CBitmap::Destroy()
{
	FreeDC();
	if (m_uSize)
	{
		// LocalFree(m_pvBits);
		m_uSize = 0;
		m_szX = m_szY = 0;
		m_pvBits = NULL;
		DeleteObject( m_hbufferBm );
	}
}

BOOL CBitmap::LoadBitmap(HINSTANCE hInstance, LPCTSTR lpBitmapName)
{
	HRSRC hRes = FindResource( hInstance, lpBitmapName, (LPCTSTR) RT_BITMAP);
	if ( hRes )
	{
		DWORD uSize = SizeofResource( hInstance, hRes );
		HGLOBAL hData = LoadResource( hInstance, hRes );
		if ( hData )
		{
			PVOID pvBits = LockResource( hData );
			PVOID pvBmpBits = PBYTE( pvBits ) + 40;
			INT x = 0, y = 0, bppxl = 0;
			x			= ((LPDWORD) pvBits)[1];
			y			= ((LPDWORD) pvBits)[2];
			bppxl		= ((USHORT*) pvBits)[7];


			if ( 32 == bppxl )
			{
				Create( x, y );

				for(int i = 0; i < y; ++i)
					CopyMemory(&((WRGBQUAD*) m_pvBits)[i*x], &((WRGBQUAD*) pvBmpBits)[(y - i - 1)*x], x * sizeof(WRGBQUAD));

				return ( m_uSize > 0 );
			}
			else
			{
				HBITMAP hBmp = ::LoadBitmap(hInstance, lpBitmapName);
				if ( hBmp )
				{
					CreateFromBitmap( hBmp );
					DeleteObject( hBmp );
					return ( m_uSize > 0 );
				}
			}
		}
	}

	return FALSE;
}


// FUNCTION:		Resize
// DESCRIPTION:		Resizes bitmap image in memory
// ARGUMENTS:
//					x			- new width of bitmap
//					y			- new height of bitmap
// REVISION:		
//					Created Mar-24-2007 by ALXR

BOOL CBitmap::Resize(INT x, INT y)
{
	return Create( x, y );
}


// FUNCTION:		GradientFill
// DESCRIPTION:		Fills gradient bitmap
// ARGUMENTS:
//					x,y			- upper-left conner
//					cx,cy		- width and height of gradient area
//					crColor1	- upper-left conner color
//					crColor2	- right-down conner color
//					dwFlags		- Indicates vertical or horizontal gradient
// REVISION:		
//					Created Mar-24-2007 by ALXR

VOID CBitmap::GradientFill(INT x, INT y, INT cx, INT cy, COLORREF crColor1, COLORREF crColor2, DWORD dwFlags)
{
	if (!m_uSize || m_bLocked) return;		// return if bitmap was not created
	GradientFillEx(x, y, cx, cy, crColor1, crColor2, 255, 255, dwFlags);
}

VOID CBitmap::GradientFillEx(INT x, INT y, INT cx, INT cy, COLORREF crColor1, COLORREF crColor2, BYTE bAlpha1, BYTE bAlpha2, DWORD dwFlags)
{
	if (!m_uSize || m_bLocked) return;		// return if bitmap was not created

	TRIVERTEX tvi[2] = { 0 };
	GRADIENT_RECT grc = { 0 };
	tvi[0].Alpha = WORD(bAlpha1) << 8;
	tvi[0].Red = GetRValue(crColor1) << 8;
	tvi[0].Green = GetGValue(crColor1) << 8;
	tvi[0].Blue = GetBValue(crColor1) << 8;
	tvi[0].x = x;
	tvi[0].y = y;
	
	tvi[1].Alpha = WORD(bAlpha1) << 8;
	tvi[1].Red = GetRValue(crColor2) << 8;
	tvi[1].Green = GetGValue(crColor2) << 8;
	tvi[1].Blue = GetBValue(crColor2) << 8;
	tvi[1].x = x + cx;
	tvi[1].y = y + cy;

	grc.UpperLeft = 0; grc.LowerRight = 1;
	::GdiGradientFill(LockDC(), tvi, 2, &grc, 1, dwFlags);
	FreeDC();
}



BOOL CBitmap::LoadIcon(HICON hIcon)
{
	ICONINFO pi = { 0 };
	if ( GetIconInfo(hIcon, &pi) )
	{
		CreateFromBitmap( pi.hbmColor );
			
		DeleteObject( pi.hbmColor );
		DeleteObject( pi.hbmMask );

		return TRUE;
	}

	return FALSE;
}

VOID CBitmap::CreateFromBitmap(HBITMAP hBmp)
{
	BOOL fResult = FALSE;
	Destroy();
	BITMAP bmp = { 0 };
	if ( GetObject(hBmp, sizeof(bmp), &bmp) )
	{
		HDC hDC = CreateCompatibleDC( m_hdesktopDC );
		HGDIOBJ hbmOld = SelectObject( hDC, hBmp );
		Create( bmp.bmWidth, bmp.bmHeight );

		struct
		{
			BITMAPINFOHEADER bmiHeader;
			COLORREF pal[256];
		} bmi = { 0 };

		// BITMAPINFO bmi = { 0 };
		bmi.bmiHeader.biSize = sizeof(bmi);
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biWidth = bmp.bmWidth;
		bmi.bmiHeader.biHeight = -bmp.bmHeight;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = m_uSize;

		fResult = ( 0 != GetDIBits( hDC, hBmp, 0, bmp.bmHeight, m_pvBits, PBITMAPINFO(&bmi), DIB_RGB_COLORS ) );

		SelectObject( hDC, hbmOld );
		// DeleteObject( hBmp );
		DeleteDC( hDC );
	}
}

// FUNCTION:		ConvertToHandle
// DESCRIPTION:		Creates a GDI bitmap handle. Its MUST BE destroyed after using
// ARGUMENTS:
// REVISION:		
//					Created Mar-24-2007 by ALXR

HBITMAP CBitmap::ConvertToHandle(INT y)
{
	if (!m_uSize || m_bLocked) return FALSE;		// return if bitmap was not created	

	if (!y) y = m_szY;
	if (y > m_szY) y = m_szY;

	ULONG uSize = m_szX * y * sizeof(WRGBQUAD);
	BITMAPINFO bmpDest = { 0 };
	bmpDest.bmiHeader.biSize = sizeof(bmpDest); // bmiSize;
	bmpDest.bmiHeader.biWidth = m_szX;
	bmpDest.bmiHeader.biHeight = -y;
	bmpDest.bmiHeader.biPlanes = 1;
	bmpDest.bmiHeader.biBitCount = 32;
	bmpDest.bmiHeader.biCompression = BI_RGB;
	bmpDest.bmiHeader.biSizeImage = uSize;
	PVOID pvBits = NULL;

	HBITMAP hRetBitmap = CreateDIBSection( m_hbufferDC, &bmpDest, DIB_RGB_COLORS, &pvBits, NULL, 0 );
	if ( pvBits )
		CopyMemory(pvBits, m_pvBits, uSize);
	
	return hRetBitmap;
}


// FUNCTION:		RenderRawBitmap
// DESCRIPTION:		Renders bitmap with alpha channels to our bitmap
// ARGUMENTS:
//					pvBits		- bits of source bitmap
//					src_cx		- width of source bitmap
//					src_cy		- height of source bitmap
//					bAlpha		- transparency level
//					bUseSourceAlpha	- if FALSE then alpha channel of source bitmap will be ignored
//					x, y		- upper-left conner of source bitmap target area
//					cx, cy		- width/height of source bitmap target area
//					dest_x, dest_y - upper-left conner of our bitmap
//					dwFlags		- reserved
//
// REVISION:		
//					Created Mar-24-2007 by ALXR

VOID CBitmap::RenderRawBitmap(CONST WRGBQUAD* pvBits, INT src_cx, INT src_cy, BYTE bAlphaConstant, BOOL bUseSourceAlphaChannel, INT x, INT y, INT cx, INT cy, INT dest_x, INT dest_y, DWORD dwFlags)
{
	if (!m_uSize || !bAlphaConstant || m_bLocked) return;		// return if bitmap was not created

	INT sx = dest_x, sy = dest_y, iright = sx+cx, ibottom = sy+cy, c = 0, c_src = 0;
	if (x < 0 || y < 0 || cx < 0 || cy < 0 || dest_x < 0 || dest_y < 0 || sx+cx > m_szX || sy + cy > m_szY || !cx || !cy) return;

	__EnterCriticalSection(&m_lpcrs);
	WRGBQUAD * pvMyBits = (WRGBQUAD*) m_pvBits; // LocalLock(m_pvBits);
	if (bUseSourceAlphaChannel)
	{
		if (bAlphaConstant == 255)
		{
			for(sy=dest_y;sy<ibottom;sy++)
			{
				c = sy*m_szX + dest_x;
				c_src = y*src_cx + x;
				for(sx=dest_x;sx<iright;sx++)
				{
					pvMyBits[c].rgbRed = ALPHA_COLOR(pvBits[c_src].rgbRed, pvMyBits[c].rgbRed, pvBits[c_src].rgbAlpha);
					pvMyBits[c].rgbGreen = ALPHA_COLOR(pvBits[c_src].rgbGreen, pvMyBits[c].rgbGreen, pvBits[c_src].rgbAlpha);
					pvMyBits[c].rgbBlue = ALPHA_COLOR(pvBits[c_src].rgbBlue, pvMyBits[c].rgbBlue, pvBits[c_src].rgbAlpha);
					++c;
					++c_src;
				}
				++y;
			}
		}	
		else
		{
			WRGBQUAD rgb;
			// bAlphaConstant = 255-bAlphaConstant;
			for(sy=dest_y;sy<ibottom;sy++)
			{
				c = sy*m_szX + dest_x;
				c_src = y*src_cx + x;
				for(sx=dest_x;sx<iright;sx++)
				{
					rgb.rgbBlue = ALPHA_COLOR(pvBits[c_src].rgbBlue, pvMyBits[c].rgbBlue, pvBits[c_src].rgbAlpha);
					rgb.rgbGreen = ALPHA_COLOR(pvBits[c_src].rgbGreen, pvMyBits[c].rgbGreen, pvBits[c_src].rgbAlpha);
					rgb.rgbRed = ALPHA_COLOR(pvBits[c_src].rgbRed, pvMyBits[c].rgbRed, pvBits[c_src].rgbAlpha);
					pvMyBits[c].rgbBlue = ALPHA_COLOR(rgb.rgbBlue, pvMyBits[c].rgbBlue, bAlphaConstant);
					pvMyBits[c].rgbGreen = ALPHA_COLOR(rgb.rgbGreen, pvMyBits[c].rgbGreen, bAlphaConstant);
					pvMyBits[c].rgbRed = ALPHA_COLOR(rgb.rgbRed, pvMyBits[c].rgbRed, bAlphaConstant);
					++c;
					++c_src;
				}
				++y;
			}
		}
	}
	else
	{
		if (bAlphaConstant == 255)
		{
			for(sy=dest_y;sy<ibottom;sy++)
			{
				c = sy*m_szX + dest_x;
				c_src = y*src_cx + x;
				for(sx=dest_x;sx<iright;sx++)
				{
					pvMyBits[c].rgbBlue = pvBits[c_src].rgbBlue;
					pvMyBits[c].rgbGreen = pvBits[c_src].rgbGreen;
					pvMyBits[c].rgbRed = pvBits[c_src].rgbRed;
					++c;
					++c_src;
				}
				++y;
			}			
		}	
		else
		{
			bAlphaConstant = 255-bAlphaConstant;
			for(sy=dest_y;sy<ibottom;sy++)
			{
				c = sy*m_szX + dest_x;
				c_src = y*src_cx + x;
				for(sx=dest_x;sx<iright;sx++)
				{
					pvMyBits[c].rgbBlue = ALPHA_COLOR(pvMyBits[c].rgbBlue, pvBits[c_src].rgbBlue, bAlphaConstant);
					pvMyBits[c].rgbGreen = ALPHA_COLOR(pvMyBits[c].rgbGreen, pvBits[c_src].rgbGreen, bAlphaConstant);
					pvMyBits[c].rgbRed = ALPHA_COLOR(pvMyBits[c].rgbRed, pvBits[c_src].rgbRed, bAlphaConstant);
					++c;
					++c_src;
				}
				++y;
			}
		}
	}
	// LocalUnlock(m_pvBits);
	__LeaveCriticalSection(&m_lpcrs);
}


// FUNCTION:		RenderRawTransparentBitmap
// DESCRIPTION:		Renders bitmap with transparent color to our bitmap
// ARGUMENTS:
//					pvBits		- bits of source bitmap
//					src_cx		- width of source bitmap
//					src_cy		- height of source bitmap
//					x, y		- upper-left conner of source bitmap target area
//					cx, cy		- width/height of source bitmap target area
//					dest_x, dest_y - upper-left conner of our bitmap
//					crTransparent	- transparent color
//					dwFlags		- reserved
//
// REVISION:		
//					Created Mar-24-2007 by ALXR

VOID CBitmap::RenderRawTransparentBitmap(CONST WRGBQUAD *pvBits, INT src_width, INT src_height, INT x, INT y, INT cx, INT cy, INT dest_x, INT dest_y, COLORREF crTransparent, DWORD dwFlags)
{
	if (!m_uSize || m_bLocked) return;		// return if bitmap was not created

	INT sx = dest_x, sy = dest_y, iright = sx+cx, ibottom = sy+cy, c = 0, c_src = 0;
	if (sx+cx > m_szX || sy + cy > m_szY || !cx || !cy) return;

	__EnterCriticalSection(&m_lpcrs);
	LPDWORD lpdwBits = (LPDWORD) pvBits;
	LPDWORD pvMyBits = (LPDWORD) m_pvBits; // LocalLock(m_pvBits);


	for(sy=dest_y;sy<ibottom;sy++)
	{
		c = sy*m_szX + dest_x;
		c_src = y*src_width + x;
		for(sx=dest_x;sx<iright;sx++)
		{
			LPDWORD p_src = &lpdwBits[c_src];

			if (*p_src != crTransparent) // wrqCrTrnsp->rgbRed != pvBits[c_src].rgbRed || wrqCrTrnsp->rgbGreen != pvBits[c_src].rgbGreen || wrqCrTrnsp->rgbBlue != pvBits[c_src].rgbBlue)
			{
				pvMyBits[c] = *p_src;
			}
			++c;
			++c_src;
		}
		++y;
	}


	// LocalUnlock(m_pvBits);
	__LeaveCriticalSection(&m_lpcrs);
}

// FUNCTION:		RenderBitmap
// DESCRIPTION:		Wrapper for RenderRawTransparentBitmap and RenderRawBitmap
// ARGUMENTS:
//					hSourceBmp	- handle of source bitmap
//					bAlpha		- transparency level
//					x, y		- upper-left conner of source bitmap target area
//					cx, cy		- width/height of source bitmap target area
//					dest_x, dest_y - upper-left conner of our bitmap
//					crTransparent	- transparent color
//					dwFlags		- reserved
//
// REVISION:		
//					Created Mar-24-2007 by ALXR

VOID CBitmap::RenderBitmap(HBITMAP hSourceBmp, BYTE bAlphaConstant, INT x, INT y, INT cx, INT cy, INT dest_x, INT dest_y, COLORREF crTransparent, DWORD dwFlags)
{
	if (!m_uSize || m_bLocked) return;		// return if bitmap was not created

	INT sx = dest_x, sy = dest_y;
	if (sx+cx >= m_szX || sy + cy >= m_szY || !cx || !cy) return;
	
	CBitmap bmpSrc;
	bmpSrc.CreateFromBitmap( hSourceBmp );
	if ( bmpSrc.Width() > 0 || bmpSrc.Height() > 0 )
		RenderRawBitmap( bmpSrc.LockRawBits(), bmpSrc.Width(), bmpSrc.Height(), bAlphaConstant, FALSE, x, y, cx, cy, dest_x, dest_y, dwFlags );

}



// FUNCTION:		FillRect
// DESCRIPTION:		Fills rect with border by the specified color 
// ARGUMENTS:
//					x, y		- upper-left conner
//					cx, cy		- dimensions of rectangle
//					crColor		- rectangle color
//
// REVISION:		
//					Created Mar-24-2007 by ALXR

VOID CBitmap::FillRect(INT x, INT y, INT cx, INT cy, COLORREF crColor, COLORREF crBorder, INT iWidth, BYTE fInnerAlpha)
{
	if (!m_uSize || m_bLocked) return;		// return if bitmap was not created

	INT sx = x, sy = y,iright = x+cx,ibottom = y+cy, c = 0;

	if(iright > m_szX || ibottom > m_szY || x > m_szX || y > m_szY || x<0 || y<0 || cx<=0 || cy<=0) return;
	crColor = XCHGCOLORREF(crColor); crBorder = XCHGCOLORREF(crBorder);
	WRGBQUAD *rgb = (WRGBQUAD*) &crColor;
	
	((WRGBQUAD*) &crColor)->rgbAlpha = fInnerAlpha;
	((WRGBQUAD*) &crBorder)->rgbAlpha = 255;


	__EnterCriticalSection(&m_lpcrs);

	WRGBQUAD * pvBits = (WRGBQUAD*) m_pvBits; // LocalLock(m_pvBits);

	if (iWidth > 0 || crColor != crBorder)
	{
		for(sy = y; sy<ibottom; sy++)
		{
			c = sy*m_szX+x;

			if(sy-y < iWidth || ibottom-sy-1 < iWidth)
			{
				rgb = (WRGBQUAD*) &crBorder;
			}
			else
			{
				rgb = (WRGBQUAD*) &crColor;
			}

			for(sx = x; sx<iright; sx++)
			{
				if (sx-x < iWidth || iright-sx-1 < iWidth)
				{
					pvBits[c] = *((WRGBQUAD*) &crBorder);
				}
				else
				{
					pvBits[c] = *rgb;
				}
				++c;
			}
		}
	}
	else
	{
		for(sy = y; sy<ibottom; sy++)
		{
			c = sy*m_szX+x;

			for(sx = x; sx<iright; sx++)
			{
				pvBits[c] = *rgb;
				pvBits[c].rgbAlpha = fInnerAlpha;
				++c;
			}
		}

	}
		
	// LocalUnlock(m_pvBits);
	__LeaveCriticalSection(&m_lpcrs);
}


// FUNCTION:		CopyRawRect
// DESCRIPTION:		Copies rectangle from specified bitmap to our bitmap
// ARGUMENTS:
//					pvBits		- bits of source bitmap
//					src_cx, src_cy	- dimensions of source bitmap
//					x, y		- upper-left conner of source bitmap
//					cx, cy		- dimensions of rectangle
//					dest_x, dest_y	- upper-left conner of our bitmap
//
// REVISION:		
//					Created Mar-24-2007 by ALXR

VOID CBitmap::CopyRawRect(CONST WRGBQUAD * pvBits, INT src_cx, INT src_cy, INT x, INT y, INT cx, INT cy, INT dest_x, INT dest_y)
{
	if (!m_uSize || m_bLocked) return;		// return if bitmap was not created

	INT sx = dest_x, sy = dest_y,iright = dest_x+cx,ibottom = dest_y+cy, c = 0, c_src;

	if(iright > m_szX || ibottom > m_szY || dest_x > m_szX || dest_y > m_szY || dest_x<0 || dest_y<0 || cx<=0 || cy<=0) return;

	__EnterCriticalSection(&m_lpcrs);
	WRGBQUAD * pvMyBits = (WRGBQUAD*) m_pvBits; // LocalLock(m_pvBits);

/*
	for(sy = dest_y; sy<ibottom; sy++)
	{
		c = sy*m_szX + dest_x;
		c_src = y*src_cx + x;
*/		

		INT szx = m_szX;			// global vars are invisible?
		c = sy*m_szX + dest_x;		// init arrays indexes
		c_src = y*src_cx + x;
		LPVOID lpvMyBits = &pvMyBits[c];	// load addresses of bits
		LPVOID lpvBits = (CONST LPWRGBQUAD) &pvBits[c_src];
		DWORD c_repeat = iright-sx, y_repeat = ibottom-dest_y;	// calcs repeat counters


		__asm
		{
			push ecx;
			push edi;
			push esi;							// save the registers

			mov ecx,y_repeat;					// initialize loop counter

myrep:
			push ecx;							// save counter
			mov ecx, c_repeat;
			mov edi, lpvMyBits;
			mov esi, lpvBits;
			rep movsd;							// copy line from lpvBits to lpvMyBits
			mov eax,szx;						// go to the next line of lpvMyBits
			add c,eax;
			mov eax,src_cx;						// go to the next line of lpvBits
			add c_src,eax;
		}

		lpvMyBits = &pvMyBits[c];						// get the address of new line
		lpvBits = (CONST LPWRGBQUAD) &pvBits[c_src];

		__asm
		{
			pop ecx;
			loop myrep;							// loop

			pop esi;							// restore registers
			pop edi;
			pop ecx;

		}
		/*
		for(sx = dest_x; sx<iright; sx++)
		{
			pvMyBits[c] = pvBits[c_src];
			++c;
			++c_src;
		}
		*/
		//++y;
		
//	}
//	LocalUnlock(m_pvBits);
	__LeaveCriticalSection(&m_lpcrs);
}

// FUNCTION:		CopyRect
// DESCRIPTION:		Wrapper for CopyRawRect
// ARGUMENTS:
//					hSrcBitmap		- handle of source bitmap
//					x, y		- upper-left conner of source bitmap
//					cx, cy		- dimensions of rectangle
//					dest_x, dest_y	- upper-left conner of our bitmap
//
// REVISION:		
//					Created Mar-24-2007 by ALXR

VOID CBitmap::CopyRect(HBITMAP hSrcBitmap, INT x, INT y, INT cx, INT cy, INT dest_x, INT dest_y)
{
	if (!m_uSize || m_bLocked) return;		// return if bitmap was not created

	INT iright = x+cx,ibottom = y+cy;

	if(iright > m_szX || ibottom > m_szY || x > m_szX || y > m_szY || x<0 || y<0 || cx<=0 || cy<=0) return;

	CBitmap bmpSrc;
	bmpSrc.CreateFromBitmap(hSrcBitmap);
	if ( bmpSrc.Width() > 0 && bmpSrc.Height() > 0 )
		CopyRawRect( bmpSrc.LockRawBits(), bmpSrc.Width(), bmpSrc.Height(), x, y, cx, cy, dest_x, dest_y );
}


// FUNCTION:		RenderText
// DESCRIPTION:		Renders text to our bitmap.
// ARGUMENTS:
//					x, y		- upper-left conner of source bitmap
//					cx, cy		- dimensions of rectangle
//					pszText		- text to render
//					hFont		- font of text
//					bTransparent - draws transparent text if its TRUE
//					crTextColor	- color of text
//					crBackColor - color of background if bTransparent is FALSE
//					dwFlags		- flags for DrawText
//
// NOTE:			WARNING! The function will FAIL if another bitmap was selected to any DC,
//					because GDI can select only one bitmap into the any DC at the one time
//
// REVISION:		
//					Created Mar-24-2007 by ALXR

VOID CBitmap::RenderText(INT x, INT y, INT cx, INT cy, LPCTSTR pszText, HFONT hFont, BOOL bTransparent, COLORREF crTextColor, COLORREF crBackColor, DWORD dwFlags)
{
	if (!m_uSize || m_bLocked) return;		// return if bitmap was not created

	INT iright = x+cx,ibottom = y+cy;

	// if(iright > m_szX || ibottom > m_szY || x > m_szX || y > m_szY || x<0 || y<0 || cx<=0 || cy<=0) return;

	int bkMode = GetBkMode( m_hbufferDC );
	COLORREF clrText = GetTextColor( m_hbufferDC );
	COLORREF clrBg = GetBkColor( m_hbufferDC );
	HGDIOBJ hfntOld = SelectObject( m_hbufferDC, hFont );
	HGDIOBJ hbmOld = SelectObject( m_hbufferDC, m_hbufferBm );

	SetBkMode( m_hbufferDC,  bTransparent ? TRANSPARENT : OPAQUE );
	SetTextColor( m_hbufferDC, crTextColor );
	SetBkColor( m_hbufferDC, crBackColor );
	RECT rc = { x, y, x + cx, y + cy };

	DrawText( m_hbufferDC, pszText, lstrlen(pszText), &rc, dwFlags);

	SelectObject( m_hbufferDC, hbmOld );
	SetBkMode( m_hbufferDC,  bkMode );
	SetTextColor( m_hbufferDC, clrText );
	SetBkColor( m_hbufferDC, clrBg );
	SelectObject( m_hbufferDC, hfntOld );
}


VOID CBitmap::Copy(CBitmap * pSource)
{
	if (!pSource->Width() && !pSource->Height())
	{
		return;
	}

	this->Create( pSource->Width(), pSource->Height() );
	CopyMemory( m_pvBits, pSource->m_pvBits, m_uSize );
}

BOOL CBitmap::RenderToDevice(HDC hDC, UINT xDest, UINT yDest, UINT cxDest, UINT cyDest, UINT xSrc, UINT ySrc)
{
	if (!m_uSize || m_bLocked) return FALSE;

	BITMAPINFO bmp = { 0 };
    bmp.bmiHeader.biSize = sizeof(bmp);
    bmp.bmiHeader.biHeight = -m_szY;
    bmp.bmiHeader.biPlanes = 1;
    bmp.bmiHeader.biWidth = m_szX;
    bmp.bmiHeader.biBitCount = 32;
	
	DWORD l = 0;
	l = SetDIBitsToDevice(hDC, xDest, yDest, cxDest, cyDest, xSrc, ySrc, 0, m_szY, this->LockRawBits(), &bmp, DIB_RGB_COLORS);
	this->UnlockRawBits();
	return (l != 0);
}


VOID CBitmap::FillRectTransparentBits(WRGBQUAD * pvBits, INT src_width, INT src_height, INT chunk_X, INT chunk_Y, INT chunk_CX, INT chunk_CY, INT start_X, INT start_Y, BOOL bVertical, INT count, COLORREF crTransparent)
{
	if (!m_uSize || m_bLocked) return;

	if (count <= 0) return;

	while(count > 0)
	{
		RenderRawTransparentBitmap(pvBits, src_width, src_height, chunk_X, chunk_Y, chunk_CX, chunk_CY, start_X, start_Y, crTransparent, 0);
		if (bVertical) start_Y+=chunk_Y; else start_X+=chunk_X;
		count--;
	}	
}

VOID CBitmap::FillRectBits(WRGBQUAD * pvBits, INT src_width, INT src_height, INT chunk_X, INT chunk_Y, INT chunk_CX, INT chunk_CY, INT start_X, INT start_Y, BOOL bVertical, INT count)
{
	if (!m_uSize || m_bLocked) return;

	if (count <= 0) return;

	while(count > 0)
	{
		CopyRawRect(pvBits, src_width, src_height, chunk_X, chunk_Y, chunk_CX, chunk_CY, start_X, start_Y);
		if (bVertical) start_Y+=chunk_CY; else start_X+=chunk_CX;
		count--;
	}	
}

HDC CBitmap::LockDC()
{
	if ( m_holdBm )
		return m_hbufferDC;

	m_holdBm = (HBITMAP) SelectObject(m_hbufferDC, m_hbufferBm);
	return m_hbufferDC;
}

void CBitmap::FreeDC()
{
	if ( !m_holdBm )
		return;

	SelectObject(m_hbufferDC, m_holdBm);
	m_holdBm = NULL;
}

WRGBQUAD * CBitmap::LockRawBits()
{
	if (!m_uSize) return NULL;
//	m_bLocked = true;
//	return (WRGBQUAD*) LocalLock(m_pvBits);
	GdiFlush();
	return (WRGBQUAD*) m_pvBits;
}

#ifdef _DEBUG
VOID CBitmap::DebugInfo(LPSTR pszFuncName, UINT funcRet)
{
	return;
	UINT err = GetLastError();
	if ( !funcRet || err != 0 )
	{
		TCHAR pszErr[256];
		char pszErrA[256];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, NULL, pszErr, sizeof(pszErr), NULL);
		CharToOem(pszErr, pszErrA);
		//printf("Function '%s' returned %d [LastError: %d ['%s']]\n", pszFuncName, funcRet, err, pszErrA);
	}
//	else
		//printf("Function '%s' returns SUCCESS.\n", pszFuncName);
}
#endif