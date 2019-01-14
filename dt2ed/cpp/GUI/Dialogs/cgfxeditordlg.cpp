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

CGfxEditorDlg::CGfxEditorDlg(HINSTANCE hInstance)
	: CDialog(hInstance), m_fExtended( FALSE )
{

}

VOID CGfxEditorDlg::OnPreInit(CDialogTemplate & d)
{
	d.SetDlgStyle(DS_SHELLFONT | WS_CAPTION | WS_BORDER | DS_MODALFRAME | WS_SYSMENU, 0);
	d.SetDlgTitle(TEXT("Objects graphics editor"));
	d.SetDlgPos(0, 0, 370, 200);

	d.AddPreItem(-1, cButton, NULL, 7, 7, 110, 158, BS_GROUPBOX, TRUE, TRUE, FALSE);
	d.AddPreItem(0x1010, cCombobox, NULL, 15, 7, 75, 100, CBS_DROPDOWNLIST | WS_VSCROLL);
	d.AddPreItem(0x1011, TEXT("SysLevel32"), NULL, 15, 23, 94, 94, WS_BORDER);
	d.AddPreItem(0x1012, cCombobox, NULL, 15, 122, 94, 100, CBS_DROPDOWNLIST | WS_VSCROLL);
	d.AddPreItem(0x1013, cEdit, NULL, 15, 139, 94, 22, ES_READONLY | ES_AUTOHSCROLL | ES_MULTILINE);

	d.AddPreItem(-1, cButton, TEXT("Object view"), 125, 7, -7, 158, BS_GROUPBOX, TRUE, TRUE, FALSE);
	d.AddPreItem(0x1032, TEXT("SysLevel32"), NULL, 133, 15, -15, 125, WS_BORDER);
	d.AddPreItem(0x1031, cEdit, NULL, 133, 143, -15, 10, ES_READONLY | ES_AUTOHSCROLL);

	if ( m_fExtended )
	{
		d.AddPreItem(0x1050, cButton, TEXT("Create new..."), 7, -(7 + 15), 60, 15, BS_PUSHBUTTON);
		d.AddPreItem(0x1051, cButton, TEXT("Add obj..."), 7 + 60 + 7, -(7 + 15), 50, 15, BS_PUSHBUTTON);
		d.AddPreItem(0x1052, cButton, TEXT("Del obj..."), 7 + 60 + 7 + 50 + 7, -(7 + 15), 50, 15, BS_PUSHBUTTON);
	}

	d.AddPreItem(IDOK, cButton, MB_GetString(IDOK - 1), -(7 + 50 + 7 + 50), -(7 + 15), 50, 15, BS_DEFPUSHBUTTON);
	d.AddPreItem(IDCANCEL, cButton, MB_GetString(IDCANCEL- 1), -(7 + 50), -(7 + 15), 50, 15, BS_PUSHBUTTON);
}

VOID CGfxEditorDlg::OnInit(LPARAM lParam)
{
	m_cbSet.AssignWindow(Ctl(0x1010));
	m_cbSprId.AssignWindow(Ctl(0x1012));

	m_Tiles.InitBufView((CBufView*) dSendMessage(0x1011, BVM_GETINSTANCE), 130, 130);
	m_Sprite.InitBufView((CBufView*) dSendMessage(0x1032, BVM_GETINSTANCE), 130, 130);
	m_Sprite.SetCursorSet(0, RGB(255, 255, 255), 130, 130, 130, 130);
	m_Sprite.SetActiveCursor(0);

	RECT rc = { 0 };
	GetClientRect(m_Tiles, &rc);
	FillRect(m_Tiles, &rc, GetStockBrush(GRAY_BRUSH));

	for(int i = 0; i < NES_COUNT_LEVELS * 8; ++i)
	{
		CString sSet, sLst;
		sSet.Format(TEXT("Lvl: %d, Set #%d"), i / 8 + 1, i % 8);
		m_cbSet.AddString(sSet, i);
	}

	m_cbSet.SetCurSel(8 * m_info.lvl + m_info.set);

	UpdateInfo();
}

VOID CGfxEditorDlg::OnOK()
{
	Close(1);
}

INT_PTR CGfxEditorDlg::OnNotify(LPNMHDR lpnm)
{
	switch(lpnm->idFrom)
	{
	case 0x1032:
		{
			switch(lpnm->code)
			{
			case CVN_REQUESTPOINT: return ReqSprPos((PCVNREQPOS) lpnm);
			}
		}
	}

	return 0;
}

