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

CNesTextEditorDlg::CNesTextEditorDlg(HINSTANCE hInstance, BOOL fEditMode)
	: CDialog(hInstance), m_fEditMode( fEditMode ), m_fEdit( FALSE )
{
	m_sel.nChar = m_sel.nLevel = 0;
}

VOID CNesTextEditorDlg::OnInit(LPARAM lParam)
{
	m_Canvas.InitBufView((CBufView*) dSendMessage(0x1010, BVM_GETINSTANCE), 1024, 1024);
	m_Canvas.SetCursorSet(0, RGB(0x1F, 0x1F, 0x1F), 8, 8, 8, 8);
	m_Canvas.SetActiveCursor(0);
	m_Canvas.HideCursor(TRUE);

	m_cb.AssignWindow(Ctl(0x1011));

	CNesTiles tiles;
	m_nEditor.Get_Tiles(tiles);
	m_bmTiles.Create(0x80, 0x80);

	BYTE bPal[4] = { 0x20, 0x00, 0x10, 0x0f };
	tiles.DrawSet(m_bmTiles.LockDC(), 0, 0, TRUE, 1, bPal);
	
	for(int i = 0; i < 100; ++i)
	{
		CString s;
		s.Format(TEXT("Dialog id #%02X"), i);
		m_cb.AddString(s, i);
	}

	INT id = m_nEditor.GetDialogsCount(), counter = 0;
	BOOL fNotLink = FALSE;
	NES_DIALOG nd;
	m_dwBaseId = 0;
	while( m_nEditor.Get_Dialog(--id, nd, fNotLink) && id >= 0 )
	{
		if ( nd.fNullPtr )
		{
			if ( counter < 1 )
			{
				counter++;
				continue;
			}

			m_dwBaseId = id;
			break;
		}
	}

	id = 0;
	if ( m_sel.nChar )
		id = (int(m_sel.nLevel) * 0x10 + int(m_sel.nChar)) / 2 + m_dwBaseId;

	m_cb.SetCurSel(id);
	OnCommand(CBN_SELENDOK, 0x1011, 0);
}

VOID CNesTextEditorDlg::OnButton(USHORT uId)
{
	if ( m_fEditMode && (0x1012 == uId || 0x1013 == uId) )
	{
		if ( !m_fEdit )
		{
			EnableAll(FALSE, 0x1011, 0);
			Visible(0x1013, TRUE);
			SetCtlText(0x1012, TEXT("Apply"));
			m_fEdit = TRUE;
			m_Canvas.HideCursor(FALSE);
			SetTextCursorPos();
			SetFocus(m_Canvas);
		}
		else
		{
			m_fEdit = FALSE;
			if ( 0x1012 == uId )
			{
				if ( !m_text.fStandalone )
				{
					if ( !Confirm(TEXT("WARNING! You're updating shared dialog!\n")
									TEXT("If you choose 'YES', then will updated other linked dialogs\n")
									TEXT("If you choose 'NO', then new space will allocated for this dialog")
									)
						)
					{
						m_text.fStandalone = TRUE;
					}
				}
				AssembleDialog(m_text.nDialogId);
			}
			else
				ParseDialog(m_text.nDialogId);

			Visible(0x1013, FALSE);
			SetCtlText(0x1012, TEXT("Edit"));
			EnableAll(TRUE, 0x1011, 0);
			m_Canvas.HideCursor(TRUE);
			DrawCurrentDialog();
		}
	}
}

VOID CNesTextEditorDlg::OnCommand(USHORT uCmd, USHORT uId, HWND hCtl)
{
	if ( CBN_SELENDOK == uCmd && 0x1011 == uId )
	{
		int id = m_cb.GetParam(m_cb.GetCurSel());
		// DrawDialog(id);
		Visible(0x1012, ParseDialog(id));
		DrawCurrentDialog();
		m_Canvas.SetCursorPosition(0, 0);
	}
}

