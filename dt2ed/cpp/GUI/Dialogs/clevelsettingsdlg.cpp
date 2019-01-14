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

CColorSelectDlg::CColorSelectDlg(HINSTANCE hInstance)
	: CDialog(hInstance), m_bColor(0x0f)
{

}

VOID CColorSelectDlg::OnPreInit(CDialogTemplate & d)
{
	d.SetDlgStyle(DS_SHELLFONT | WS_BORDER | WS_CAPTION, 0);
	d.SetDlgTitle(TEXT("Choose a color"));
	d.SetDlgPos(0, 0, 200, 110);

	d.AddPreItem(0x1010, TEXT("SysLevel32"), NULL, 7, 7, -7, -(7 + 15 + 7), WS_BORDER);
	d.AddPreItem(IDOK, cButton, MB_GetString(IDOK - 1), -(7 + 50 + 7 + 50), -(7 + 15), 50, 15, BS_DEFPUSHBUTTON);
	d.AddPreItem(IDCANCEL, cButton, MB_GetString(IDCANCEL - 1), -(7 + 50), -(7 + 15), 50, 15, BS_PUSHBUTTON);
}

VOID CColorSelectDlg::OnInit(LPARAM lParam)
{
	m_Canvas.InitBufView((CBufView*) dSendMessage(0x1010, BVM_GETINSTANCE), 260, 112);
	
	HBRUSH hBr = CreateSolidBrush(RGB(0xa0, 0xb0, 0xc0));
	RECT rc = { 0, 0, 260, 260 };
	FillRect(m_Canvas, &rc ,hBr);
	DeleteBrush(hBr);

	m_Canvas.SetCursorSet(0, RGB(0xc0, 0xb0, 0xa0), 16, 16, 16, 16);
	m_Canvas.SetActiveCursor(0);

	for(int i = 0; i < 64; ++i)
	{
		int x = i % 16, y = i / 16;
		RECT rc = { x * 16, y * 16, (x + 1) * 16, (y + 1) * 16 };

		HBRUSH hBr = CreateSolidBrush(XCHGCOLORREF( g_crNesColor[i] ));
		FillRect(m_Canvas, &rc, hBr);
		DeleteBrush(hBr);
	}

	m_Canvas.SetCursorPosition( m_bColor % 16, m_bColor / 16 );

	m_Canvas.Update();
}

INT_PTR CColorSelectDlg::OnNotify(LPNMHDR lpnm)
{
	if ( CVN_REQUESTPOINT == lpnm->code && 0x1010 == lpnm->idFrom )
	{
		PCVNREQPOS p = (PCVNREQPOS) lpnm;

		if ( p->pt.x < 0 || p->pt.y < 0 || p->pt.x >= 16 || p->pt.y >= 4 )
			return 1;

		BitBlt(m_Canvas, 16, 5 * 16, 16, 16, m_Canvas, p->pt.x * 16, p->pt.y * 16, SRCCOPY);
		m_Canvas.Update();
	}

	return 0;
}

VOID CColorSelectDlg::OnOK()
{
	POINT pt = m_Canvas.GetActiveCursorPos();

	m_bColor = LOBYTE(pt.x) + 0x10 * LOBYTE(pt.y);

	Close(1);
}

VOID CColorSelectDlg::Let_Color(BYTE clr)
{
	m_bColor = (clr & 0x3f);
}

VOID CColorSelectDlg::Get_Color(BYTE & clr)
{
	clr = m_bColor;
}


CLevelSettingsDlg::CLevelSettingsDlg(HINSTANCE hInstance)
	: CDialog(hInstance)
{
	m_bSound = 0;
	m_bBossLife = 1;
	ZeroMemory(m_bPalette, sizeof(m_bPalette));
	ZeroMemory(m_hPals, sizeof(m_hPals));
}

CLevelSettingsDlg::~CLevelSettingsDlg()
{
	FlushPalBitmaps();
}

VOID CLevelSettingsDlg::FlushPalBitmaps()
{
	for(int i = 0; i < 4; ++i)
		if ( m_hPals[i] )
			DeleteObject(m_hPals[i]);

	ZeroMemory(m_hPals, sizeof(m_hPals));
}

VOID CLevelSettingsDlg::OnPreInit(CDialogTemplate & d)
{
	d.SetDlgStyle(DS_SHELLFONT | WS_BORDER | WS_CAPTION | WS_OVERLAPPED, 0);
	d.SetDlgTitle(TEXT("Level settings"));
	d.SetDlgPos(0, 0, 150, 270);


	d.AddPreItem(-1, cButton, TEXT("Level name"), 7, 7, -7, 50, BS_GROUPBOX, TRUE, TRUE, FALSE);
	d.AddPreItem(0x1010, cEdit, NULL, 15, 20, -15, 13, ES_AUTOHSCROLL | WS_BORDER | ES_UPPERCASE);

	d.AddPreItem(-1, cButton, TEXT("Level palette"), 7, 67, -7, 50, BS_GROUPBOX, TRUE, TRUE, FALSE);
	d.AddPreItem(0x1020, cCombobox, NULL, 15, 80, -15, 100, WS_VSCROLL | CBS_DROPDOWNLIST);
	d.AddPreItem(0x1021, cButton, NULL, 15, 95, 15, 15, BS_BITMAP, TRUE, FALSE);
	d.AddPreItem(0x1022, cButton, NULL, 35, 95, 15, 15, BS_BITMAP);
	d.AddPreItem(0x1023, cButton, NULL, 55, 95, 15, 15, BS_BITMAP);
	d.AddPreItem(0x1024, cButton, NULL, 75, 95, 15, 15, BS_BITMAP);

	d.AddPreItem(-1, cButton, TEXT("Boss life"), 7, 127, -7, 50, BS_GROUPBOX, TRUE, TRUE, FALSE);
	d.AddPreItem(0x1040, cCombobox, NULL, 15, 140, -15, 100, CBS_DROPDOWNLIST | WS_VSCROLL);

	d.AddPreItem(-1, cButton, TEXT("Level sound"), 7, 187, -7, 50, BS_GROUPBOX, TRUE, TRUE, FALSE);
	d.AddPreItem(0x1030, cCombobox, NULL, 15, 200, -15, 100, CBS_DROPDOWNLIST | WS_VSCROLL);

	d.AddPreItem(IDOK, cButton, MB_GetString(IDOK - 1), -(7 + 50 + 7 + 50), -(7 + 15), 50, 15, BS_DEFPUSHBUTTON);
	d.AddPreItem(IDCANCEL, cButton, MB_GetString(IDCANCEL - 1), -(7 + 50), -(7 + 15), 50, 15, BS_PUSHBUTTON);
}

