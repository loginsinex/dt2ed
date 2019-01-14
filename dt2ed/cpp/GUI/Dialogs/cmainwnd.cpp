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

CLeftPaneDlg::CLeftPaneDlg(HINSTANCE hInstance)
	: CDialog(hInstance)
{

}

VOID CLeftPaneDlg::OnPreInit(CDialogTemplate & d)
{
	d.SetDlgStyle(DS_SHELLFONT | DS_CONTROL | WS_CHILD | WS_CLIPSIBLINGS, 0);
	d.SetDlgPos(0, 0, 150, 0);

	d.AddPreItem(-1, cStatic, TEXT("File"), 7, 7, -7, 9, SS_LEFT, TRUE, TRUE, FALSE);
	d.AddPreItem(ID_FILE_OPEN_TEXT, TEXT("SysLink"), CString( TEXT("<A>") + GetStr(IDS_PANEL_MAIN_OPEN) + TEXT("</A>") ).GetString(), 7, 25, -7, 10 );
	d.AddPreItem(ID_FILE_SAVE_TEXT, TEXT("SysLink"), CString( TEXT("<A>") + GetStr(IDS_PANEL_MAIN_SAVE) + TEXT("</A>") ).GetString(), 7, 40, -7, 10 );
	d.AddPreItem(-1, cStatic, NULL, 7, 55, -7, 5, SS_ETCHEDHORZ, TRUE, TRUE, FALSE);

	d.AddPreItem(-1, cStatic, TEXT("Layers"), 7, 70, -7, 9, SS_LEFT, TRUE, TRUE, FALSE);
	d.AddPreItem(0x1001, cCombobox, NULL, 7, 85, -7, 100, CBS_DROPDOWNLIST | WS_VSCROLL, TRUE, FALSE, TRUE);
	d.AddPreItem(ID_REMOVEALLSLOTS, TEXT("SysLink"), CString( TEXT("<A>Remove all slots here...</A>") ).GetString(), 7, 100, -7, 10, 0, TRUE, FALSE, TRUE);
	d.AddPreItem(-1, cStatic, NULL, 7, 115, -7, 5, SS_ETCHEDHORZ, TRUE, TRUE, FALSE);

	d.AddPreItem(-1, cStatic, TEXT("Level specific"), 7, 130, -7, 9, SS_LEFT, TRUE, TRUE, FALSE);
	d.AddPreItem(ID_EDIT_CREATENEWLEVEL, TEXT("SysLink"), CString( TEXT("<A>Create new level...</A>") ).GetString(), 7, 145, -7, 10, 0, TRUE, FALSE);
	d.AddPreItem(ID_EDIT_LEVELSETTINGS, TEXT("SysLink"), CString( TEXT("<A>Level settings...</A>") ).GetString(), 7, 160, -7, 10, 0);
	d.AddPreItem(ID_TOOLS_FLUSHOBJECTS, TEXT("SysLink"), CString( TEXT("<A>Flush objects...</A>") ).GetString(), 7, 175, -7, 10, 0);
	d.AddPreItem(ID_EDIT_FIND_TEXT, TEXT("SysLink"), CString( TEXT("<A>Select level...</A>") ).GetString(), 7, 190, -7, 10, 0);
	d.AddPreItem(ID_SHOWDIALOG, TEXT("SysLink"), CString( TEXT("<A>Edit character dialog...</A>") ).GetString(), 7, 205, -7, 10, 0, TRUE, FALSE);
	d.AddPreItem(ID_TOOLS_GRAPHICSEDITOR, TEXT("SysLink"), CString( TEXT("<A>Edit this graphics...</A>") ).GetString(), 7, 220, -7, 10, 0, TRUE, FALSE);
	d.AddPreItem(-1, cStatic, NULL, 7, 235, -7, 5, SS_ETCHEDHORZ, TRUE, TRUE, FALSE);
}

