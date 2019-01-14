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

class CListView: public CControl
{
	HWND		m_hHeader;

public:
	INT			AddItem(DWORD pszText, LPARAM lParam);
	INT			AddItem(LPCTSTR pszText, LPARAM lParam);
	INT			InsertItem(INT iAfterItem, LPCTSTR pszText, LPARAM lParam);
	void		SetItem(INT iItem, INT iSubItem, LPCTSTR pszText);
	void		SetItem(INT iItem, INT iSubItem, DWORD dwValue);
	void		DeleteItem(INT iItem);
	void		Param(INT iItem, LPARAM lParam);
	LPARAM		Param(INT iItem);
	void		ResetContent();
	void		CurSel(INT iItem);
	INT			CurSel();
	INT			Count();
	INT			State(INT iItem);
	void		State(INT iItem, INT iState);
	BOOL		Selected(INT iItem);
	void		Selected(INT iItem, BOOL fSelected);
	BOOL		Check(INT iItem);
	void		Check(INT iItem, BOOL fCheck);
	BOOL		ItemRect(INT iItem, INT code, LPRECT lprc);
	INT			ColumnWidth(INT iColumn);
	void		ScrollToVisible();
	BOOL		GetItemText(INT iItem, INT iSubItem, CString & text);
	CString		GetItemText(INT iItem, INT iSubItem);
	void		AddColumn(LPCTSTR pszText, INT cx = -1);
	void		ExtendedStyle(DWORD dwStyle);
	DWORD		ExtendedStyle();

	CListView() : m_hHeader( NULL ) { };
};

