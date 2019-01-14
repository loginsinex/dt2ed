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

class CObjectInsert
{
	BYTE				m_bObjId;
	HINSTANCE			m_hInstance;
	CTilesCreator &		m_tiles;
	CNesGraphics &		m_gfx;
	BYTE				m_bPal[16];
	BYTE				m_bBossObject;

public:
	CObjectInsert(HINSTANCE hInstance, CTilesCreator & tiles, CNesGraphics & gfx, PBYTE pbPal, BYTE bBossObject);
	VOID				Let_Object(BYTE bObjId);
	VOID				Get_Object(BYTE & bObjId);

	BOOL				Show(CDialog * pParent);
};


class CObjectInsertDlg: public CDialog
{
	CObjectInsert &		m_Parent;
	BYTE				m_objId;
	CTilesCreator &		m_tiles;
	BYTE				m_bBossObject;

	VOID				OnInit(LPARAM lParam);
	VOID				OnPreInit(CDialogTemplate & d);
	INT_PTR				OnNotify(LPNMHDR lpnm);
	INT					PSOnSetActive();

public:
	CObjectInsertDlg(HINSTANCE hInstance, CObjectInsert & parent, CTilesCreator & tiles, BYTE bBossObject);

};

class CSpecObjectInsertDlg: public CDialog
{
	CObjectInsert &		m_Parent;
	CNesGraphics &		m_gfx;
	BYTE				m_bPal[16];
	CNesCanvas			m_SetView;

	BYTE				m_objId;

	struct
	{
		std::vector<std::vector<DWORD>>	vSprLists;
		std::vector<size_t>				vObjInfo;
	} m_info;

	VOID				OnInit(LPARAM lParam);
	VOID				OnPreInit(CDialogTemplate & d);
	INT					PSOnSetActive();
	VOID				OnCommand(USHORT uCmd, USHORT uId, HWND hCtl);
	VOID				OnButton(USHORT uId);
	VOID				UpdateGraphics();
	INT_PTR				OnNotify(LPNMHDR lpnm);

public:
	CSpecObjectInsertDlg(HINSTANCE hInstance, CObjectInsert & parent, CNesGraphics & gfx, PBYTE bPal);
};