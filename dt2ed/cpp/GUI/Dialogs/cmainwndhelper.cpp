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
#include <Shellapi.h>

void CMainWnd::OnInitPane()
{
	RECT rc = { 0 };
	m_dlgLeftPane.Create(this);
	GetClientRect(m_dlgLeftPane, &rc);
	m_panel.sz.cx = rc.right; m_panel.sz.cy = rc.bottom;

	SetWindowPos(m_dlgLeftPane, NULL, 0, 0, m_panel.sz.cx, m_panel.sz.cy, 0);
	ShowWindow(m_dlgLeftPane, SW_SHOW);
}

void CMainWnd::OnRebarHeightChange()
{
	RECT rcWnd = { 0 };
	RECT rcBar = { 0 };
	RECT rcStb = { 0 };
	RECT rcPan = { 0 };
	RECT rcTab = { 0 };

	GetClientRect(*this, &rcWnd);
	GetClientRect(Ctl(IDC_REBAR), &rcBar);
	GetClientRect(Ctl(IDC_STATUSBAR), &rcStb);
	GetClientRect(m_dlgLeftPane, &rcPan);
	
	SetWindowPos(m_dlgLeftPane, NULL, 
		0, rcBar.bottom, 
		rcPan.right = m_panel.sz.cx, rcWnd.bottom - rcStb.bottom - rcBar.bottom, 0);

	SetWindowPos(Ctl(IDC_CANVAS), NULL,
		rcPan.right, rcBar.bottom, rcWnd.right - rcPan.right, rcWnd.bottom - rcStb.bottom - rcBar.bottom, 0);

	UpdateAnchor(IDC_TAB1);
	UpdateAnchor(IDC_CANVAS);
}

