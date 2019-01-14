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

CObjectInsert::CObjectInsert(HINSTANCE hInstance, CTilesCreator & tiles, CNesGraphics & gfx, PBYTE pbPal, BYTE bBossObject)
	: m_tiles(tiles), m_hInstance(hInstance), m_gfx( gfx ), m_bBossObject( bBossObject )
{
	CopyMemory(m_bPal, pbPal, sizeof(m_bPal));
}

VOID CObjectInsert::Let_Object(BYTE bObjId)
{
	m_bObjId = bObjId;
}

VOID CObjectInsert::Get_Object(BYTE & bObjId)
{
	bObjId = m_bObjId;
}


BOOL CObjectInsert::Show(CDialog * pParent)
{
	CObjectInsertDlg dlg1(m_hInstance, *this, m_tiles, m_bBossObject);
	CSpecObjectInsertDlg dlg2(m_hInstance, *this, m_gfx, m_bPal);

	CPropSheet PropSheet(m_hInstance);
	PropSheet.FApply(FALSE);
	PropSheet.AddSheet(&dlg1);
	PropSheet.AddSheet(&dlg2);

	return PropSheet.Show(pParent, 0, TEXT("Objects list"));
}


CObjectInsertDlg::CObjectInsertDlg(HINSTANCE hInstance, CObjectInsert & parent, CTilesCreator & tiles, BYTE bBossObject)
	: CDialog(hInstance), m_Parent(parent), m_tiles(tiles), m_objId(0), m_bBossObject( bBossObject )
{

}

VOID CObjectInsertDlg::OnPreInit(CDialogTemplate & d)
{
	d.SetDlgTitle(TEXT("Objects"));
	d.SetDlgPos(0, 0, 400, 300);

	d.AddPreItem(0x1010, WC_LISTVIEW, NULL, 7, 7, -7, -(7 + 15 + 7), WS_BORDER | LVS_ICON | LVS_SINGLESEL);
	d.AddPreItem(0x1011, cStatic, NULL, 7, -(7 + 15), -7, 15, SS_SUNKEN);
}

VOID CObjectInsertDlg::OnInit(LPARAM lParam)
{
	CListView lv;
	lv.AssignWindow(Ctl(0x1010));

	HIMAGELIST himl = ImageList_Create(128, 80, ILC_COLOR32, 0x99, 0);

	CBitmap * pbm[0x100] = { NULL };
	std::map<INT, INT> ids;
	int c = 0;
	if ( m_tiles.LoadTiles(RGB(0xFA, 0xFB, 0xFC), pbm) )
	{
		for(int x = 0; x < sizeof(pbm) / sizeof(pbm[0]); ++x)
		{
			if ( pbm[x] || x == 0x5e )
			{
				int cobj = x;

				if ( cobj == 0x5e )
					cobj = m_bBossObject;

				CBitmap & bm = *pbm[cobj];
				CBitmap bmNew;
				bmNew.Create(128, 80);
				BitBlt(bmNew.LockDC(), 0, 0, bmNew.Width(), bmNew.Height(), bm.LockDC(), 0, 0, SRCCOPY);
				bmNew.FreeDC(); bm.FreeDC();

				HBITMAP hbm = bmNew.ConvertToHandle();
				ImageList_AddMasked(himl, hbm, RGB(0xFA, 0xFB, 0xFC));
				DeleteObject(hbm);
				
				if ( x != 0x5e )
					delete pbm[x];

				ids[x] = (c++);
			}
		}
	}
	else
		return;

	ListView_SetBkColor(lv, RGB(0xA0, 0xA0, 0xA0));

	ListView_SetImageList(lv, himl, LVSIL_NORMAL);

	for(int x = 0; x < sizeof(pbm) / sizeof(pbm[0]); ++x)
	{
		if ( pbm[x] || x == 0x5e )
		{
			CString s;
			s.Format(TEXT("Object #%02X"), x);
			int id = lv.AddItem( x == 0x5e ? TEXT("Level boss") : s, x);
			LV_ITEM lvi = { 0 };
			lvi.iItem = id;
			lvi.iImage = ids[x == 0x5e ? m_bBossObject : x];
			lvi.mask = LVIF_IMAGE;
			ListView_SetItem(lv, &lvi);
			pbm[x] = NULL;
		}
	}

	lv.CurSel(0);
}