VOID CNesTextEditorDlg::OnPreInit(CDialogTemplate & d)
{
	d.SetDlgPos(0, 0, 300, 300);
	d.SetDlgTitle(TEXT("Text editor"));
	d.SetDlgStyle(DS_SHELLFONT | WS_CAPTION | WS_SYSMENU | WS_BORDER | WS_POPUPWINDOW, 0);

	d.AddPreItem(0x1010, TEXT("SysLevel32"), NULL, 7, 7, -7, -(7 + 15 + 7), WS_BORDER);
	d.AddPreItem(0x1011, cCombobox, NULL, 7, -(7 + 15), 65, 100, CBS_DROPDOWNLIST | WS_VSCROLL);

	if ( m_fEditMode )
	{
		d.AddPreItem(0x1012, cButton, TEXT("Edit"), 75, -(7 + 15), 50, 15, BS_PUSHBUTTON);
		d.AddPreItem(0x1013, cButton, TEXT("&Cancel"), 130, -(7 + 15), 50, 15, BS_PUSHBUTTON, FALSE);
	}
	
	d.AddPreItem(IDOK, cButton, MB_GetString(IDOK - 1), -(7 + 50 + 7 + 50), -(7 + 15), 50, 15, BS_DEFPUSHBUTTON);
	d.AddPreItem(IDCANCEL, cButton, MB_GetString(IDCANCEL - 1), -(7 + 50), -(7 + 15), 50, 15, BS_PUSHBUTTON);
}

VOID CNesTextEditorDlg::Let_Editor(CNesTextEditor & nEditor)
{
	m_nEditor = nEditor;
}

VOID CNesTextEditorDlg::Get_Editor(CNesTextEditor & nEditor)
{
	nEditor = m_nEditor;
}

VOID CNesTextEditorDlg::DrawCurrentDialog()
{
	RECT rc = { 0, 0, 2000, 2000 };
	FillRect(m_Canvas, &rc, GetStockBrush(BLACK_BRUSH));

	int x = 0, y = 0;
	HDC hbmDC = m_bmTiles.LockDC();
	HDC hcvDC = m_Canvas;

	const size_t line_length = 32;

	size_t count = m_text.vDialogs.size();

	for(size_t n = 0; n < count; ++n)
	{
		DrawChar(hcvDC, hbmDC, 0x40, 0, 0, x, y);
		for(size_t f = 0; f < line_length; ++f)
			DrawChar(hcvDC, hbmDC, 0x4C, 0, 0, x, y);
		DrawChar(hcvDC, hbmDC, 0x42, 0, 0, x, y);
		x = 0; y++;

		size_t lcount = m_text.vDialogs[n].size();
		for(size_t l = 0; l < lcount; ++l)
		{
			size_t bcount = m_text.vDialogs[n][l].size();
			
			DrawChar(hcvDC, hbmDC, 0x48, 0, 0, x, y);

			for(size_t b = 0; b < line_length; ++b)
			{
				if ( b < bcount )
					DrawChar(hcvDC, hbmDC, m_text.vDialogs[n][l][b], 0, 0, x, y);
				else
					DrawChar(hcvDC, hbmDC, 0x7f, 0, 0, x, y);
			}
			DrawChar(hcvDC, hbmDC, 0x48, 0, 0, x, y);
			x = 0; y++;
		}

		DrawChar(hcvDC, hbmDC, 0x44, 0, 0, x, y);
		for(size_t f = 0; f < line_length; ++f)
			DrawChar(hcvDC, hbmDC, 0x4C, 0, 0, x, y);
		DrawChar(hcvDC, hbmDC, 0x46, 0, 0, x, y);
		x = 0; y++;
	}

	m_Canvas.Update();
}

VOID CNesTextEditorDlg::OnOK()
{
	Close(1);
}