VOID CLevelSettingsDlg::OnInit(LPARAM lParam)
{
	m_cbPal.AssignWindow(Ctl(0x1020));
	m_cbSnd.AssignWindow(Ctl(0x1030));
	m_cbBossLife.AssignWindow(Ctl(0x1040));

	dSendMessage(0x1010, EM_SETLIMITTEXT, 12, 12);
	SetCtlText(0x1010, m_sLevelName);

	for(int i = 0; i < 0x100; ++i)
	{
		CString sSound, sBossLife;
		sSound.Format(TEXT("Sound #%d"), i + 1);
		int id = m_cbSnd.AddString(sSound, i);
		if ( i == m_bSound )
			m_cbSnd.SetCurSel(id);

		if ( i > 0 && i <= 20 )
		{
			sBossLife.Format(TEXT("%d time%s"), i, i > 1 ? TEXT("s") : TEXT(""));
			id = m_cbBossLife.AddString(sBossLife, i);
			if ( i == m_bBossLife )
				m_cbBossLife.SetCurSel(id);
		}
	}

	for(int i = 0; i < 4; ++i)
	{
		CString sPal;
		sPal.Format(TEXT("Blocks palettes set #%d"), i + 1);
		m_cbPal.AddString(sPal, i);
	}

	m_cbPal.SetCurSel(0);
	UpdatePalSet();
}

VOID CLevelSettingsDlg::OnOK()
{
	m_sLevelName = GetCtlText(0x1010);
	m_bSound = LOBYTE(m_cbSnd.GetParam(m_cbSnd.GetCurSel()));
	m_bBossLife = LOBYTE(m_cbBossLife.GetParam(m_cbBossLife.GetCurSel()));
	Close(1);
}

VOID CLevelSettingsDlg::Let_BossLife(BYTE bBossLife)
{
	m_bBossLife = bBossLife;
}

VOID CLevelSettingsDlg::Let_Palette(PBYTE pbPalette)
{
	CopyMemory(m_bPalette, pbPalette, sizeof(m_bPalette));
}

VOID CLevelSettingsDlg::Let_Sound(BYTE bSound)
{
	m_bSound = bSound;
}

VOID CLevelSettingsDlg::Let_LevelName(const CString & sLevelName)
{
	m_sLevelName = sLevelName;
}

VOID CLevelSettingsDlg::Get_BossLife(BYTE & bBossLife)
{
	bBossLife = m_bBossLife;
}

VOID CLevelSettingsDlg::Get_Palette(PBYTE pbPalette)
{
	CopyMemory(pbPalette, m_bPalette, sizeof(m_bPalette));
}

VOID CLevelSettingsDlg::Get_Sound(BYTE & bSound)
{
	bSound = m_bSound;
}

VOID CLevelSettingsDlg::Get_LevelName(CString & sLevelName)
{
	sLevelName = m_sLevelName;
}

VOID CLevelSettingsDlg::OnButton(USHORT uId)
{
	if ( uId >= 0x1022 && uId <= 0x1024 )
	{
		int id = m_cbPal.GetParam(m_cbPal.GetCurSel());
		if ( id < 0 || id >= 4 )
			return;

		BYTE clr = m_bPalette[4 * id + LOBYTE(uId - 0x1021)];

		CColorSelectDlg dlg(GetInstance());
		dlg.Let_Color(clr);

		if ( dlg.Show(this) )
		{
			dlg.Get_Color(clr);
			m_bPalette[4 * id + LOBYTE(uId - 0x1021)] = clr;
			UpdatePalSet();
		}
	}
}

VOID CLevelSettingsDlg::OnCommand(USHORT uCmd, USHORT uId, HWND hCtl)
{
	if ( CBN_SELENDOK == uCmd && 0x1020 == uId )
		UpdatePalSet();
}

VOID CLevelSettingsDlg::UpdatePalSet()
{
	int id = m_cbPal.GetParam(m_cbPal.GetCurSel());
	if ( id < 0 || id >= 4 )
		return;

	FlushPalBitmaps();

	CBitmap bm;
	bm.Create(16, 16);
	for(int i = 0; i < 4; ++i)
	{
		bm.FillRect(0, 0, bm.Width(), bm.Height(), XCHGCOLORREF( g_crNesColor[ m_bPalette[ id * 4 + i ] ] ), 0, 0);
		dSendMessage(0x1021 + i, BM_SETIMAGE, IMAGE_BITMAP, 
			(LPARAM) (m_hPals[i] = bm.ConvertToHandle()));
	}
}