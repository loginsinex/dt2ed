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

class CColorSelectDlg: public CDialog
{
	BYTE				m_bColor;
	CNesCanvas			m_Canvas;

	VOID						OnPreInit(CDialogTemplate & d);
	VOID						OnInit(LPARAM lParam);
	VOID						OnOK();
	INT_PTR						OnNotify(LPNMHDR lpnm);


public:
	CColorSelectDlg(HINSTANCE hInstance);

	VOID				Let_Color(BYTE clr);
	VOID				Get_Color(BYTE & clr);
};

class CLevelSettingsDlg: public CDialog
{
	CComboBox					m_cbPal;
	CComboBox					m_cbSnd;
	CComboBox					m_cbBossLife;

	BYTE						m_bPalette[0x10];
	BYTE						m_bSound;
	CString						m_sLevelName;
	HBITMAP						m_hPals[4];
	BYTE						m_bBossLife;

	VOID						OnPreInit(CDialogTemplate & d);
	VOID						OnInit(LPARAM lParam);
	VOID						OnOK();
	VOID						OnButton(USHORT uId);
	VOID						OnCommand(USHORT uCmd, USHORT uId, HWND hCtl);
	VOID						UpdatePalSet();
	VOID						FlushPalBitmaps();

public:
	CLevelSettingsDlg(HINSTANCE hInstance);
	~CLevelSettingsDlg();

	VOID						Let_Palette(PBYTE pbPalette);
	VOID						Let_Sound(BYTE bSound);
	VOID						Let_LevelName(const CString & sLevelName);
	VOID						Let_BossLife(BYTE bBossLife);

	VOID						Get_Palette(PBYTE pbPalette);
	VOID						Get_Sound(BYTE & bSound);
	VOID						Get_LevelName(CString & sLevelName);
	VOID						Get_BossLife(BYTE & bBossLife);

};