INT CGfxEditorDlg::ReqSprPos(PCVNREQPOS p)
{
	if ( p->pt.y != 0 || p->pt.x < 0 || p->pt.x >= int(m_info.vSet.size()) )
		return 1;

	if ( !m_cbSprId.cSendMessage(CB_GETCOUNT) )
		return 1;


	DWORD cfg = m_info.vSet[p->pt.x];
	BYTE bList = HIBYTE(cfg), bCfg = LOBYTE(cfg);

	CString str;
	str.Format(TEXT("List: %d, Sprite: #%02X"), bList, bCfg * 2);
	SetCtlText(0x1031, str);

	CBitmap bmt, bmHl;
	bmt.Create(m_bmHlTiles.Width(), m_bmHlTiles.Height());
	BitBlt(bmt.LockDC(), 0, 0, bmt.Width(), bmt.Height(), m_bmHlTiles.LockDC(), 0, 0, SRCCOPY);
	bmt.FreeDC(); m_bmHlTiles.FreeDC();

	bmHl.Create(8, 8);
	bmHl.FillRect(0, 0, bmHl.Width(), bmHl.Height(), RGB(0xff, 0, 0), 0, 0, 100);
	WRGBQUAD * pw = bmHl.LockRawBits();
	int cx = bmHl.Width(), cy = bmHl.Height();

	std::vector<NES_SPRITE> sprl;
	m_gfx.Get_Sprite(m_vSprLists[bList][bCfg], sprl);
	size_t count = sprl.size();

	for(size_t n = 0; n < count; ++n)
	{
		NES_SPRITE & spr = sprl[n];
		for(int i = 0; i < int(spr.bCount); ++i)
		{
			int tile = spr.vTiles[i].bPPUAddr;
			bmt.RenderRawBitmap(pw, cx, cy, 255, TRUE, 0, 0, cx, cy, 8 * (tile % 16), 8 * (tile / 16), 0);
		}
	}

	BitBlt(m_Tiles, 0, 0, bmt.Width(), bmt.Height(), bmt.LockDC(), 0, 0, SRCCOPY);
	bmt.FreeDC();
	m_Tiles.Update();

	return 0;
}

VOID CGfxEditorDlg::Let_Editor(INT level, INT set, CNesGraphics & gfx, PBYTE pbPal, BOOL fExtended)
{
	m_cLevel = level;
	m_info.lvl = level;
	m_info.set = set;
	m_fExtended = fExtended;
	m_gfx = gfx;
	m_vSprLists.clear();
	m_gfx.Get_SprList(m_vSprLists);
	CopyMemory(m_bPal, pbPal, sizeof(m_bPal));
}

VOID CGfxEditorDlg::Get_Editor(CNesGraphics & gfx)
{
	gfx = m_gfx;
}

VOID CGfxEditorDlg::UpdateInfo()
{
	int set = m_cbSet.GetParam(m_cbSet.GetCurSel()), lvl = set / 8;
	set %= 8;

	if ( set < 0 || set >= 8 )
		return;

	m_info.vSprInf.clear();
	m_info.vTiles.clear();
	m_info.lvl = lvl;
	m_info.set = set;

	m_gfx.Get_Gfx(lvl, set, m_info.vSprInf, m_info.vTiles, m_info.fLink);

	if ( m_info.fLink )
		SetCtlText(0x1050, TEXT("Create new..."));
	else
		SetCtlText(0x1050, TEXT("Merge with..."));

	m_cbSprId.cSendMessage(CB_RESETCONTENT);

	size_t count = m_info.vSprInf.size();
	for(size_t n = 0; n < count; ++n)
	{
		CString str;
		str.Format(TEXT("Object #%d"), n + 1);
		m_cbSprId.AddString(str, n);
	}

	m_cbSprId.SetCurSel(0);
	UpdateTiles();
	UpdateSpriteInfo();
}

