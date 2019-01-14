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

class CNesTextEditorDlg: public CDialog
{
	CBitmap							m_bmTiles;
	CNesTextEditor					m_nEditor;
	CNesCanvas						m_Canvas;
	CComboBox						m_cb;
	INT								m_dwBaseId;
	BOOL							m_fEditMode;
	BOOL							m_fEdit;

	struct
	{
		BYTE						nLevel;
		BYTE						nChar;
	} m_sel;

	struct
	{
		INT							nDialogId;
		std::vector<std::vector<std::vector<BYTE>>>		vDialogs;
		INT							nDialog;
		INT							nLine;
		INT							nChar;
		BOOL						fStandalone;
	} m_text;

	VOID							OnInit(LPARAM lParam);
	VOID							OnPreInit(CDialogTemplate & d);
	VOID							OnButton(USHORT uId);
	VOID							OnOK();

	// VOID							DrawDialog(int id);
	VOID							DrawCurrentDialog();
	VOID							OnCommand(USHORT uCmd, USHORT uId, HWND hCtl);
	VOID							DrawChar(HDC hDC, HDC hTableDC, BYTE bChar, INT basex, INT basey, INT & x, INT & y);
	INT_PTR							OnNotify(LPNMHDR lpnm);
	INT_PTR							HandlePos(PCVNREQPOS p);
	INT_PTR							HandleKey(PCVNREQMODIFY p);

	BOOL							ValidatePos(POINT & pt);
	BOOL							SetTextCursorPos();

	BOOL							ParseDialog(INT id);
	VOID							AssembleDialog(INT id);

public:
	CNesTextEditorDlg(HINSTANCE hInstance, BOOL fEditMode = FALSE);

	VOID				Let_Editor(CNesTextEditor & nEditor);
	VOID				Get_Editor(CNesTextEditor & nEditor);
	VOID				Let_Selection(BYTE nLevel, BYTE nChar);
};