VOID CNesTextEditorDlg::DrawChar(HDC hDC, HDC hTableDC, BYTE bChar, INT basex, INT basey, INT & x, INT & y)
{
	BitBlt(hDC, (basex + x) * 8, (basey + y) * 8, 8, 8, hTableDC, 8 * (bChar % 16), 8 * (bChar / 16), SRCCOPY);
	x++;
}

VOID CNesTextEditorDlg::Let_Selection(BYTE nLevel, BYTE nChar)
{
	if ( nLevel >= NES_COUNT_LEVELS || nChar < 0x10 || nChar > 0x1e || 0 != (nChar % 2) )
		return;

	m_sel.nChar = nChar;
	m_sel.nLevel = nLevel;
}

BOOL CNesTextEditorDlg::ParseDialog(INT id)
{
	m_text.nChar = m_text.nDialog = m_text.nLine = 0;
	m_text.nDialogId = id;
	m_text.vDialogs.clear();

	std::vector<std::vector<BYTE>> vWords;
	NES_DIALOG vDialog;
	vDialog.fNullPtr = TRUE;

	if ( !m_nEditor.Get_WordsList(vWords) ||
		!m_nEditor.Get_Dialog(id, vDialog, m_text.fStandalone) || vDialog.fNullPtr )
		return FALSE;

	size_t nDialog = 0, nLine = 0, nChar = 0;
	size_t count = vDialog.nti.size();
	
	for(size_t n = 0; n < count; ++n)
	{
		nDialog = m_text.vDialogs.size();
		std::vector<std::vector<BYTE>> vLines;

		while( n < count && vDialog.nti[n].nt != ntEndOfLine && vLines.size() < 0x100 )
		{
			std::vector<BYTE> vLine;

			while( n < count && vDialog.nti[n].nt != ntNewLine && vDialog.nti[n].nt != ntEndOfLine && vLine.size() < 0x100 )
			{
				switch(vDialog.nti[n].nt)
				{
				case ntChar: vLine.push_back(vDialog.nti[n].val); break;
				case ntWord:
					{
						size_t nWord = vDialog.nti[n].val - 0x81;
						if ( nWord < vWords.size() )
						{
							size_t wcount = vWords[nWord].size();
							for(size_t w = 0; w < wcount; ++w)
								vLine.push_back(vWords[nWord][w]);
						}
						break;
					}
				}
				n++;
			}

			if ( n < vDialog.nti.size() )
				n += ( vDialog.nti[n].nt == ntNewLine ? 1 : 0 );

			vLines.push_back(vLine);
		}

		m_text.vDialogs.push_back(vLines);
	}

	return TRUE;
}

VOID CNesTextEditorDlg::AssembleDialog(INT id)
{
	NES_DIALOG vDialog;
	vDialog.fNullPtr = TRUE;
	BOOL fStandalone = FALSE;

	if ( !m_nEditor.Get_Dialog(id, vDialog, fStandalone) || vDialog.fNullPtr )
		return;

	vDialog.nti.clear();
	// in extended mode we dont need a words list
	// free size is enough to save full dialogs in ROM

	size_t count = m_text.vDialogs.size();
	for(size_t n = 0; n < count; ++n)
	{
		size_t lcount = m_text.vDialogs[n].size();
		for(size_t l = 0; l < lcount; ++l)
		{
			size_t bcount = m_text.vDialogs[n][l].size();
			for(size_t b = 0; b < bcount; ++b)
			{
				NES_TEXT_ITEM nti;
				nti.nt = ntChar;
				nti.val = m_text.vDialogs[n][l][b];
				vDialog.nti.push_back(nti);
			}

			if ( l < lcount - 1 )
			{
				NES_TEXT_ITEM nti;
				nti.nt = ntNewLine;
				vDialog.nti.push_back(nti);
			}
		}

		if ( n < count - 1)
		{
			NES_TEXT_ITEM nti;
			nti.nt = ntEndOfLine;
			vDialog.nti.push_back(nti);
		}
	}

	m_nEditor.Let_Dialog(id, vDialog, m_text.fStandalone);
}

