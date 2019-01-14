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

INT CComboBox::GetCurSel()
{
	return (INT) cSendMessage(CB_GETCURSEL, 0, 0);
}

INT CComboBox::AddString(LPCTSTR lpszString, LPARAM lParam)
{
	INT i = (INT) cSendMessage(CB_ADDSTRING, 0, LPARAM(lpszString));
	cSendMessage(CB_SETITEMDATA, i, lParam);
	return i;
}

LPARAM CComboBox::GetParam(INT iIndex)
{
	return cSendMessage(CB_GETITEMDATA, iIndex, 0);
}

void CComboBox::SetCurSel(INT iIndex)
{
	int k = (INT) cSendMessage(CB_SETCURSEL, iIndex, 0);
}
