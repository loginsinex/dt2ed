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

class CGfxMergeDlg: public CDialog
{
	struct
	{
		int		level;
		int		set;
	} m_info;

	CComboBox	m_cbSet;
	VOID		OnInit(LPARAM lParam);
	VOID		OnPreInit(CDialogTemplate & d);
	VOID		OnOK();

public:
	CGfxMergeDlg(HINSTANCE hInstance, int level, int set);
	
	VOID		Get_Info(int & level, int & set);
};

class CGfxInsertSet: public CDialog
{
	CNesCanvas				m_SetView;
	CComboBox				m_cbSet;
	CNesGraphics &			m_gfx;
	std::vector<BYTE>		m_vSets;
	INT						m_setid;
	std::vector<std::vector<DWORD>>	m_vSprLists;
	BYTE					m_bPal[16];

	VOID		OnInit(LPARAM lParam);
	VOID		OnPreInit(CDialogTemplate & d);
	VOID		OnOK();
	VOID		OnCommand(USHORT uCmd, USHORT uId, HWND hCtl);

	VOID		UpdateView();

public:
	CGfxInsertSet(HINSTANCE hInstance, std::vector<BYTE> & vSets, CNesGraphics & gfx, PBYTE pbPal);

	VOID		Get_SetId(INT & setid);
};

class CGfxEditorDlg: public CDialog
{
	INT							m_cLevel;
	BOOL						m_fExtended;

	CNesGraphics				m_gfx;
	CNesCanvas					m_Tiles;
	CNesCanvas					m_Sprite;
	std::vector<std::vector<DWORD>>	m_vSprLists;
	BYTE						m_bPal[16];

	CComboBox					m_cbSet;
	CComboBox					m_cbSprId;

	CBitmap						m_bmTiles;
	CBitmap						m_bmHlTiles;

	struct
	{
		std::vector<NES_SPRITE_INFO>	vSprInf;
		std::vector<NES_TILE>			vTiles;
		std::vector<DWORD>				vSet;
		INT								set;
		INT								lvl;
		INT								obj;
		BOOL							fLink;
	} m_info;

	VOID						OnInit(LPARAM lParam);
	VOID						OnPreInit(CDialogTemplate & d);
	VOID						OnOK();
	VOID						OnCommand(USHORT uCmd, USHORT uId, HWND hCtl);
	VOID						OnButton(USHORT uId);
	INT_PTR						OnNotify(LPNMHDR lpnm);
	INT							ReqSprPos(PCVNREQPOS p);

	VOID						UpdateInfo();
	VOID						UpdateTiles();
	VOID						UpdateSpriteInfo();

public:
	CGfxEditorDlg(HINSTANCE hInstance);
	VOID						Let_Editor(INT ilevel, INT set, CNesGraphics & gfx, PBYTE pbPal, BOOL fExtended);
	VOID						Get_Editor(CNesGraphics & gfx);
};