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

class CGameSettings
{
	BYTE			m_bEgyptCode[10];
	NES_MU_SECRET	m_nMuSecret;
	HINSTANCE		m_hInstance;
	BYTE			m_nCosts[3][5];
	WORD			m_wDrops[17];

public:
	CGameSettings(HINSTANCE hInstance);
	VOID			Let_EgyptCode(PBYTE pbCode);
	VOID			Get_EgyptCode(PBYTE pbCode);

	VOID			Let_MuSecret(NES_MU_SECRET & nmu);
	VOID			Get_MuSecret(NES_MU_SECRET & nmu);

	VOID			Let_Costs(PBYTE pbCosts);
	VOID			Get_Costs(PBYTE pbCosts);

	VOID			Let_Drops(PWORD pwDrops);
	VOID			Get_Drops(PWORD pwDrops);

	BOOL			Show(CDialog * pParent);
};

class CEgyptCodeSettingsDlg: public CDialog
{
	BYTE			m_bEgyptCode[10];
	
	VOID			OnPreInit(CDialogTemplate & d);
	VOID			OnInit(LPARAM lParam);
	VOID			OnButton(USHORT uId);
	INT				PSOnApply();

public:
	CEgyptCodeSettingsDlg(HINSTANCE hInstance, PBYTE pbCode);

	VOID			Get_EgyptCode(PBYTE pbCode);
};

class CCostsSettingsDlg: public CDialog
{
	BYTE			m_bCosts[3][5];
	
	VOID			OnPreInit(CDialogTemplate & d);
	VOID			OnInit(LPARAM lParam);
	INT				PSOnApply();

public:
	CCostsSettingsDlg(HINSTANCE hInstance, PBYTE pbCosts);

	VOID			Get_Costs(PBYTE pbCosts);
};

class CObjDropSettingsDlg: public CDialog
{
	std::map<INT, CString>	m_mObjects;
	std::map<INT, CString>	m_mDrops;

	int						m_selobj;
	int						m_lvcursel;

	CListView				m_lv;
	CComboBox				m_cbDrop;
	CComboBox				m_cbChance;

	WORD					m_wDrops[17];
	VOID					OnPreInit(CDialogTemplate & d);
	VOID					OnInit(LPARAM lParam);
	INT_PTR					OnNotify(LPNMHDR lpnm);
	VOID					OnCommand(USHORT uCmd, USHORT uId, HWND hCtl);

	INT						PSOnApply();
	BYTE					GetChance(BYTE chance);
	BYTE					GetChanceBitSet(BYTE chance);
	VOID					UpdateBoxes();

public:
	CObjDropSettingsDlg(HINSTANCE hInstance, PWORD pwDrops);

	VOID			Get_Drops(PWORD pwDrops);
};


class CMuSecretSettingsDlg: public CDialog
{
	NES_MU_SECRET	m_nMuSecret;

	VOID			OnPreInit(CDialogTemplate & d);
	VOID			OnInit(LPARAM lParam);
	INT				PSOnApply();

public:
	CMuSecretSettingsDlg(HINSTANCE hInstance, NES_MU_SECRET & nMuSecret);

	VOID			Get_MuSecret(NES_MU_SECRET & nMuSecret);
};