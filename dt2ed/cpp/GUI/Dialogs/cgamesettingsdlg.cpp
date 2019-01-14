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

CGameSettings::CGameSettings(HINSTANCE hInstance)
	: m_hInstance( hInstance )
{

}

VOID CGameSettings::Let_Drops(PWORD pwDrops)
{
	CopyMemory(m_wDrops, pwDrops, sizeof(m_wDrops));
}

VOID CGameSettings::Get_Drops(PWORD pwDrops)
{
	CopyMemory(pwDrops, m_wDrops, sizeof(m_wDrops));
}

VOID CGameSettings::Let_EgyptCode(PBYTE pbCode)
{
	CopyMemory(m_bEgyptCode, pbCode, sizeof(m_bEgyptCode));
}

VOID CGameSettings::Get_EgyptCode(PBYTE pbCode)
{
	CopyMemory(pbCode, m_bEgyptCode, sizeof(m_bEgyptCode));
}

VOID CGameSettings::Let_MuSecret(NES_MU_SECRET & nmu)
{
	m_nMuSecret = nmu;
}

VOID CGameSettings::Get_MuSecret(NES_MU_SECRET & nmu)
{
	nmu = m_nMuSecret;
}

VOID CGameSettings::Let_Costs(PBYTE pbCosts)
{
	CopyMemory(m_nCosts, pbCosts, sizeof(m_nCosts));
}

VOID CGameSettings::Get_Costs(PBYTE pbCosts)
{
	CopyMemory(pbCosts, m_nCosts, sizeof(m_nCosts));
}

BOOL CGameSettings::Show(CDialog * pParent)
{
	CPropSheet pSheet(m_hInstance);
	pSheet.FApply(FALSE);

	CEgyptCodeSettingsDlg dlg1(m_hInstance, m_bEgyptCode);
	CMuSecretSettingsDlg dlg2(m_hInstance, m_nMuSecret);
	CCostsSettingsDlg dlg3(m_hInstance, (PBYTE) m_nCosts);
	CObjDropSettingsDlg dlg4(m_hInstance, m_wDrops);

	pSheet.AddSheet(&dlg1);
	// pSheet.AddSheet(&dlg2);
	pSheet.AddSheet(&dlg3);
	pSheet.AddSheet(&dlg4);

	if ( pSheet.Show(pParent, 0, TEXT("Game settings")) )
	{
		dlg1.Get_EgyptCode(m_bEgyptCode);
		dlg2.Get_MuSecret(m_nMuSecret);
		dlg3.Get_Costs((PBYTE) m_nCosts);
		dlg4.Get_Drops(m_wDrops);
		return TRUE;
	}
	return FALSE;
}

CEgyptCodeSettingsDlg::CEgyptCodeSettingsDlg(HINSTANCE hInstance, PBYTE pbCode)
	: CDialog(hInstance)
{
	CopyMemory(m_bEgyptCode, pbCode, sizeof(m_bEgyptCode));
}

VOID CEgyptCodeSettingsDlg::OnPreInit(CDialogTemplate & d)
{
	d.SetDlgPos(0, 0, 180, 150);
	d.SetDlgTitle(TEXT("Egypt secret"));

	int y = 15, x = 7;
	for(int i = 0; i < 4; ++i)
	{
		CString sButton;
		sButton.Format(TEXT("Button #%d"), i + 1);
		d.AddPreItem(-1, cStatic, sButton, x, y + 3, 40, 10, SS_RIGHT, TRUE, TRUE, FALSE);

		for(int k = 0; k < sizeof(m_bEgyptCode); ++k)
		{
			d.AddPreItem(0x1010 + 0x10 * k + i, cButton, NULL, 
				x + 50 + 15 * k, y, 13, 13, BS_RADIOBUTTON);
		}

		y += 16;
		
		if ( i < 3 )
			d.AddPreItem(-1, cStatic, NULL, x + 50, y, 15 * sizeof(m_bEgyptCode), 2, 
			SS_ETCHEDHORZ, TRUE, TRUE, FALSE);

		y += 4;
	}
}

VOID CEgyptCodeSettingsDlg::OnInit(LPARAM lParam)
{
	for(int i = 0; i < sizeof(m_bEgyptCode); ++i)
		CheckAll(TRUE, 0x1010 + 0x10 * i + ( m_bEgyptCode[i] - 0x77 ), 0);
}

VOID CEgyptCodeSettingsDlg::OnButton(USHORT uId)
{
	int group = 0x10 * ( uId / 0x10 );

	CheckRadioButton(*this, group, group + 0x0F, uId);
}

VOID CEgyptCodeSettingsDlg::Get_EgyptCode(PBYTE pbCode)
{
	CopyMemory(pbCode, m_bEgyptCode, sizeof(m_bEgyptCode));
}