VOID CLeftPaneDlg::OnInit(LPARAM lParam)
{
	CComboBox cbLayer;
	cbLayer.AssignControl(*this, 0x1001);
	cbLayer.AddString(TEXT("Tiles layer"), ID_ACTIVELAYOUT_TILESLAYOUT);
	cbLayer.AddString(TEXT("Rooms layer"), ID_ACTIVELAYOUT_ROOMSLAYOUT);
	cbLayer.AddString(TEXT("Objects layer"), ID_ACTIVELAYOUT_OBJECTSLAYOUT);
	cbLayer.AddString(TEXT("Bridge layer"), ID_ACTIVELAYERBRIDGE);
	cbLayer.AddString(TEXT("Scrooge layer"), ID_ACTIVELAYER_SCROOGELAYER);
	cbLayer.AddString(TEXT("Checkpoint layer"), ID_ACTIVELAYER_CHECKPOINTLAYER);
	cbLayer.AddString(TEXT("Boss respawn layer"), ID_ACTIVELAYER_BOSSRESPAWNLAYER);
	cbLayer.AddString(TEXT("Hidden slots layer"), ID_ACTIVELAYER_HIDDENSLOTSLAYER);
	cbLayer.AddString(TEXT("Mu secret layer"), ID_ACTIVELAYER_MUSECRETLAYER);

	EnableThemeDialogTexture( *this,  ETDT_ENABLE | ETDT_USETABTEXTURE );
}

VOID CLeftPaneDlg::OnCommand(USHORT uCmd, USHORT uId, HWND hCtl)
{
	if ( CBN_SELENDOK == uCmd && 0x1001 == uId )
	{
		CComboBox cbLayer;
		cbLayer.AssignControl(*this, uId);
		USHORT uCmdId = LOWORD(cbLayer.GetParam(cbLayer.GetCurSel()));
		SendMessage(GetParent(*this), WM_COMMAND, MAKELONG( uCmdId, BN_CLICKED ), 0);
	}
}

INT_PTR CLeftPaneDlg::OnNotify(LPNMHDR lpnm)
{
	if ( lpnm->code == NM_CLICK || lpnm->code == NM_RETURN )
	{
		SendMessage(GetParent(*this), WM_COMMAND, MAKELONG( lpnm->idFrom, BN_CLICKED ), 0);
	}
	return 0;
}

VOID CLeftPaneDlg::EnableItem(BOOL fEnable, USHORT uId)
{
	Enable(uId, fEnable);
}

VOID CLeftPaneDlg::EnableLayers(BOOL fEnable)
{
	Enable(0x1001, fEnable);
}

VOID CLeftPaneDlg::SelectLayer(USHORT uId)
{
	CComboBox cbLayer;
	cbLayer.AssignControl(*this, 0x1001);
	UINT count = cbLayer.cSendMessage(CB_GETCOUNT);
	for(UINT i = 0; i < count; ++i)
	{
		if ( LOWORD(cbLayer.GetParam(i)) == uId )
		{
			cbLayer.SetCurSel(i);
			return;
		}
	}
}

UINT CLeftPaneDlg::GetSelectedLayer()
{
	CComboBox cbLayer;
	cbLayer.AssignControl(*this, 0x1001);

	return cbLayer.GetParam(cbLayer.GetCurSel());
}


CMainWnd::CMainWnd(HINSTANCE hInstance) 
	: CDialog(hInstance),
	m_dlgLeftPane(hInstance),
	m_hCustomMenu( NULL ),
	m_Tiles(hInstance, TEXT("BIN"), MAKEINTRESOURCE(IDR_BIN1)),
	m_ScroogeTiles(hInstance, TEXT("BIN"), MAKEINTRESOURCE(IDR_BIN2)),
	m_wCatchedSlot( 0 ),
	m_wCatchedItem( 0xFFFF )
{
	AddAnchor(IDC_TAB1, ANCHOR_FULL);
	AddAnchor(IDC_CANVAS, ANCHOR_FULL);

	MainWindow();
}

