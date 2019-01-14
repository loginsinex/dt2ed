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

#define			IDC_TAB1							0x1001
#define			IDC_MENU							0x1003
#define			IDC_REBAR							0x1004
#define			IDC_STATUSBAR						0x1005
#define			IDC_CANVAS							0x1006

class CLeftPaneDlg: public CDialog
{
	VOID				OnPreInit(CDialogTemplate & d);
	VOID				OnInit(LPARAM lParam);
	VOID				OnCommand(USHORT uCmd, USHORT uId, HWND hCtl);
	INT_PTR				OnNotify(LPNMHDR lpnm);

public:
	CLeftPaneDlg(HINSTANCE hInstance);
	VOID				EnableItem(BOOL fEnable, USHORT uId);
	VOID				EnableLayers(BOOL fEnable);
	VOID				SelectLayer(USHORT uId);
	UINT				GetSelectedLayer();
};

class CMainWnd: public CDialog
{
	CMenuWrapper	*m_pMenu;
	CLeftPaneDlg	m_dlgLeftPane;

	CToolbar		m_tb;
	CRebarWindow	m_rb;
	HWND			m_hCustomMenu;
	HACCEL			m_hAccel;
	CTilesCreator	m_Tiles;
	CTilesCreator	m_ScroogeTiles;

	CNesFile		m_File;
	CNesCanvas		m_Canvas;
	WORD			m_wCatchedItem;
	INT				m_wCatchedSlot;

	CNesEditor		m_nEditor;
	CNesTextEditor	m_nTextEditor;
	CNesGraphics	m_nGraphics;

	INT				m_nCurrentLevel;
	BYTE			m_bShowMask;

	struct
	{
		SIZE			sz;
	} m_panel;

	void			OnPreInit(CDialogTemplate & d);
	void			OnInit(LPARAM lParam);
	void			OnCommand(USHORT uCmd, USHORT uId, HWND hCtl);
	INT				OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & pHandle);
	INT_PTR			OnNotify(LPNMHDR lpnm);
	void			OnCancel();
	BOOL			OnClose(INT & iRetCode);

	VOID			SetMyIcon(LPCTSTR pszRes, LPCTSTR pszType, int id);
	VOID			CheckCommandLine();

	VOID			Ctl_OnOpenFile();
	VOID			Ctl_OnSaveFile(BOOL fReqFilename = FALSE);
	VOID			Ctl_OnEditBlocksEditor();
	VOID			Ctl_ConvertToUOROM();
	VOID			Ctl_OnSelectLevel();
	VOID			Ctl_OnShowTileset();
	VOID			Ctl_OnCreateLevel();
	VOID			Ctl_OnTextEditor();
	VOID			Ctl_OnAbout();
	VOID			Ctl_OnQuit();
	VOID			Ctl_OnEditTilesEditor();
	VOID			Ctl_OnChangeTilesBank();
	VOID			Ctl_OnActivateLayer(USHORT id);
	VOID			Ctl_OnShowDialog();
	VOID			Ctl_OnCanvasMask(USHORT uId);
	VOID			Ctl_OnGameSettings();
	VOID			Ctl_OnFlushObjects();
	VOID			Ctl_OnLevelSettings();
	VOID			Ctl_OnGraphicsEditor();
	VOID			Ctl_OnRun();
	VOID			Ctl_OnRemoveLevelSlots();

	VOID			OnOpenFile(CString s);

	INT_PTR			ReqTilePos(PCVNREQPOS p);
	INT_PTR			ReqRoomPos(PCVNREQPOS p);
	INT_PTR			ReqObjtPos(PCVNREQPOS p);
	INT_PTR			ReqScroogePos(PCVNREQPOS p);
	INT_PTR			ReqSlotPos(PCVNREQPOS p);
	INT_PTR			ReqBrdgPos(PCVNREQPOS p);
	INT_PTR			ReqMscrPos(PCVNREQPOS p);
	VOID			ReqTileModify(PCVNREQMODIFY p);
	VOID			ReqRoomModify(PCVNREQMODIFY p);
	VOID			ReqObjtModify(PCVNREQMODIFY p);
	VOID			ReqSlotModify(PCVNREQMODIFY p);
	VOID			ReqBrdgModify(PCVNREQMODIFY p);
	VOID			ReqMscrModify(PCVNREQMODIFY p);

	void			PreOnSize(UINT fwSizeType, SHORT cx, SHORT cy);
	void			PreOnSizing(UINT fwSizeType, LPRECT lprc);
	void			OnButton(USHORT uId);
	void			OnDestroy();

	//		-- custom handlers
	INT				OnFindDlgEvent(FINDREPLACE * pfr);
	void			OnInitPane();
	void			OnRebarHeightChange();

	VOID			ShowStat(POINT * ppt = NULL);
	VOID			DrawCurrentLevel(BYTE bRoomOnly = 0xFF, BOOL fDrawBridge = FALSE);

public:
	CMainWnd(HINSTANCE hInstance);
};