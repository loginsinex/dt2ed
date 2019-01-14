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

CNesCanvas::CNesCanvas()
	: m_hbmDC( NULL ), m_pBufView( NULL )
{
	m_curs.active = -1;
	m_curs.fVisible = FALSE;
	m_curs.fHidden = FALSE;
	m_hbrBg = NULL;
}

CNesCanvas::~CNesCanvas()
{
	for(std::map<INT, CANVAS_CURSOR>::iterator v = m_curs.mCursors.begin(); v != m_curs.mCursors.end(); v++)
	{
		if ( v->second.pbmp )
			delete v->second.pbmp;
	}

	if ( m_hbrBg )
		DeleteObject(m_hbrBg);

	m_curs.mCursors.clear();
}

VOID CNesCanvas::SetBg(CBitmap & bg)
{
	if ( m_hbrBg )
		DeleteObject(m_hbrBg);

	m_hbrBg = bg.CreateBrush();
}


VOID CNesCanvas::InitBufView(CBufView * pBufView, INT cx, INT cy)
{
	if ( pBufView )
	{
		m_pBufView = pBufView;

		m_pBufView->Assign(CNesCanvas::BufViewProc, this);
		m_scroll.Assign(*m_pBufView);
		SetTimer(*m_pBufView, 1, 300, NULL);

		ResizeBufView(cx, cy);
	}
}

VOID CNesCanvas::ResizeBufView(INT cx, INT cy)
{
	if ( !m_pBufView )
		return;

	RECT rc = { 0 };
	GetClientRect(*m_pBufView, &rc);
	FillRect(*m_pBufView, &rc, ( m_hbrBg ? m_hbrBg : GetStockBrush(BLACK_BRUSH)));

	if ( m_hbmDC )
		m_bmImage.FreeDC();

	m_rcArea.cx = cx;
	m_rcArea.cy = cy;

	m_bmCanvas.Create(cx, cy);
	m_bmImage.Create(cx, cy);

	SetRect(&rc, 0, 0, cx, cy);
	m_hbmDC = m_bmImage.LockDC();
	FillRect(m_hbmDC, &rc, ( m_hbrBg ? m_hbrBg : GetStockBrush(BLACK_BRUSH)));
		
	m_scroll.SetScrollArea(m_bmCanvas.Width(), m_bmCanvas.Height());
	m_scroll.SetScrollChars(32, 8);
}