VOID CGfxEditorDlg::UpdateSpriteInfo()
{
	m_Sprite.ResizeBufView(1, 1);

	int spr = m_cbSprId.GetParam(m_cbSprId.GetCurSel());
	if ( spr < 0 || spr >= int(m_info.vSprInf.size()) || !m_cbSprId.cSendMessage(CB_GETCOUNT) )
		return;

	int set = m_cbSet.GetParam(m_cbSet.GetCurSel()), lvl = set / 8;
	set %= 8;

	if ( set < 0 || set >= 8 )
		return;

	m_info.obj = spr;

	// find suitable sprites from list

	NES_SPRITE_INFO & sprinf = m_info.vSprInf[spr];

	// update tiles info
	CString sTilesInf;
	sTilesInf.Format(TEXT("Begin tile: #%02X. Count: %d\r\nStream: #%d"), sprinf.bPPUAddr, sprinf.nLength, sprinf.nStream);
	SetCtlText(0x1013, sTilesInf);

	// hilite tiles
	CBitmap & bm = m_bmHlTiles;
	CBitmap bmhl;
	bm.Create(m_bmTiles.Width(), m_bmTiles.Height());
	bmhl.Create(8, 8);
	bmhl.FillRect(0, 0, bmhl.Width(), bmhl.Height(), RGB(0, 0xff, 0), 0, 0, 100);
	
	BitBlt(bm.LockDC(), 0, 0, bm.Width(), bm.Height(), m_bmTiles.LockDC(), 0, 0, SRCCOPY);
	bm.FreeDC(); m_bmTiles.FreeDC();

	// do hilite
	int istart = sprinf.bPPUAddr, iend = istart + sprinf.nLength,
		cx = bmhl.Width(), cy = bmhl.Height();

	WRGBQUAD * pw = bmhl.LockRawBits();
	for(int i = istart; i < iend; ++i)
		bm.RenderRawBitmap(pw, cx, cy, 255, TRUE, 0, 0, cx, cy, 8 * ( i % 16 ), 8 * ( i / 16 ), 0);
	
	BitBlt(m_Tiles, 0, 0, bm.Width(), bm.Height(), bm.LockDC(), 0, 0, SRCCOPY);
	m_Tiles.Update();

	std::vector<DWORD> vBestCfgs;
	if ( m_gfx.GetBestCfgForObject(lvl, set, spr, vBestCfgs) )
	{
		size_t count = vBestCfgs.size();
		m_info.vSet = vBestCfgs;

		m_Sprite.ResizeBufView(130 * count, 130);

		for(size_t n = 0; n < count; ++n)
		{
			BYTE bCfg = LOBYTE(vBestCfgs[n]), bList = HIBYTE(vBestCfgs[n]);

			CBitmap bm;
			bm.Create(130, 130);
			bm.FillRect(0, 0, bm.Width(), bm.Height(), RGB(0x20, 0x20, 0x20), 0, 1);

			HDC hdc = bm.LockDC();
			m_gfx.DrawSprite(hdc, 64, 64, m_info.vTiles, m_vSprLists[bList][bCfg], m_bPal);

			BitBlt(m_Sprite, 130 * n, 0, bm.Width(), bm.Height(), hdc, 0, 0, SRCCOPY);
		}
		m_Sprite.Update();
		m_Sprite.SetCursorPosition(0, 0);
	}
}

VOID CGfxEditorDlg::UpdateTiles()
{
	CBitmap & bm = m_bmTiles;
	bm.Create(128, 128);
	bm.FillRect(0, 0, bm.Width(), bm.Height(), RGB(0x80, 0x80, 0x80), 0, 0);

	COLORREF clr[4] = { 0, 0x808080, 0xA0A0A0, 0xFFFFFF };
	COLORREF * pclr = (COLORREF*) bm.LockRawBits();

	for(int by = 0; by < 16; ++by)
	{
		for(int bx = 0; bx < 16; ++bx)
		{
			NES_TILE & tile = m_info.vTiles[bx + 16 * by];

			for(int ty = 0; ty < 8; ++ty)
			{
				for(int tx = 0; tx < 8; ++tx)
				{
					pclr[(8 * bx + tx) + 128 * (8 * by + ty)] = clr[tile.pix[tx][ty].clr];
				}
			}
		}
	}

	BitBlt(m_Tiles, 0, 0, bm.Width(), bm.Height(), bm.LockDC(), 0, 0, SRCCOPY);
	bm.FreeDC();
	m_Tiles.Update();
}

VOID CGfxEditorDlg::OnCommand(USHORT uCmd, USHORT uId, HWND hCtl)
{
	if ( CBN_SELENDOK == uCmd )
	{
		switch(uId)
		{
		case 0x1010: UpdateInfo(); break;
		case 0x1012: UpdateSpriteInfo(); break;
		}
	}
}