INT CEgyptCodeSettingsDlg::PSOnApply()
{
	for(int i = 0; i < sizeof(m_bEgyptCode); ++i)
	{
		for(int button = 0; button < 4; ++button)
		{
			if ( Checked(LOWORD(0x1010 + 0x10 * i + button) ) )
				m_bEgyptCode[i] = 0x77 + button;
		}
	}

	return PSNRET_NOERROR;
}


CMuSecretSettingsDlg::CMuSecretSettingsDlg(HINSTANCE hInstance, NES_MU_SECRET & nMuSecret)
	: CDialog(hInstance), m_nMuSecret(nMuSecret)
{

}

VOID CMuSecretSettingsDlg::OnPreInit(CDialogTemplate & d)
{
	d.SetDlgPos(0, 0, 180, 150);
	d.SetDlgTitle(TEXT("MU secret"));

	d.AddPreItem(0x1010, cButton, TEXT("Vertical secret"), 15, 15, 100, 13, BS_AUTOCHECKBOX, TRUE, (m_nMuSecret.bObjId != 0));
}

VOID CMuSecretSettingsDlg::OnInit(LPARAM lParam)
{
	CheckAll(m_nMuSecret.fVertical, 0x1010, 0);
}

INT CMuSecretSettingsDlg::PSOnApply()
{
	m_nMuSecret.fVertical = Checked(0x1010);
	return PSNRET_NOERROR;
}

VOID CMuSecretSettingsDlg::Get_MuSecret(NES_MU_SECRET & nMuSecret)
{
	nMuSecret = m_nMuSecret;
}

// -----------

CCostsSettingsDlg::CCostsSettingsDlg(HINSTANCE hInstance, PBYTE pbCosts)
	: CDialog(hInstance)
{
	CopyMemory(m_bCosts, pbCosts, sizeof(m_bCosts));
}

VOID CCostsSettingsDlg::OnPreInit(CDialogTemplate & d)
{
	d.SetDlgPos(0, 0, 180, 150);
	d.SetDlgTitle(TEXT("Costs settings"));

	d.AddPreItem(-1, cStatic, TEXT("Big red diamond cost:"), 7, 9, 90, 10, SS_RIGHT, TRUE, TRUE, FALSE);
	d.AddPreItem(0x1010, cEdit, NULL, 100, 7, -40, 13, ES_AUTOHSCROLL | ES_NUMBER | WS_BORDER);
	d.AddPreItem(-1, cStatic, TEXT("x1000 $"), -35, 9, -7, 10, SS_LEFT, TRUE, TRUE, FALSE);

	d.AddPreItem(-1, cStatic, TEXT("Big diamond cost:"), 7, 29, 90, 10, SS_RIGHT, TRUE, TRUE, FALSE);
	d.AddPreItem(0x1020, cEdit, NULL, 100, 27, -40, 13, ES_AUTOHSCROLL | ES_NUMBER | WS_BORDER);
	d.AddPreItem(-1, cStatic, TEXT("x1000 $"), -35, 29, -7, 10, SS_LEFT, TRUE, TRUE, FALSE);

	d.AddPreItem(-1, cStatic, TEXT("Treasures cost:"), 7, 49, 90, 10, SS_RIGHT, TRUE, TRUE, FALSE);
	d.AddPreItem(0x1030, cEdit, NULL, 100, 47, -40, 13, ES_AUTOHSCROLL | ES_NUMBER | WS_BORDER);
	d.AddPreItem(-1, cStatic, TEXT("x1000 $"), -35, 49, -7, 10, SS_LEFT, TRUE, TRUE, FALSE);
}

VOID CCostsSettingsDlg::OnInit(LPARAM lParam)
{
	for(int i = 0; i < 3; ++i)
	{
		DWORD dwCost = m_bCosts[i][0] * 10000 
						+ m_bCosts[i][1] * 1000
						+ m_bCosts[i][2] * 100
						+ m_bCosts[i][3] * 10
						+ m_bCosts[i][4];

		CString sCost;
		sCost.Format(TEXT("%d"), dwCost);
		SetCtlText(0x1010 + 0x10 * i, sCost);
	}
}

INT CCostsSettingsDlg::PSOnApply()
{
	for(int i = 0; i < 3; ++i)
	{
		BOOL f;
		DWORD dwCost = GetDlgItemInt(*this, 0x1010 + 0x10 * i, &f, FALSE);
		if ( !f || dwCost > 99999 )
		{
			ShowMessage(TEXT("Out of range. Must be betwean 0 and 99999"));
			return PSNRET_INVALID;
		}
		else
		{
			m_bCosts[i][0] = ( dwCost / 10000 ) % 10;
			m_bCosts[i][1] = ( dwCost / 1000 ) % 10;
			m_bCosts[i][2] = ( dwCost / 100 ) % 10;
			m_bCosts[i][3] = ( dwCost / 10 ) % 10;
			m_bCosts[i][4] = ( dwCost / 1 ) % 10;
		}
	}

	return PSNRET_NOERROR;
}