INT_PTR CNesTextEditorDlg::OnNotify(LPNMHDR lpnm)
{
	switch(lpnm->idFrom)
	{
	case 0x1010: 
		{
			switch(lpnm->code)
			{
			case CVN_REQUESTPOINT: return HandlePos((PCVNREQPOS) lpnm);
			case CVN_REQUESTMODIFY: return HandleKey((PCVNREQMODIFY) lpnm);
			case CVN_REQDLGCODE: return (DLGC_WANTARROWS | DLGC_WANTCHARS | DLGC_WANTALLKEYS);
			}
			break;
		}
	}
	return 0;
}

INT_PTR CNesTextEditorDlg::HandlePos(PCVNREQPOS p)
{
	if ( !m_fEdit )
		return 1;

	return !ValidatePos(p->pt);
}

INT_PTR CNesTextEditorDlg::HandleKey(PCVNREQMODIFY p)
{
	if ( !m_fEdit )
		return 0;

	if ( p->key >= _T('0') && p->key <= _T('9') && HIWORD(GetKeyState(VK_SHIFT)) )
		p->key -= 0x10;

	switch(p->key)
	{
	case VK_RETURN: 
	case VK_HOME:
	case VK_END:
	case VK_BACK:
	case VK_DELETE:
		{
			p->key += 0x100;
			break;
		}
	case -1:
		{
			if ( p->delta )
				return 0;

			p->key = VK_SPACE;
			break;
		}
	case 0x27: p->key = _T('?'); break;
	case 0xBE: p->key = _T('.'); break;
	case 0xBC: p->key = _T(','); break;
	case 0x6F:
	case 0xBF: p->key = _T('/'); break;
	case 0xDC: p->key = _T('-'); break;
	case 0xDE: p->key = _T('\''); break;
	}

	WORD bChar = m_nEditor.FindChar(p->key);
	_tprintf(TEXT("Requested key: %02X. Result: %s\n"), p->key, bChar == 0xFFFF ? TEXT("NOT FOUND!") : TEXT("Found ok"));

	if ( m_text.nDialog < INT(m_text.vDialogs.size()) )
	{
		if ( m_text.nLine < INT(m_text.vDialogs[m_text.nDialog].size()) )
		{
			if ( m_text.nChar <= INT(m_text.vDialogs[m_text.nDialog][m_text.nLine].size()) )
			{
				switch(p->key)
				{
				case VK_END + 0x100:
					{
						m_text.nChar = INT(m_text.vDialogs[m_text.nDialog][m_text.nLine].size());
						SetTextCursorPos();
						break;
					}
				case VK_HOME + 0x100:
					{
						m_text.nChar = 0;
						SetTextCursorPos();
						break;
					}
				case VK_RETURN + 0x100:
					{
						if ( HIWORD(GetKeyState(VK_CONTROL)) )
						{
							std::vector<std::vector<BYTE>> vLines;
							std::vector<BYTE> vChars;
							vLines.push_back(vChars);

							m_text.vDialogs.insert(m_text.vDialogs.begin() + m_text.nDialog + 1, vLines);
							m_text.nDialog++;
							m_text.nLine = 0;
							m_text.nChar = 0;
							SetTextCursorPos();
						}
						else
						{
							std::vector<BYTE> vChars;

							vChars.insert( vChars.begin(), m_text.vDialogs[m_text.nDialog][m_text.nLine].begin() + m_text.nChar,
								m_text.vDialogs[m_text.nDialog][m_text.nLine].end());

							m_text.vDialogs[m_text.nDialog][m_text.nLine].erase(
								m_text.vDialogs[m_text.nDialog][m_text.nLine].begin() + m_text.nChar,
								m_text.vDialogs[m_text.nDialog][m_text.nLine].end());

							m_text.vDialogs[m_text.nDialog].insert(
								m_text.vDialogs[m_text.nDialog].begin() + m_text.nLine + 1, vChars);
							m_text.nLine++;
							m_text.nChar = 0;
							SetTextCursorPos();
						}
						break;
					}
				case VK_BACK + 0x100:
				case VK_DELETE + 0x100:
					{
						if ( !m_text.vDialogs[m_text.nDialog][m_text.nLine].size() )
						{
							m_text.vDialogs[m_text.nDialog].erase(
								m_text.vDialogs[m_text.nDialog].begin() + m_text.nLine);
							
							m_text.nChar = 0;

							if ( !m_text.vDialogs[m_text.nDialog].size() )
							{
								m_text.nLine = 0;
								m_text.vDialogs.erase(
									m_text.vDialogs.begin() + m_text.nDialog);

								if ( m_text.nDialog >= int(m_text.vDialogs.size()) )
									m_text.nDialog = max(0, int(m_text.vDialogs.size()) - 1);

							}
							else if ( m_text.nLine >= int(m_text.vDialogs[m_text.nDialog].size()) )
								m_text.nLine = int(m_text.vDialogs[m_text.nDialog].size()) - 1;
						}
						else
						{
							if ( m_text.nChar == int(m_text.vDialogs[m_text.nDialog][m_text.nLine].size())
								&& ( VK_DELETE + 0x100 )== p->key )
							{
								if ( m_text.nLine + 1 < int(m_text.vDialogs[m_text.nDialog].size()) )
								{
									m_text.vDialogs[m_text.nDialog][m_text.nLine].insert(
										m_text.vDialogs[m_text.nDialog][m_text.nLine].end(),
										m_text.vDialogs[m_text.nDialog][m_text.nLine + 1].begin(),
										m_text.vDialogs[m_text.nDialog][m_text.nLine + 1].end()
										);
									m_text.vDialogs[m_text.nDialog].erase(
										m_text.vDialogs[m_text.nDialog].begin() + m_text.nLine + 1);
								}
							}
							else if ( !m_text.nChar && (0x100 + VK_BACK) == p->key )
							{
								if ( m_text.nLine > 0 )
								{
									m_text.nChar = int(m_text.vDialogs[m_text.nDialog][m_text.nLine - 1].size());
									m_text.vDialogs[m_text.nDialog][m_text.nLine - 1].insert(
										m_text.vDialogs[m_text.nDialog][m_text.nLine - 1].end(),
										m_text.vDialogs[m_text.nDialog][m_text.nLine].begin(),
										m_text.vDialogs[m_text.nDialog][m_text.nLine].end()
										);
									m_text.vDialogs[m_text.nDialog].erase(
										m_text.vDialogs[m_text.nDialog].begin() + m_text.nLine);
									m_text.nLine--;
								}
							}
							else
							{
								m_text.vDialogs[m_text.nDialog][m_text.nLine].erase(
									m_text.vDialogs[m_text.nDialog][m_text.nLine].begin() + m_text.nChar
									- ( m_text.nChar > 0 && p->key == (0x100 + VK_BACK) ? 1 : 0 ));
								if ( p->key == (VK_BACK + 0x100))
									m_text.nChar -= ( m_text.nChar > 0 ? 1 : 0 );
							}


						}
						break;
					}
				default:
					{
						if ( 0xffff != bChar )
						{
							m_text.vDialogs[m_text.nDialog][m_text.nLine].insert(
								m_text.vDialogs[m_text.nDialog][m_text.nLine].begin() + m_text.nChar,
								1, LOBYTE(bChar));
							m_text.nChar++;
						}
						break;
					}
				}

				SetTextCursorPos();
				DrawCurrentDialog();
			}
		}
	}
	else if ( p->key == (0x100 + VK_RETURN) && HIWORD(GetKeyState(VK_CONTROL)) && !m_text.vDialogs.size() )
	{
		std::vector<std::vector<BYTE>> vLines;
		std::vector<BYTE> vChars;
		vLines.push_back(vChars);

		m_text.vDialogs.insert(m_text.vDialogs.begin(), vLines);
		m_text.nDialog = 0;
		m_text.nLine = 0;
		m_text.nChar = 0;
		SetTextCursorPos();
	}

	return 0;
}