void CMainWnd::OnPreInit(CDialogTemplate & d)
{
	d.SetDlgClass(TEXT("DuckTales2Editor"));
	d.SetDlgPos(0, 0, 600, 400);
	d.SetDlgTitle(TEXT("[NES] Duck Tales 2 Level Editor"));

	d.SetDlgStyle(WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_VISIBLE |
		DS_CENTER | DS_SHELLFONT | WS_SIZEBOX | WS_CLIPCHILDREN, 0);

	d.AddPreItem(IDC_REBAR, REBARCLASSNAME, NULL, 0, 0, 0, 0, 
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | RBS_AUTOSIZE | CCS_TOP | CCS_ADJUSTABLE | RBS_BANDBORDERS, TRUE, TRUE, FALSE);
	d.AddPreItem(IDC_STATUSBAR, STATUSCLASSNAME, NULL, 0, 0, 0, 0, 0, TRUE, TRUE, FALSE);
	d.AddPreItem(IDC_TAB1, TOOLBARCLASSNAME, NULL, 0, 0, 0, 0, 
		TBSTYLE_FLAT | CCS_NODIVIDER | CCS_NOPARENTALIGN, TRUE, TRUE, FALSE);
	d.AddPreItem(IDC_CANVAS, TEXT("SysLevel32"), NULL, 0, 0, 0, 0);
}

void CMainWnd::OnInit(LPARAM lParam)
{

	SetMyIcon(MAKEINTRESOURCE(IDB_PNG2), TEXT("PNG"), 4);

	m_hAccel = (HACCEL) lParam;

	m_rb.AssignWindow(Ctl(IDC_REBAR));
	m_tb.AssignWindow(Ctl(IDC_TAB1));

	int iPart[2] = { 400, -1 };
	dSendMessage(IDC_STATUSBAR, SB_SETPARTS, 2, (LPARAM) iPart);

	HMENU hMenu = LoadMenu(GetInstance(), MAKEINTRESOURCE( IDR_MENU1 ));
	SetMenu( *this, hMenu );
	m_pMenu = new CMenuWrapper( hMenu );

	DeleteObject( 
		m_tb.SetImageList( LoadBitmap(GetInstance(), MAKEINTRESOURCE(IDB_BITMAP1)) )
		);

	m_tb.AddButton(43, ID_FILE_OPEN_TEXT);
	m_tb.AddButton(13, ID_FILE_SAVE_TEXT, NULL, 0, FALSE);
	m_tb.AddButton(1, ID_RUN, NULL, 0, FALSE);
	m_tb.AddSeparator();
	m_tb.AddButton(25, ID_EDIT_CREATENEWLEVEL, NULL, 0, FALSE);
	m_tb.AddButton(10, ID_TOOLS_FLUSHOBJECTS, NULL, 0, FALSE);
	m_tb.AddButton(3, ID_EDIT_LEVELSETTINGS, NULL, 0, FALSE);
	m_tb.AddButton(4, ID_EDIT_FIND_TEXT, NULL, 0, FALSE);
	m_tb.AddSeparator();
	m_tb.AddButton(20, ID_EDIT_BLOCKSEDITOR, NULL, 0, FALSE);
	m_tb.AddButton(21, ID_EDIT_TILESEDITOR, NULL, 0, FALSE);
	m_tb.AddButton(29, ID_BLOCKSANDTILES_TILESBANK, NULL, 0, FALSE);
	m_tb.AddButton(35, ID_TOOLS_GRAPHICSEDITOR, NULL, 0, FALSE);
	m_tb.AddSeparator();
	m_tb.AddButton(42, ID_EDIT_CONVERTTOUOROM, NULL, 0, FALSE);
	m_tb.AddButton(22, ID_EDIT_TEXTEDITOR, NULL, 0, FALSE);
	m_tb.AddButton(23, ID_EDIT_REPLACE_TEXT, NULL, 0, FALSE);
	m_tb.AddButton(33, ID_EDIT_GAMESETTINGS, NULL, 0, FALSE);
	m_tb.AddSeparator();
//	m_tb.AddButton(10, ID_FILE_QUIT);
	m_tb.AddButton(16, ID_HELP_ABOUT);

	OnInitPane();

	OnRebarHeightChange();

	m_rb.InsertBand(m_tb, 0, TRUE);

	CBitmap bg;
	bg.Create(1, 1536);
	bg.GradientFill(0, 0, bg.Width(), bg.Height(), RGB(0x40, 0x40, 0x40), RGB(0x50, 0x50, 0x50), GRADIENT_FILL_RECT_V);
	m_Canvas.SetBg(bg);

	m_Canvas.InitBufView((CBufView*) dSendMessage(IDC_CANVAS, BVM_GETINSTANCE), 2048, 1536);
	m_Canvas.Update();

	CheckCommandLine();
}