VOID CCostsSettingsDlg::Get_Costs(PBYTE pbCosts)
{
	CopyMemory(pbCosts, m_bCosts, sizeof(m_bCosts));
}

// -----------

CObjDropSettingsDlg::CObjDropSettingsDlg(HINSTANCE hInstance, PWORD pwDrops)
	: CDialog(hInstance), m_selobj( -1 )
{
	CopyMemory(m_wDrops, pwDrops, sizeof(m_wDrops));

	m_mObjects[0x4c] = TEXT("Flower");
	m_mObjects[0x4d] = TEXT("Mole");
	m_mObjects[0x4e] = TEXT("Frog");
	m_mObjects[0x4f] = TEXT("Pirate 4f");
	m_mObjects[0x50] = TEXT("Pirate 50");
	m_mObjects[0x51] = TEXT("Crab");
	m_mObjects[0x52] = TEXT("Mouse");
	m_mObjects[0x53] = TEXT("Bird");
	m_mObjects[0x54] = TEXT("Egypt dragonfly");
	m_mObjects[0x55] = TEXT("Egypt mummy");
	m_mObjects[0x56] = TEXT("Egypt snake");
	m_mObjects[0x57] = TEXT("Mu mermaid");
	m_mObjects[0x58] = TEXT("Mu statue 58");
	m_mObjects[0x59] = TEXT("Mu statue 59");
	m_mObjects[0x5a] = TEXT("Castle knight");
	m_mObjects[0x5b] = TEXT("Castle ghost 5b");
	m_mObjects[0x5c] = TEXT("Castle ghost 5c");

	m_mDrops[0x2a] = TEXT("Big red diamond");
	m_mDrops[0x2b] = TEXT("Big diamond");
	m_mDrops[0x2c] = TEXT("Small diamond");
	m_mDrops[0x2d] = TEXT("HP Ice cream");
	m_mDrops[0x2e] = TEXT("1 UP");
}

BYTE CObjDropSettingsDlg::GetChance(BYTE chance)
{
	BYTE rchance = 0;
	for(int i = 7; i >= 0; --i)
	{
		if ( ((chance >> i) & 1) )
			return rchance;

		rchance++;
	}

	return 8;
}

BYTE CObjDropSettingsDlg::GetChanceBitSet(BYTE chance)
{
	BYTE bitset = 0;
	for(int i = 0; i < (8 - chance); ++i)
	{
		bitset |= ( 1 << i );
	}

	return bitset;
}


VOID CObjDropSettingsDlg::OnPreInit(CDialogTemplate & d)
{
	d.SetDlgPos(0, 0, 200, 160);
	d.SetDlgTitle(TEXT("Drops settings"));

	d.AddPreItem(0x1010, WC_LISTVIEW, NULL, 7, 7, -7, 100, LVS_REPORT | LVS_NOSORTHEADER | WS_BORDER | LVS_SINGLESEL | LVS_SHOWSELALWAYS);
	d.AddPreItem(-1, cStatic, TEXT("Drop:"), 7, 112, 50, 10, SS_RIGHT, TRUE, TRUE, FALSE);
	d.AddPreItem(0x1011, cCombobox, NULL, 60, 110, -7, 100, CBS_DROPDOWNLIST | WS_VSCROLL, TRUE, FALSE);
	d.AddPreItem(-1, cStatic, TEXT("Chance:"), 7, 132, 50, 10, SS_RIGHT, TRUE, TRUE, FALSE);
	d.AddPreItem(0x1012, cCombobox, NULL, 60, 130, -7, 100, CBS_DROPDOWNLIST | WS_VSCROLL, TRUE, FALSE);
}