BOOL CNesTextEditorDlg::ValidatePos(POINT & pt)
{
	if ( !pt.y )
	{
		size_t count = m_text.vDialogs.size();
		pt.x = 1; pt.y = 0;
		for(size_t n = 0; n < count; ++n)
		{
			pt.y++;
			size_t lcount = m_text.vDialogs[n].size();
			for(size_t l = 0; l < lcount; ++l)
			{
				if ( m_text.vDialogs[n][l].size() > 0 )
				{
					m_text.nChar = 0;
					m_text.nDialog = n;
					m_text.nLine = l;
					pt.x = 1;
					return TRUE;
				}
				pt.y++;
			}
			pt.y++;
		}
	}
	else
	{
		size_t count = m_text.vDialogs.size();
		POINT walk_pt = { 0, 0 };
		walk_pt.x = pt.x; walk_pt.y = 0;

		for(size_t n = 0; n < count; ++n)
		{
			walk_pt.y++;
			size_t lcount = m_text.vDialogs[n].size();
			for(size_t l = 0; l < lcount; ++l)
			{
				if ( walk_pt.y == pt.y )
				{
					if ( pt.x > 0 && ( pt.x - 1 ) <= int(m_text.vDialogs[n][l].size()) )
					{
						m_text.nDialog = n; m_text.nLine = l;
						m_text.nChar = pt.x - 1;
						return TRUE;
					}

					return FALSE;
				}
				walk_pt.y++;
			}
			walk_pt.y++;
		}
	}
	m_text.nDialog = m_text.nLine = m_text.nChar = 0;
	pt.x = pt.y = 0;
	return FALSE;
}