VOID CGfxEditorDlg::OnButton(USHORT uId)
{
	if ( !m_fExtended )
		return;

	switch(uId)
	{
	case 0x1050:
		{
			if ( m_info.fLink )
			{
				if ( Confirm(TEXT("Are you sure that you want to create new set instead of this link?")) )
				{
					if ( m_gfx.CreateGfx(m_info.lvl, m_info.set) )
						UpdateInfo();
					else
						ShowError(TEXT("Failed to create new set"));
				}
			}
			else
			{
				CGfxMergeDlg dlg(GetInstance(), m_info.lvl, m_info.set);
				if ( dlg.Show(this) )
				{
					INT lvl = 0, set = 0;
					dlg.Get_Info(lvl, set);

					if ( m_gfx.MergeWith(m_info.lvl, m_info.set, lvl, set) )
						UpdateInfo();
					else
						ShowError(TEXT("Merge failed"));
				}
			}
			break;
		}
	case 0x1051:
		{
			std::vector<BYTE> vSets;
			if ( m_gfx.GetAvailableSets(m_info.lvl, m_info.set, vSets) )
			{
				CGfxInsertSet dlg(GetInstance(), vSets, m_gfx, m_bPal);
				if ( dlg.Show(this) )
				{
					int setid = 0;
					dlg.Get_SetId(setid);
					if ( m_gfx.GfxAddObject(m_info.lvl, m_info.set, setid) )
						UpdateInfo();
					else
						ShowError(TEXT("Failed to add predefined object"));
				}
			}
			else
				ShowError(TEXT("No free space for predefined objects"));
			break;
		}
	case 0x1052:
		{
			if ( Confirm(TEXT("Are you sure that you want to delete selected object from this graphics object?")) )
			{
				if ( m_gfx.GfxDelObject(m_info.lvl, m_info.set, m_info.obj) )
					UpdateInfo();
				else
					ShowError(TEXT("Unable to delete this object."));
			}
			break;
		}
	}
}


// --- merger


CGfxMergeDlg::CGfxMergeDlg(HINSTANCE hInstance, int level, int set)
	: CDialog(hInstance)
{
	m_info.level = level;
	m_info.set = set;
}
	
VOID CGfxMergeDlg::Get_Info(int & level, int & set)
{
	level = m_info.level;
	set = m_info.set;
}

VOID CGfxMergeDlg::OnInit(LPARAM lParam)
{
	m_cbSet.AssignWindow(Ctl(0x1010));

	for(int lev = 0; lev < NES_COUNT_LEVELS; ++lev)
	{
		for(int set = 0; set < 8; ++set)
		{
			if ( lev == m_info.level && set == m_info.set )
				continue;

			CString str;
			str.Format(TEXT("Level: %d, Set: %d"), lev + 1, set);
			m_cbSet.AddString(str, MAKEWORD(set, lev));
		}
	}

	m_cbSet.SetCurSel(0);
}

VOID CGfxMergeDlg::OnPreInit(CDialogTemplate & d)
{
	d.SetDlgPos(0, 0, 170, 80);
	d.SetDlgTitle(TEXT("Merge this graphics object with..."));
	
	d.AddPreItem(-1, cButton, TEXT("Select a level and set"), 7, 7, -7, -(7 + 15 + 7), BS_GROUPBOX);
	d.AddPreItem(-1, cStatic, TEXT("Objects set:"), 15, 27, 50, 10, SS_RIGHT, TRUE, TRUE, FALSE);
	d.AddPreItem(0x1010, cCombobox, NULL, 70, 25, -15, 100, CBS_DROPDOWNLIST | WS_VSCROLL);

	d.AddPreItem(IDOK, cButton, MB_GetString(IDOK - 1), -(7 + 50 + 7 + 50), -(7 + 15), 50, 15, BS_DEFPUSHBUTTON);
	d.AddPreItem(IDCANCEL, cButton, MB_GetString(IDCANCEL - 1), -(7 + 50), -(7 + 15), 50, 15, BS_PUSHBUTTON);
}

VOID CGfxMergeDlg::OnOK()
{
	int dat = m_cbSet.GetParam(m_cbSet.GetCurSel());
	m_info.level = HIBYTE(dat);
	m_info.set = LOBYTE(dat);
	Close(1);
}

// -----------
CGfxInsertSet::CGfxInsertSet(HINSTANCE hInstance, std::vector<BYTE> & vSets, CNesGraphics & gfx, PBYTE pbPal)
	: CDialog(hInstance), m_gfx(gfx), m_vSets(vSets), m_setid(0)
{
	m_gfx.Get_SprList(m_vSprLists);
	CopyMemory(m_bPal, pbPal, sizeof(m_bPal));
}

