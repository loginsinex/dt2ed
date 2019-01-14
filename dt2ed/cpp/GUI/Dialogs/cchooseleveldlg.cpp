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

CChooseLevelDlg::CChooseLevelDlg(HINSTANCE hInstance)
	: CDialog(hInstance), m_nLevel( 0 )
{

}

VOID CChooseLevelDlg::Let_LevelNames(std::vector<CString> & sLevelsNames)
{
	m_sLevelsNames = sLevelsNames;
}

VOID CChooseLevelDlg::OnPreInit(CDialogTemplate & d)
{
	d.SetDlgPos(0, 0, 150, 175);
	d.SetDlgTitle(TEXT("Select a level"));
	d.SetDlgStyle(DS_SHELLFONT | WS_CAPTION | WS_BORDER, 0);

	d.AddPreItem(IDC_LIST1, cListbox, NULL, 7, 7, -7, -(7 + 7 + 15), WS_BORDER);
	d.AddPreItem(IDOK, cButton, MB_GetString(IDOK - 1), -(7 + 50 + 7 + 50), -(7 + 15), 50, 15, BS_DEFPUSHBUTTON);
	d.AddPreItem(IDCANCEL, cButton, MB_GetString(IDCANCEL - 1), -(7 + 50), -(7 + 15), 50, 15, BS_PUSHBUTTON);
}

VOID CChooseLevelDlg::OnInit(LPARAM lParam)
{
	m_nLevel = lParam;
	size_t count = m_sLevelsNames.size();

	for(size_t i = 0; i < count; ++i)
	{
		CString str = ( i < m_sLevelsNames.size() ? m_sLevelsNames[i] : TEXT("") );
		str = str.TrimRight();
		CharLower(str.GetBuffer() + 1);
		str.ReleaseBuffer();

		if ( str.GetLength() < 1 )
			str.Format(TEXT("Level #%d"), i+1);

		int id = dSendMessage(IDC_LIST1, LB_ADDSTRING, 0, (LPARAM) str.GetString());
		if ( i == m_nLevel )
			dSendMessage(IDC_LIST1, LB_SETCURSEL, i, i);
	}
}

VOID CChooseLevelDlg::OnOK()
{
	INT id = dSendMessage(IDC_LIST1, LB_GETCURSEL);
	if ( -1 == id || id >= 7 )
		return;

	m_nLevel = id;
	Close(1);
}

INT CChooseLevelDlg::GetLevel()
{
	return m_nLevel;
}