INT CObjectInsertDlg::OnNotify(LPNMHDR lpnm)
{
	if ( LVN_ITEMCHANGED == lpnm->code && 0x1010 == lpnm->idFrom )
	{
		CListView lv;
		lv.AssignWindow(Ctl(0x1010));
		
		int id = lv.CurSel();
		if ( -1 != id )
		{
			m_objId = LOBYTE(lv.Param(id));
			m_Parent.Let_Object(m_objId);
		}
	}
	return 0;
}

INT CObjectInsertDlg::PSOnSetActive()
{
	m_Parent.Let_Object(m_objId);
	return 0;
}

CSpecObjectInsertDlg::CSpecObjectInsertDlg(HINSTANCE hInstance, CObjectInsert & parent, CNesGraphics & gfx, PBYTE pbPal)
	: CDialog(hInstance), m_Parent(parent), m_objId(0), m_gfx( gfx )
{
	CopyMemory(m_bPal, pbPal, sizeof(m_bPal));
	m_gfx.Get_SprList(m_info.vSprLists);
}

VOID CSpecObjectInsertDlg::OnPreInit(CDialogTemplate & d)
{
	d.SetDlgTitle(TEXT("Special objects"));
	d.SetDlgPos(0, 0, 400, 300);

	d.AddPreItem(-1, cButton, NULL, 7, 7, -7, 80, BS_GROUPBOX, TRUE, TRUE, FALSE);
	d.AddPreItem(0x1010, cButton, TEXT("Palette"), 15, 7, 80, 10, BS_AUTORADIOBUTTON);
	d.AddPreItem(-1, cButton, NULL, 7, 100, -7, 130, BS_GROUPBOX, TRUE, TRUE, FALSE);
	d.AddPreItem(0x1011, cButton, TEXT("Graphic loader"), 15, 100, 80, 10, BS_AUTORADIOBUTTON);

	d.AddPreItem(-1, cStatic, TEXT("Palette template:"), 15, 25, 90, 10, SS_RIGHT, TRUE, TRUE, FALSE);
	d.AddPreItem(0x1020, cEdit, NULL, 125, 25, 40, 13, ES_AUTOHSCROLL | WS_BORDER);
	d.AddPreItem(0x1021, UPDOWN_CLASS, NULL, 0, 0, 0, 0, 
		UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_AUTOBUDDY | UDS_SETBUDDYINT | UDS_WRAP, TRUE, TRUE, FALSE);

	d.AddPreItem(-1, cStatic, TEXT("Level set:"), 15, 125, 50, 10, SS_RIGHT, TRUE, TRUE, FALSE);
	d.AddPreItem(-1, cStatic, TEXT("Template:"), 15, 145, 50, 10, SS_RIGHT, TRUE, TRUE, FALSE);

	d.AddPreItem(0x1030, cCombobox, NULL, 70, 125, 50, 150, CBS_DROPDOWNLIST | WS_VSCROLL);
	d.AddPreItem(0x1031, cEdit, NULL, 70, 145, 50, 13, ES_AUTOHSCROLL | WS_BORDER);
	d.AddPreItem(0x1032, UPDOWN_CLASS, NULL, 0, 0, 0, 0, 
		 UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_AUTOBUDDY | UDS_SETBUDDYINT | UDS_WRAP, TRUE, TRUE, FALSE);

	d.AddPreItem(0x1040, TEXT("SysLevel32"), NULL, 130, 120, -15, 100, WS_BORDER);
}

VOID CSpecObjectInsertDlg::OnInit(LPARAM lParam)
{
	m_SetView.InitBufView( (CBufView*) dSendMessage(0x1040, BVM_GETINSTANCE), 16, 16);
	m_SetView.SetCursorSet(0, RGB(255, 255, 255), 130, 130, 130, 130);
	m_SetView.SetActiveCursor(0);

	SPINRANGE(0x1021, 3, 0);
	SPINRANGE(0x1032, 7, 0);
	SPINSET(0x1021, 0);
	SPINSET(0x1032, 0);

	CComboBox cb;
	cb.AssignWindow(Ctl(0x1030));
	for(int i = 0; i < NES_COUNT_LEVELS; ++i)
	{
		CString s;
		s.Format(TEXT("Level #%d"), i + 1);
		cb.AddString(s);
	}

	cb.SetCurSel(0);
	CheckAll(TRUE, 0x1010);
	OnCommand(EN_CHANGE, 0x1020, 0);
	OnButton(0x1010);
	UpdateGraphics();
}