LRESULT CALLBACK CNesCanvas::BufViewProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, PVOID pData)
{
	if ( pData )
		return ((CNesCanvas*) pData)->BVProc(uMsg, wParam, lParam);
	else
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


LRESULT CNesCanvas::BVProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_CHAR:
	case WM_SYSCHAR:
		{
			return 0;
		}
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		{
			SetFocus(*m_pBufView);
			break;
		}
	case WM_SETFOCUS:
		{
			m_curs.fVisible = TRUE;
			ShowActiveCursor(m_curs.fVisible);
			break;
		}
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		{
			RECT rc = m_scroll.GetClientArea();
			if ( !GetScrollPos( *m_pBufView, SB_HORZ ) )
			{
				rc.right -= rc.left;
				rc.left = 0;
			}

			if ( !GetScrollPos( *m_pBufView, SB_VERT ) )
			{
				rc.bottom -= rc.top;
				rc.top = 0;
			}

			SetFocus(*m_pBufView);
			ShowActiveCursor(FALSE);
			BOOL fCursorMoved = SetActiveCursorPos(GET_X_LPARAM(lParam) + rc.left , GET_Y_LPARAM(lParam) + rc.top );

			if ( fCursorMoved && WM_LBUTTONDBLCLK == uMsg )
				ReqModify(VK_INSERT);

			if ( !fCursorMoved )
				MessageBeep(0x10);

			break;
		}
	case WM_MOUSEWHEEL:
		{
			SHORT wheel = GET_WHEEL_DELTA_WPARAM(wParam);
			if ( wheel < 0 )
				return BVProc(WM_VSCROLL, SB_LINEDOWN, 0);
			else 
				return BVProc(WM_VSCROLL, SB_LINEUP, 0);
			break;
		}
	case WM_GETDLGCODE:
		{
			CVNDLGCODE cv = { 0 };
			cv.nm.code = CVN_REQDLGCODE;
			cv.nm.hwndFrom = *m_pBufView;
			cv.nm.idFrom = GetWindowLong(*m_pBufView, GWL_ID);
			if ( m_curs.mCursors.find(m_curs.active) != m_curs.mCursors.end() )
				cv.pt = m_curs.mCursors[m_curs.active].pt;
			cv.id = m_curs.active;
			cv.vKey = wParam; cv.lpmsg = (LPMSG) lParam;

			LRESULT ret = SendMessage(GetParent(*m_pBufView), WM_NOTIFY, 0, (LPARAM) &cv);
			return ( ret ? ret : (DLGC_WANTARROWS | DLGC_WANTCHARS) );
		}
	case WM_KEYDOWN:
		{
			switch(wParam)
			{
			case VK_LEFT: MoveActiveCursorPos( -1, 0 ); break;
			case VK_RIGHT: MoveActiveCursorPos( 1, 0 ); break;
			case VK_UP: MoveActiveCursorPos( 0, -1 ); break;
			case VK_DOWN: MoveActiveCursorPos( 0, 1 ); break;

			case VK_OEM_PLUS:
			case VK_ADD: ChangeItem(1); break;

			case VK_OEM_MINUS:
			case VK_SUBTRACT: ChangeItem(-1); break;
			case VK_PRIOR: ChangeItem(-32); break;
			case VK_NEXT: ChangeItem(32); break;

			case VK_SPACE: ChangeItem(0); break;
			default: ReqModify(wParam); break;
			}
			break;
		}
	case WM_VSCROLL:
	case WM_HSCROLL:
		{
			RECT rc = { 0 };
			GetClientRect(*m_pBufView, &rc);

			if ( m_rcArea.cx > rc.right && ( WM_HSCROLL == uMsg ) ||
				m_rcArea.cy > rc.bottom && ( WM_VSCROLL == uMsg )
				)
				m_scroll.ScrollEvent(uMsg, wParam);

			Update();
			break;
		}
	case WM_TIMER:
		{
			BlinkCursor();
			break;
		}
	case WM_SIZE:
	case WM_SIZING:
		{
			m_scroll.UpdateScroll();
			Update();
			break;
		}
	case WM_DESTROY:
		{
			m_bmCanvas.FreeDC();
			m_hbmDC = NULL;
			break;
		}
	}

	return DefWindowProc(*m_pBufView, uMsg, wParam, lParam);
}


CNesCanvas::operator HDC()
{
	return ( m_pBufView ? m_hbmDC : NULL );
}

CNesCanvas::operator HWND()
{
	return ( m_pBufView ? *m_pBufView : NULL );
}


CNesCanvas::operator HBRUSH()
{
	return m_hbrBg;
}

VOID CNesCanvas::Refresh()
{
	if ( m_pBufView )
	{
		RECT rc = m_scroll.GetClientArea();

		if ( !GetScrollPos( *m_pBufView, SB_HORZ ) )
		{
			rc.right -= rc.left;
			rc.left = 0;
		}

		if ( !GetScrollPos( *m_pBufView, SB_VERT ) )
		{
			rc.bottom -= rc.top;
			rc.top = 0;
		}

		BitBlt(*m_pBufView, 0, 0, rc.right - rc.left, rc.bottom - rc.top, m_bmCanvas.LockDC(), rc.left, rc.top, SRCCOPY);
		m_bmCanvas.FreeDC();
		m_pBufView->Refresh();
	}
}

