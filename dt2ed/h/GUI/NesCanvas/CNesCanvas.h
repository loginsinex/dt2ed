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

#define				CVN_FIRST						-1500
#define				CVN_REQUESTPOINT				( CVN_FIRST - 1 )
#define				CVN_REQUESTMODIFY				( CVN_FIRST - 2 ) 
#define				CVN_REQDLGCODE					( CVN_FIRST - 3 )

typedef struct _tagCVNDLGCODE
{
	NMHDR			nm;
	POINT			pt;
	INT				id;
	int				vKey;
	LPMSG			lpmsg;
} CVNDLGCODE, *PCVNDLGCODE;

typedef struct _tagCVNREQPOS
{
	NMHDR			nm;
	POINT			pt;
	INT				id;
	BOOL			fKeybdEvent;
} CVNREQPOS, *PCVNREQPOS;

typedef struct _tagCVNREQMODIFY
{
	NMHDR			nm;
	POINT			pt;
	INT				delta;
	INT				key;
	INT				id;
} CVNREQMODIFY, *PCVNREQMODIFY;

typedef struct _tagCANVAS_CURSOR
{
	SIZE			delta;
	POINT			pt;
	BOOL			fMouseCursor;
	CBitmap			*pbmp;
} CANVAS_CURSOR, *PCANVAS_CURSOR;

class CNesCanvas
{
	CBufView			* m_pBufView;
	CWinPaneScroller	m_scroll;
	CBitmap				m_bmCanvas;
	CBitmap				m_bmImage;
	HBRUSH				m_hbrBg;
	SIZE				m_rcArea;

	struct
	{
		INT								active;
		BOOL							fVisible;
		BOOL							fHidden;
		std::map<INT, CANVAS_CURSOR>	mCursors;
	} m_curs;

	HDC					m_hbmDC;

	static LRESULT	CALLBACK	BufViewProc(HWND, UINT, WPARAM, LPARAM, PVOID);
	LRESULT						BVProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	BOOL						SetActiveCursorPos(INT x, INT y, BOOL fKeybdEvent = FALSE);
	VOID						MoveActiveCursorPos(INT x, INT y);
	VOID						ShowActiveCursor(BOOL fVisible, BOOL fRefresh = TRUE);
	VOID						ChangeItem(INT value);
	VOID						BlinkCursor();
	VOID						Refresh();
	VOID						ReqModify(INT vKey);

public:
	CNesCanvas();
	~CNesCanvas();

	VOID				InitBufView(CBufView * pBufView, INT cx, INT cy);
	VOID				ResizeBufView(INT cx, INT cy);

	VOID				SetBg(CBitmap & bg);
	VOID				SetCursorSet(INT id, COLORREF clr, INT cx, INT cy, INT delta_x, INT delta_y,
										BOOL fMouseCursor = FALSE);

	VOID				SetActiveCursor(INT id);
	VOID				SetCursorPosition(INT x, INT y);
	VOID				SetCursorColor(INT id, COLORREF clr);

	INT					GetActiveCursor();
	POINT				GetActiveCursorPos();
	VOID				HideCursor(BOOL fHide);

	operator HDC();
	operator HWND();
	operator HBRUSH();

	VOID				Update();
};