INT CSpecObjectInsertDlg::PSOnSetActive()
{
	m_Parent.Let_Object(m_objId);
	return 0;
}

VOID CSpecObjectInsertDlg::OnCommand(USHORT uCmd, USHORT uId, HWND hCtl)
{
	if ( Checked(0x1010) )
	{
		if ( EN_CHANGE == uCmd && 0x1020 == uId )
		{
			if ( !SPINERR(0x1021) )
			{
				m_objId = 0x99 + LOBYTE(SPINVAL(0x1021));
				m_Parent.Let_Object(m_objId);				
			}
		}
	}
	else if ( Checked(0x1011) )
	{
		if ( CBN_SELENDOK == uCmd && 0x1030 == uId || EN_CHANGE == uCmd && 0x1031 == uId )
		{
			CComboBox cbLev;
			cbLev.AssignWindow(Ctl(0x1030));

			INT lev = cbLev.GetCurSel();
			if ( lev >= 0 && lev < NES_COUNT_LEVELS )
			{
				if ( !SPINERR(0x1032) )
				{
					m_objId = 0x9d + LOBYTE(8 * lev + SPINVAL(0x1032));
					m_Parent.Let_Object(m_objId);
					UpdateGraphics();
				}
			}
		}
	}
}

VOID CSpecObjectInsertDlg::OnButton(USHORT uId)
{
	if ( 0x1010 == uId || 0x1011 == uId )
	{
		OnCommand(EN_CHANGE, 0x1020, 0);
		OnCommand(EN_CHANGE, 0x1031, 0);

		EnableAll(Checked(0x1010), 0x1020, 0x1021, 0);
		EnableAll(Checked(0x1011), 0x1030, 0x1031, 0x1032, 0x1040, 0);
	}
}

INT_PTR CSpecObjectInsertDlg::OnNotify(LPNMHDR lpnm)
{
	if ( CVN_REQUESTPOINT == lpnm->code && 0x1040 == lpnm->idFrom )
	{
		PCVNREQPOS p = (PCVNREQPOS) lpnm;
		if ( p->pt.x < 0 || p->pt.y >= int(m_info.vObjInfo.size()) || p->pt.y < 0 )
			return 1;

		if ( p->pt.x >= int(m_info.vObjInfo[p->pt.y]) )
			return 1;
	}

	return 0;
}

VOID CSpecObjectInsertDlg::UpdateGraphics()
{
	std::vector<NES_SPRITE_INFO> vSprInf;
	std::vector<NES_TILE> vTiles;
	std::vector<std::vector<DWORD>> vObjects;

	CComboBox cb;
	cb.AssignWindow(Ctl(0x1030));

	BOOL fLink = FALSE;
	INT lvl = cb.GetCurSel(), obj = SPINVAL(0x1032);
	m_info.vObjInfo.clear();
	m_SetView.SetCursorPosition(0, 0);

	if ( !m_gfx.Get_Gfx(lvl, obj, vSprInf, vTiles, fLink) )
		return;
	
	// get max spr info
	size_t count = vSprInf.size(), smax = 0;
	for(size_t n = 0; n < count; n++)
	{
		std::vector<DWORD> vObj;
		m_gfx.GetBestCfgForObject(lvl, obj, n, vObj);
		vObjects.push_back(vObj);
		smax = max(smax, vObj.size());
		m_info.vObjInfo.push_back(vObj.size());
	}
	m_SetView.ResizeBufView(130 * smax, 130 * vSprInf.size());

	for(size_t n = 0; n < count; n++)
	{
		size_t scount = vObjects[n].size();
		for(size_t sn = 0; sn < scount; ++sn)
		{
			CBitmap bm;
			bm.Create(130, 130);
			bm.FillRect(0, 0, bm.Width(), bm.Height(), RGB(0x20, 0x20, 0x20), 0, 1);
			BYTE bList = HIBYTE(vObjects[n][sn]), bSpr = LOBYTE(vObjects[n][sn]);

			HDC hdc = bm.LockDC();
			m_gfx.DrawSprite(hdc, 1 + (bm.Width() - 2) / 2, 1 + (bm.Height() - 2) / 2, vTiles, m_info.vSprLists[bList][bSpr], m_bPal);

			BitBlt(m_SetView, bm.Width() * sn, bm.Height() * n, bm.Width(), bm.Height(), hdc, 0, 0, SRCCOPY);
			bm.FreeDC();
		}
	}

	m_SetView.Update();
}