VOID CNesCanvas::Update()
{
	if ( m_pBufView )
	{
		BitBlt(m_bmCanvas.LockDC(), 0, 0, m_bmCanvas.Width(), m_bmCanvas.Height(), m_hbmDC, 0, 0, SRCCOPY);
		m_bmCanvas.FreeDC();
		Refresh();
	}
}

VOID CNesCanvas::SetCursorSet(INT id, COLORREF clr, INT cx, INT cy, INT delta_x, INT delta_y, BOOL fMouseCursor)
{
	CBitmap * pbmp = NULL;
	if ( m_curs.mCursors.find(id) != m_curs.mCursors.end() )
	{
		CANVAS_CURSOR & curs = m_curs.mCursors[id];
		delete curs.pbmp;

		curs.pbmp = new CBitmap;
		curs.pbmp->Create(cx, cy);
		curs.delta.cx = delta_x;
		curs.delta.cy = delta_y;
		curs.pt.x = curs.pt.y = 0;
		curs.fMouseCursor = fMouseCursor;
		
		pbmp = curs.pbmp;
	}
	else
	{
		CANVAS_CURSOR curs = { 0 };
		curs.pbmp = new CBitmap;
		curs.pbmp->Create(cx, cy);
		curs.delta.cx = delta_x;
		curs.delta.cy = delta_y;
		curs.pt.x = curs.pt.y = 0;
		curs.fMouseCursor = fMouseCursor;

		pbmp = curs.pbmp;
		m_curs.mCursors[id] = curs;
	}

	if ( pbmp )
		pbmp->FillRect(0, 0, pbmp->Width(), pbmp->Height(), clr, clr, 1, 190);
}

VOID CNesCanvas::SetActiveCursor(INT id)
{
	if ( m_curs.mCursors.find(id) != m_curs.mCursors.end() )
	{
		ShowActiveCursor(FALSE);

		m_curs.active = id;
	}
}

INT CNesCanvas::GetActiveCursor()
{
	return m_curs.active;
}

POINT CNesCanvas::GetActiveCursorPos()
{
	if ( m_curs.mCursors.find(m_curs.active) != m_curs.mCursors.end() )
		return m_curs.mCursors[m_curs.active].pt;

	POINT pt = { -1, -1 };
	return pt;
}

VOID CNesCanvas::BlinkCursor()
{
	m_curs.fVisible = !m_curs.fVisible;
	ShowActiveCursor(m_curs.fVisible);
}

BOOL CNesCanvas::SetActiveCursorPos(INT x, INT y, BOOL fKeybdEvent)
{
	if ( m_curs.mCursors.find(m_curs.active) != m_curs.mCursors.end() )
	{
		CANVAS_CURSOR & curs = m_curs.mCursors[m_curs.active];
		x /= curs.delta.cx;
		y /= curs.delta.cy;

		CVNREQPOS cvn = { 0 };
		cvn.nm.code = CVN_REQUESTPOINT;
		cvn.nm.hwndFrom = *m_pBufView;
		cvn.nm.idFrom = GetWindowLong(*m_pBufView, GWL_ID);
		cvn.pt.x = x;
		cvn.pt.y = y;
		cvn.id = m_curs.active;
		cvn.fKeybdEvent = fKeybdEvent;

		if ( !SendMessage(GetParent(*m_pBufView), WM_NOTIFY, 0, (LPARAM) &cvn) )
		{
			ShowActiveCursor(FALSE);
			// curs.pt.x = x; curs.pt.y = y;
			curs.pt.x = cvn.pt.x;
			curs.pt.y = cvn.pt.y;
			m_curs.fVisible = TRUE;

			ShowActiveCursor(m_curs.fVisible);
			return TRUE;
		}
	}

	return FALSE;
}

VOID CNesCanvas::MoveActiveCursorPos(INT x, INT y)
{
	if ( m_curs.mCursors.find(m_curs.active) != m_curs.mCursors.end() )
	{
		CANVAS_CURSOR & curs = m_curs.mCursors[m_curs.active];
		SetActiveCursorPos( curs.delta.cx * (curs.pt.x + x), curs.delta.cy * (curs.pt.y + y), TRUE );
	}	
}