VOID CObjDropSettingsDlg::OnInit(LPARAM lParam)
{
	m_lv.AssignWindow(Ctl(0x1010));
	m_cbDrop.AssignWindow(Ctl(0x1011));
	m_cbChance.AssignWindow(Ctl(0x1012));

	m_lv.AddColumn(TEXT("Object"), 0);
	m_lv.AddColumn(TEXT("Drop"), 100);
	m_lv.AddColumn(TEXT("Chance"), -1);
	m_lv.ExtendedStyle(LVS_EX_FULLROWSELECT);

	for(std::map<INT, CString>::const_iterator v = m_mObjects.begin(); v != m_mObjects.end(); ++v)
	{
		int id = m_lv.AddItem(v->second, v->first);
		
		BYTE bObject = HIBYTE(m_wDrops[v->first - 0x4c]), bChance = LOBYTE(m_wDrops[v->first - 0x4c]);

		if ( m_mDrops.find(bObject) == m_mDrops.end() )
		{
			m_lv.SetItem(id, 1, TEXT("No drop"));
		}
		else
		{
			CString sChance;
			sChance.Format(TEXT("%d of %d"), 1, 1 << (8-GetChance(bChance)));
			m_lv.SetItem(id, 1, m_mDrops[bObject]);
			m_lv.SetItem(id, 2, sChance);
		}
	}

	for(std::map<INT, CString>::const_iterator v = m_mDrops.begin(); v != m_mDrops.end(); ++v)
		m_cbDrop.AddString(v->second, v->first);
	
	m_cbDrop.AddString(TEXT("No drop"), 0xff);

	for(int i = 0; i <= 8; ++i)
	{
		CString sChance;
		sChance.Format(TEXT("%d of %d"), 1, (1 << (8-i)));
		m_cbChance.AddString(sChance, i);
	}

	m_cbDrop.SetCurSel(0);
	m_cbChance.SetCurSel(0);
	UpdateBoxes();
}

INT CObjDropSettingsDlg::PSOnApply()
{
	return PSNRET_NOERROR;
}

VOID CObjDropSettingsDlg::OnCommand(USHORT uCmd, USHORT uId, HWND hCtl)
{
	if ( CBN_SELENDOK == uCmd && -1 != m_selobj )
	{
		BYTE bObj = LOBYTE(m_cbDrop.GetParam(m_cbDrop.GetCurSel()));
		BYTE bChance = LOBYTE(m_cbChance.GetParam(m_cbChance.GetCurSel()));

		if ( 0x1011 == uId )
		{
			if ( m_mDrops.find(bObj) == m_mDrops.end() )
			{
				m_cbChance.SetCurSel(-1);
				m_cbChance.Enable(FALSE);
				bChance = 0xff;
			}
			else
			{
				m_cbChance.SetCurSel(bChance > 8 ? 0 : bChance);
				m_cbChance.Enable(TRUE);
			}
		}

		WORD & wsel = m_wDrops[m_selobj - 0x4c];

		if ( 0xff == bChance )
		{
			m_lv.SetItem(m_lvcursel, 1, TEXT("No drop"));
			m_lv.SetItem(m_lvcursel, 2, TEXT(""));
			wsel = MAKEWORD(0, 0xff);
		}
		else
		{
			CString sChance;
			sChance.Format(TEXT("%d of %d"), 1, (1 << (8-bChance)));
			m_lv.SetItem(m_lvcursel, 1, m_mDrops[bObj]);
			m_lv.SetItem(m_lvcursel, 2, sChance);
			wsel = MAKEWORD(GetChanceBitSet(bChance), bObj);
		}
	}
}

VOID CObjDropSettingsDlg::Get_Drops(PWORD pwDrops)
{
	CopyMemory(pwDrops, m_wDrops, sizeof(m_wDrops));
}

INT_PTR CObjDropSettingsDlg::OnNotify(LPNMHDR lpnm)
{
	if ( lpnm->code == LVN_ITEMCHANGED && lpnm->idFrom == 0x1010 )
	{
		int id = m_lv.CurSel();
		if ( id >= 0 )
		{
			m_selobj = m_lv.Param(id);
			if ( m_selobj < 0x4c && m_selobj > 0x5c )
				m_selobj = -1;
		}
		else
			m_selobj = -1;

		m_lvcursel = id;
		UpdateBoxes();
	}
	return 0;
}

VOID CObjDropSettingsDlg::UpdateBoxes()
{
	if ( m_selobj == -1 )
	{
		m_cbChance.SetCurSel(-1);
		m_cbDrop.SetCurSel(-1);
		EnableAll(FALSE, 0x1011, 0x1012, 0);
		return;
	}

	EnableAll(TRUE, 0x1011, 0x1012, 0);

	BYTE bObject = HIBYTE(m_wDrops[m_selobj - 0x4c]), bChance = LOBYTE(m_wDrops[m_selobj - 0x4c]);
	
	if ( m_mDrops.find(bObject) == m_mDrops.end() )
	{
		m_cbChance.SetCurSel(-1);
		EnableAll(FALSE, 0x1012, 0);
		m_cbDrop.SetCurSel(m_cbDrop.cSendMessage(CB_GETCOUNT)-1);
	}
	else
	{
		m_cbDrop.SetCurSel(bObject - 0x2a);
		m_cbChance.SetCurSel(GetChance(bChance));
	}
}