BOOL CNesTextEditorDlg::SetTextCursorPos()
{
	POINT pt = { 0 };
	size_t count = m_text.vDialogs.size();
	pt.x = 1; pt.y = 0;
	for(size_t n = 0; n < count; ++n)
	{
		pt.y++;
		size_t lcount = m_text.vDialogs[n].size();
		for(size_t l = 0; l < lcount; ++l)
		{
			if ( n == m_text.nDialog && l == m_text.nLine )
			{
				if ( m_text.vDialogs[n][l].size() > size_t(m_text.nChar) )
				{
					pt.x = 1 + m_text.nChar;
					m_Canvas.SetCursorPosition(pt.x, pt.y);
					return TRUE;
				}
				else if ( m_text.vDialogs[n][l].size() >= size_t(m_text.nChar) )
				{
					pt.x = 1 + m_text.nChar;
					m_Canvas.SetCursorPosition(pt.x, pt.y);
					return TRUE;
				}
				else if ( m_text.vDialogs[n][l].size() > size_t(m_text.nChar) )
				{
					m_text.nChar = 0;
					pt.x = 1;
					m_text.nLine++;
					pt.y++;
					if ( size_t(m_text.nLine) >= m_text.vDialogs[n].size() )
					{
						m_text.nLine = 0;
						m_text.nDialog++;
						pt.y += 2;
						if ( size_t(m_text.nDialog) >= m_text.vDialogs.size() )
						{
							m_text.nDialog = int(m_text.vDialogs.size()) - 1;
							m_text.nLine = m_text.nChar = 0;
							pt.x = pt.y = 0;
							m_Canvas.SetCursorPosition(pt.x, pt.y);
						}
					}
					m_Canvas.SetCursorPosition(pt.x, pt.y);
					return TRUE;
				}
			}
			pt.y++;
		}
		pt.y++;
	}

	return FALSE;
}