VOID CMainWnd::ShowStat(POINT * ppt)
{
	CString sText;
	sText.Format(TEXT("File: %s"), PathFindFileName( m_File ));
	dSendMessage(IDC_STATUSBAR, WM_SETTEXT, 0, (LPARAM) sText.GetString());


	// hide all items
	m_dlgLeftPane.EnableItem(FALSE, ID_SHOWDIALOG);
	m_dlgLeftPane.EnableItem(FALSE, ID_TOOLS_GRAPHICSEDITOR);

	sText = TEXT("");
	POINT pt = m_Canvas.GetActiveCursorPos();

	if ( ppt )
		pt = *ppt;
	
	switch(m_Canvas.GetActiveCursor())
	{
	case 0:
		{
			BYTE bRoom, bBlock;
			if ( m_nEditor.GetBlockFromPoint(pt.x, pt.y, bRoom, bBlock) )
				sText.Format(TEXT("Room: #%02X, Block: #%02X"), bRoom + 1, bBlock);
			break;
		}
	case 1:
		{
			BYTE bRoom, bBlock;
			if ( m_nEditor.GetBlockFromPoint(pt.x * 8, pt.y * 8, bRoom, bBlock) )
				sText.Format(TEXT("Room: #%02X"), bRoom + 1);
			else
				sText.Format(TEXT("No room"));
			break;
		}
	case 2:
		{
			BYTE nRoomY = LOBYTE(pt.y / ( 32 * 8 )), nRoomX = LOBYTE( pt.x / 32 );
			POINT opt = { pt.x - INT(nRoomX) * 32, pt.y - INT(nRoomY) * 256 };

			WORD id = ( m_wCatchedItem == 0xFFFF ?
				m_nEditor.GetObjectFromPos(nRoomX, nRoomY, LOBYTE(opt.x), LOBYTE(opt.y), opt, TRUE)
				:
				0xFFFF
					);
			switch(id)
			{
			case 0xFFFE:
				{
					sText.Format(TEXT("No object"));
					break;
				}
			case 0xFFFF:
				{
					CString sCatched;
					sCatched.Format(TEXT("Catched object #%02X"), m_wCatchedItem);

					sText.Format(TEXT("%s %02d:%02d"), 
						(m_wCatchedItem == 0xFFFF ?
						TEXT("No object at") :
						sCatched.GetString()),
						opt.x % 32, opt.y % 256);
					break;
				}
			default:
				{
					sText.Format(TEXT("Floor: %0d, Pos: %0d:%0d, Object: #%02X"), 
						nRoomY, opt.x % 32, opt.y % 256, id);

					if ( id >= 0x3c && id <= 0x42 )
						m_dlgLeftPane.EnableItem(TRUE, ID_SHOWDIALOG);
					else if ( id >= 0x9d )
						m_dlgLeftPane.EnableItem(TRUE, ID_TOOLS_GRAPHICSEDITOR);

					break;
				}
			}
			break;
		}
	case 3:
	case 4:
	case 8: 
		{
			NES_POINT npt[3];
			m_nEditor.Get_ScroogePos(npt[0], 0);
			m_nEditor.Get_ScroogePos(npt[1], 1);
			m_nEditor.Get_ScroogePos(npt[2], 2);

			sText.Format(
				TEXT("Scrooge: %02d:%02d/%02d:%02d, CP: %02d:%02d/%02d:%02d, BS: %02d:%02d/%02d:%02d"),
				npt[0].bScrX, npt[0].bScrY, npt[0].bPosX, npt[0].bPosY,
				npt[1].bScrX, npt[1].bScrY, npt[1].bPosX, npt[1].bPosY,
				npt[2].bScrX, npt[2].bScrY, npt[2].bPosX, npt[2].bPosY);
			break;
		}
	case 5:
		{
			if ( 0xFFFF != m_wCatchedSlot )
			{
				sText.Format(TEXT("Selected slots set #%02X"), m_wCatchedSlot);
			}
			break;
		}
	case 6:
		{
			sText.Format(TEXT("Bridge: %02d:%02d"), pt.x, pt.y);
			break;
		}
	case 7:
		{
			NES_MU_SECRET nmu;
			if ( m_nEditor.Get_MuSecret(nmu) )
			{
				if ( nmu.vobjX.size() > 0 )
				{
					CString sObj;
					NES_POINT npt = { 0 };
					if ( -1 != m_nEditor.GetMuObjectId(LOBYTE(pt.x / 256), LOBYTE(pt.y / 256), LOBYTE(pt.x % 256), LOBYTE(pt.y % 256),
						npt) )
					{
						sObj.Format(TEXT("Object: %d:%d"), npt.bPosX, npt.bPosY);
					}
					else
					{
						m_nEditor.ValidateMuObjectPos(pt);
						sObj.Format(TEXT("Cursor: %d:%d"), LOBYTE(pt.x % 256), LOBYTE(pt.y % 256));
					}
				
					sText.Format(TEXT("Room %s: %d. %s"), ( nmu.fVertical ? TEXT("line") : TEXT("row") ), nmu.vobjX[0].bScr, sObj.GetString());
				}
				else
					sText.Format(TEXT("Place object 0x22 in any room"));
			}
			break;

		}
	}

	dSendMessage(IDC_STATUSBAR, SB_SETTEXT, 1, (LPARAM) sText.GetString());
}

VOID CMainWnd::SetMyIcon(LPCTSTR pszRes, LPCTSTR pszType, int id)
{
	HICON hIco = LoadIcon(GetInstance(), MAKEINTRESOURCE(IDI_ICON1));
	SendMessage(*this, WM_SETICON, ICON_BIG, (LPARAM) hIco);
	SendMessage(*this, WM_SETICON, ICON_SMALL, (LPARAM) hIco);
}

VOID CMainWnd::CheckCommandLine()
{
	int argc = 0;
	LPCTSTR * argv = (LPCTSTR*) CommandLineToArgvW(GetCommandLineW(), &argc);
	if ( argv )
	{
		if ( argc > 1 )
		{
			OnOpenFile(argv[1]);
			if ( !m_File.IsFileLoaded() )
			{
				Close(0xff);	// dont load editor, where load file failed
			}
		}

		LocalFree(argv);
	}	
}