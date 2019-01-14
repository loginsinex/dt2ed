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

#define			BVM_GETINSTANCE			( WM_USER + 0x111 )
#define			BVM_GETUPDATEAREA		( WM_USER + 0x112 )

VOID InitBufView(HINSTANCE hInstance, LPTSTR pszClass);
typedef LRESULT (CALLBACK* BVPROC)(HWND, UINT, WPARAM, LPARAM, PVOID);

class CBufView
{
	HWND		m_hWnd;
	BVPROC		m_pwndProc;
	PVOID		m_pParam;
	HINSTANCE	m_hInstance;
	LPTSTR		m_pszClass;
	CREATESTRUCT	m_cs;
	HFONT		m_hGlobalFont;
	HDC			m_hDC;

	struct
	{
		SIZE	sz;
		HDC		hDCDesktop;
		HDC		hDC;
		HBITMAP	hBmp;
		HBITMAP	hOldBmp;
		PBYTE	pvBits;
		BITMAPINFO bmi;

		struct
		{
			HFONT		hOldFont;
			HFONT		hFont;
		} font;
		struct
		{
			HPEN		hOldPen;
			HPEN		hPen;
		} pen;
		struct
		{
			HBRUSH		hOldBrush;
			HBRUSH		hBrush;
		} brush;
	} m_buffer;

	LRESULT			OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK		BufViewProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	friend	VOID InitBufView(HINSTANCE hInstance, LPTSTR pszClass);

	VOID			RenderOnDevice(HDC hDC);

public:
	CBufView(LPCREATESTRUCT lpcs);
	~CBufView();

	VOID		SetPen(HPEN hPen, BOOL fDestroy = TRUE);
	VOID		SetBrush(HBRUSH hBrush, BOOL fDestroy = TRUE);
	VOID		SetFont(HFONT hFont, BOOL fDestroy = TRUE);
	HWND		GetWindowHandle();
	HDC			GetBufferHandle();
	VOID		Refresh();
	VOID		Erase(HBRUSH hBrush, BOOL fDestroy = FALSE);
	VOID		Assign(BVPROC pDlgProc = NULL, PVOID pParam = NULL);
	operator HDC();
	operator HWND();
};