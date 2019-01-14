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

CTileSetDlg::CTileSetDlg(HINSTANCE hInstance, CNesTiles & nLevel, std::vector<NES_BLOCK> & nMinor, PBYTE nPalTiles)
	: CDialog(hInstance), m_nLevel(nLevel), m_nMinor( nMinor )
{
	InitBufView(hInstance, TEXT("SysTileSet32"));
	CopyMemory(m_bPalTiles, nPalTiles, sizeof(m_bPalTiles));

	AddAnchor(0x1001, ANCHOR_FULL);
	AddAnchor(0x1002, ANCHOR_LEFT_BOTTOM);
	AddAnchor(IDCANCEL, ANCHOR_RIGHT_BOTTOM);
}

LRESULT CALLBACK CTileSetDlg::BufViewProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, PVOID pData)
{
	if ( pData )
		return ((CTileSetDlg*) pData)->BVProc(uMsg, wParam, lParam);
	else
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CTileSetDlg::BVProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_HSCROLL:
	case WM_VSCROLL:
		{
			m_scroll.ScrollEvent(uMsg, wParam);
			Update();
			break;
		}
	case WM_SIZE:
	case WM_SIZING:
		{
			m_scroll.UpdateScroll();
			Update();
			break;
		}
	case WM_MOUSEMOVE:
		{
			RECT rc = m_scroll.GetClientArea();

			if ( !GetScrollPos(*m_pBufView, SB_HORZ) )
				rc.left = 0;
			if ( !GetScrollPos(*m_pBufView, SB_VERT) )
				rc.top = 0;

			rc.left += GET_X_LPARAM(lParam); rc.top += GET_Y_LPARAM(lParam);

			CString s;

			if ( rc.left >= 0 && rc.top >= 0 && rc.left < 256 && rc.top < 256 )
			{
				s.Format(TEXT("Tile: #%02X"), rc.left / 16 + 16 * ( rc.top / 16 ));
			}
			else if ( rc.left >= 272 && rc.top >= 0 && rc.left < 528 && rc.top < 256 )
			{
				s.Format(TEXT("Tile: #%02X"), ( rc.left - 272 ) / 16 + 16 * ( rc.top / 16 ) + 0x100);
			}
			else if ( rc.left >= 536 && rc.top >= 0 && rc.left < 1080 && rc.top < 544 )
			{
				INT block = ( rc.left - 536 ) / 34 + 16 * ( rc.top / 34 );
				INT nPal = ( m_bPalTiles[( block ) / 4] >> ( 2 * ( ( (block ) % 4 ) ) ) ) & 3;
				s.Format(TEXT("Tiles block: #%02X\nPalette: %d"), block, nPal);
			}

			if ( GetCtlText(0x1002) != s )
				SetCtlText(0x1002, s);

			break;
		}
	}
	return DefWindowProc(*m_pBufView, uMsg, wParam, lParam);
}

VOID CTileSetDlg::OnPreInit(CDialogTemplate & d)
{
	d.SetDlgStyle(DS_SHELLFONT | WS_BORDER | WS_CAPTION | WS_SIZEBOX | WS_SYSMENU | WS_MAXIMIZEBOX, 0);
	d.SetDlgTitle(TEXT("Tiles set"));
	d.SetDlgPos(0, 0, 400, 200);

	d.AddPreItem(0x1001, TEXT("SysTileSet32"), NULL, 7, 7, -7, -(7 + 15 + 7), 0, WS_EX_STATICEDGE);
	d.AddPreItem(0x1002, cStatic, NULL, 7, -(7 + 15 + 7), 100, 15, 0, TRUE, TRUE, FALSE);

	d.AddPreItem(IDCANCEL, cButton, MB_GetString(IDCLOSE - 1), -(7 + 50), -(7 + 15), 50, 15, BS_PUSHBUTTON);
	
}

VOID CTileSetDlg::OnInit(LPARAM lParam)
{
	m_pBufView = (CBufView*) dSendMessage(0x1001, BVM_GETINSTANCE);

	m_dc.Create(1500, 600);

	if ( m_pBufView )
	{
		m_pBufView->Assign(CTileSetDlg::BufViewProc, this);
		m_scroll.Assign(*m_pBufView);
		m_scroll.SetScrollArea(1500, 600);
		m_scroll.SetScrollChars(32, 256);
		m_nLevel.DrawTileSet(m_dc, 0, 0, m_nMinor, 2);
		Update();
	}
}

VOID CTileSetDlg::Update()
{
	RECT rc = m_scroll.GetClientArea();
	
	if ( !GetScrollPos(*m_pBufView, SB_HORZ) )
		rc.left = 0;
	if ( !GetScrollPos(*m_pBufView, SB_VERT) )
		rc.top = 0;

	BitBlt(*m_pBufView, 0, 0, rc.right - rc.left, rc.bottom - rc.top, m_dc, rc.left, rc.top, SRCCOPY);
	m_pBufView->Refresh();
}