VOID CGfxInsertSet::OnInit(LPARAM lParam)
{
	m_cbSet.AssignWindow(Ctl(0x1010));

	m_SetView.InitBufView((CBufView*) dSendMessage(0x1011, BVM_GETINSTANCE), 10, 10);
	m_SetView.SetCursorSet(0, RGB(255, 255, 255), 130, 130, 130, 130);
	m_SetView.SetActiveCursor(0);

	size_t count = m_vSets.size();
	for(size_t n = 0; n < count; ++n)
	{
		CString str;
		str.Format(TEXT("Stream #%d"), m_vSets[n]);
		m_cbSet.AddString(str, n);
	}

	m_cbSet.SetCurSel(0);
	UpdateView();
}

VOID CGfxInsertSet::OnPreInit(CDialogTemplate & d)
{
	d.SetDlgPos(0, 0, 200, 200);
	d.SetDlgTitle(TEXT("Select a stream"));

	d.AddPreItem(-1, cButton, NULL, 7, 7, -7, -(7 + 15 + 7), BS_GROUPBOX);
	d.AddPreItem(-1, cStatic, TEXT("Stream:"), 15, 27, 50, 10, SS_RIGHT, TRUE, TRUE, FALSE);
	d.AddPreItem(0x1010, cCombobox, NULL, 70, 25, -15, 100, CBS_DROPDOWNLIST | WS_VSCROLL);
	d.AddPreItem(0x1011, TEXT("SysLevel32"), NULL, 15, 45, -15, -(7 + 15 + 7 + 15), WS_BORDER);

	d.AddPreItem(IDOK, cButton, MB_GetString(IDOK - 1), -(7 + 50 + 7 + 50), -(7 + 15), 50, 15, BS_DEFPUSHBUTTON);
	d.AddPreItem(IDCANCEL, cButton, MB_GetString(IDCANCEL - 1), -(7 + 50), -(7 + 15), 50, 15, BS_PUSHBUTTON);
}

VOID CGfxInsertSet::OnOK()
{
	m_setid = m_cbSet.GetParam(m_cbSet.GetCurSel());
	Close(1);
}

VOID CGfxInsertSet::OnCommand(USHORT uCmd, USHORT uId, HWND hCtl)
{
	if ( CBN_SELENDOK == uCmd && 0x1010 == uId )
	{
		m_setid = m_cbSet.GetParam(m_cbSet.GetCurSel());
		UpdateView();
	}
}

VOID CGfxInsertSet::UpdateView()
{
	std::vector<NES_SPRITE_INFO> vSprInf;
	std::vector<NES_TILE> vTiles;
	std::vector<std::vector<DWORD>> vObjects;

	int setid = m_setid; //m_cbSet.GetParam(m_cbSet.GetCurSel());
	if ( setid < 0 || setid >= int( m_vSets.size() ) )
		return;

	setid = m_vSets[setid];

	BOOL fLink = FALSE;
	m_SetView.SetCursorPosition(0, 0);

	if ( !m_gfx.Get_Tiles(setid, vTiles) )
		return;
	
	// get max spr info
	std::vector<DWORD> vCfgs;
	m_gfx.Get_SetObjects(setid, vCfgs);
	size_t smax = vCfgs.size();

	m_SetView.ResizeBufView(130 * smax, 130);

	for(size_t n = 0; n < smax; n++)
	{
		CBitmap bm;
		bm.Create(130, 130);
		bm.FillRect(0, 0, bm.Width(), bm.Height(), RGB(0x20, 0x20, 0x20), 0, 1);
		BYTE bList = HIBYTE(vCfgs[n]), bSpr = LOBYTE(vCfgs[n]);

		HDC hdc = bm.LockDC();
		m_gfx.DrawSprite(hdc, 1 + (bm.Width() - 2) / 2, 1 + (bm.Height() - 2) / 2, vTiles, m_vSprLists[bList][bSpr], m_bPal);

		BitBlt(m_SetView, bm.Width() * n, 0, bm.Width(), bm.Height(), hdc, 0, 0, SRCCOPY);
		bm.FreeDC();
	}

	m_SetView.Update();
}


VOID CGfxInsertSet::Get_SetId(INT & setid)
{
	setid = m_vSets[m_setid];
}