VOID CNesCanvas::ShowActiveCursor(BOOL fShow, BOOL fRefresh)
{
	if ( m_curs.mCursors.find(m_curs.active) != m_curs.mCursors.end() )
	{
		CANVAS_CURSOR & curs = m_curs.mCursors[m_curs.active];

		CBitmap * pcurs = curs.pbmp;
		if ( !pcurs )
			return;

		const int x = curs.pt.x * curs.delta.cx, y = curs.pt.y * curs.delta.cy;

		if ( !m_curs.fHidden && fShow && GetFocus() == *m_pBufView )
		{
			m_bmCanvas.RenderRawBitmap(pcurs->LockRawBits(), pcurs->Width(), pcurs->Height(), 255, TRUE, 
				0, 0, pcurs->Width(), pcurs->Height(), x, y, 0);
		}
		else
		{
			BitBlt(m_bmCanvas.LockDC(), x, y, pcurs->Width(), pcurs->Height(),
				m_hbmDC, x, y, SRCCOPY);

			m_bmCanvas.FreeDC();
		}

		if ( fRefresh )
			Refresh();
	}
}

VOID CNesCanvas::ChangeItem(INT value)
{
	if ( m_curs.mCursors.find(m_curs.active) != m_curs.mCursors.end() )
	{
		CANVAS_CURSOR & curs = m_curs.mCursors[m_curs.active];

		CVNREQMODIFY cvn = { 0 };
		cvn.nm.code = CVN_REQUESTMODIFY;
		cvn.nm.hwndFrom = *m_pBufView;
		cvn.nm.idFrom = GetWindowLong(*m_pBufView, GWL_ID);
		cvn.pt.x = curs.pt.x;
		cvn.pt.y = curs.pt.y;
		cvn.delta = value;
		cvn.key = -1;
		cvn.id = m_curs.active;

		SendMessage(GetParent(*m_pBufView), WM_NOTIFY, 0, (LPARAM) &cvn);
	}
}

VOID CNesCanvas::ReqModify(INT vKey)
{
	if ( m_curs.mCursors.find(m_curs.active) != m_curs.mCursors.end() )
	{
		CANVAS_CURSOR & curs = m_curs.mCursors[m_curs.active];
		CVNREQMODIFY cvn = { 0 };
		cvn.nm.code = CVN_REQUESTMODIFY;
		cvn.nm.hwndFrom = *m_pBufView;
		cvn.nm.idFrom = GetWindowLong(*m_pBufView, GWL_ID);
		cvn.pt.x = curs.pt.x;
		cvn.pt.y = curs.pt.y;
		cvn.delta = 0;
		cvn.key = vKey;
		cvn.id = m_curs.active;

		SendMessage(GetParent(*m_pBufView), WM_NOTIFY, 0, (LPARAM) &cvn);
	}
}

VOID CNesCanvas::SetCursorPosition(INT x, INT y)
{
	if ( m_curs.mCursors.find(m_curs.active) != m_curs.mCursors.end() )
	{
		CANVAS_CURSOR & curs = m_curs.mCursors[m_curs.active];
		SetActiveCursorPos(x * curs.pbmp->Width(), y * curs.pbmp->Height());	
	}
}

VOID CNesCanvas::SetCursorColor(INT id, COLORREF clr)
{
	if ( m_curs.mCursors.find(id) != m_curs.mCursors.end() )
	{
		CANVAS_CURSOR & curs = m_curs.mCursors[m_curs.active];
		curs.pbmp->FillRect(0, 0, curs.pbmp->Width(), curs.pbmp->Height(), clr, clr, 1, 100);
	}
}

VOID CNesCanvas::HideCursor(BOOL fHide)
{
	m_curs.fHidden = fHide;
	ShowActiveCursor(!fHide);
}