void CMainWnd::PreOnSize(UINT fwSizeType, SHORT cx, SHORT cy)
{
	dSendMessage(IDC_STATUSBAR, WM_SIZE);
	dSendMessage(IDC_REBAR, WM_SIZE);
}

void CMainWnd::PreOnSizing(UINT fwSizeType, LPRECT lprc)
{
	PreOnSize(fwSizeType, LOWORD(lprc->right), LOWORD(lprc->bottom));
}


void CMainWnd::OnCommand(USHORT uCmd, USHORT uId, HWND hCtl)
{
	if ( 1 == uCmd ) /* from accelerator */
		OnButton(uId);
}


INT_PTR CMainWnd::OnNotify(LPNMHDR lpnm)
{
	switch(lpnm->idFrom)
	{
	case IDC_REBAR:
		{
			switch(lpnm->code)
			{
			case RBN_HEIGHTCHANGE: OnRebarHeightChange(); break;
			}
			break;
		}
	case IDC_CANVAS:
		{
			switch(lpnm->code)
			{
			case CVN_REQUESTPOINT:
				{
					PCVNREQPOS p = (PCVNREQPOS) lpnm;
					INT_PTR ret = 0;
					switch(p->id)
					{
					case 0: ret = ReqTilePos(p); break;
					case 1: ret = ReqRoomPos(p); break;
					case 2: ret = ReqObjtPos(p); break;
					case 8:
					case 3:
					case 4: ret = ReqScroogePos(p); break;
					case 5: ret = ReqSlotPos(p); break;
					case 6: ret = ReqBrdgPos(p); break;
					case 7: ret = ReqMscrPos(p); break;
					default: ret = 0; break;
					}

					if ( !ret )
						ShowStat(&p->pt);

					return ret;
				}
			case CVN_REQUESTMODIFY:
				{
					PCVNREQMODIFY p = (PCVNREQMODIFY) lpnm;
					switch(p->id)
					{
					case 0: ReqTileModify(p); break;
					case 1: ReqRoomModify(p); break;
					case 2: ReqObjtModify(p); break;
					case 5: ReqSlotModify(p); break;
					case 6: ReqBrdgModify(p); break;
					case 7: ReqMscrModify(p); break;
					}

					ShowStat();
					break;
				}
			}
		}
	}

	return 0;
}


void CMainWnd::OnDestroy()
{
	DestroyIcon((HICON) SendMessage(*this, WM_GETICON, ICON_BIG, 0));
	delete m_pMenu;
}

INT CMainWnd::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & pHandle)
{
	switch(uMsg)
	{
	case WM_SIZE:
		{
			PreOnSize(wParam, LOWORD(lParam), HIWORD(lParam));
			break;
		}
	case WM_SIZING:
		{
			PreOnSizing(wParam, LPRECT(lParam));
			break;
		}
	}
	return 0;
}

void CMainWnd::OnCancel()
{
	return;
}

BOOL CMainWnd::OnClose(INT & iRetCode)
{
	if ( m_File.IsFileLoaded() )
	{
		if ( !Confirm(TEXT("Do you realy want to close the editor?")) )
			return FALSE;
	}

	return TRUE;
}