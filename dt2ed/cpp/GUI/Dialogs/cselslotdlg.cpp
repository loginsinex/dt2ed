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

CSelSlotDlg::CSelSlotDlg(HINSTANCE hInstance)
	: CDialog(hInstance), m_dSelected( 0 )
{

}

VOID CSelSlotDlg::OnPreInit(CDialogTemplate & d)
{
	d.SetDlgTitle(TEXT("Slots sets"));
	d.SetDlgStyle(DS_SHELLFONT | WS_BORDER | WS_CAPTION, 0);
	d.SetDlgPos(0, 0, 130, 10);

	INT cy = 7;
	for(INT i = 0; i < 7; ++i)
	{
		CString s;
		s.Format(TEXT("Slots set #%d (Object: #%02X)"), i + 1, 0x7C + i);
		d.AddPreItem(0x1010 + i, cButton, s, 7, cy, -7, 13, BS_AUTORADIOBUTTON);

		cy += 15;
	}

	cy += 7;
	
	d.AddPreItem(IDOK, cButton, MB_GetString(IDOK - 1), -(7 + 50 + 7 + 50), cy, 50, 15, BS_DEFPUSHBUTTON);
	d.AddPreItem(IDCANCEL, cButton, MB_GetString(IDCANCEL - 1), -(7 + 50), cy, 50, 15, BS_PUSHBUTTON);

	cy += (15 + 7);

	d.SetDlgPos(0, 0, 130, cy);

}

VOID CSelSlotDlg::OnInit(LPARAM lParam)
{
	CheckAll(TRUE, 0x1010 + m_dSelected);
}

VOID CSelSlotDlg::Get_Selected(INT & dSelected)
{
	dSelected = m_dSelected;
}

VOID CSelSlotDlg::Let_Selected(INT dSelected)
{
	if ( dSelected >= 0 && dSelected < 7 )
		m_dSelected = dSelected;
}

VOID CSelSlotDlg::OnOK()
{
	for(int i = 0; i < 7; ++i)
	{
		if ( Checked(LOWORD(0x1010 + i)) )
		{
			m_dSelected = i;
			break;
		}
	}

	Close(1);
}