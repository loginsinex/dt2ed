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

CMenuWrapper::CMenuWrapper(HMENU hMenu)
	: m_hMenu( hMenu )
{

}

HMENU CMenuWrapper::FindItem(HMENU hMenu, UINT uId)
{
	UINT uCount = GetMenuItemCount(hMenu);
	for(UINT i = 0; i < uCount; ++i)
	{
		HMENU hSubMenu = GetSubMenu(hMenu, i);
		if ( hSubMenu )
		{
			HMENU hFoundMenu = FindItem(hSubMenu, uId);
			if ( hFoundMenu )
				return hFoundMenu;
		}
		else
		{
			if ( uId == GetMenuItemID(hMenu, i) )
			{
				return hMenu;
			}
		}
	}

	return NULL;
}

VOID CMenuWrapper::EnableItems(BOOL fEnable, ...)
{
	va_list argptr;
	va_start(argptr, fEnable);

	do
	{
		UINT id = va_arg( argptr, UINT );
		if ( !id )
			break;
		
		EnableItem(fEnable, id);
	} while( true );
}

VOID CMenuWrapper::EnableItem(BOOL fEnable, UINT uId)
{
	HMENU hSubMenu = FindItem(m_hMenu, uId);
	if ( !hSubMenu )
		return;

	if ( fEnable )
		EnableMenuItem(hSubMenu, uId, MF_ENABLED | MF_BYCOMMAND);
	else
		EnableMenuItem(hSubMenu, uId, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
}

VOID CMenuWrapper::CheckItems(BOOL fEnable, ...)
{
	va_list argptr;
	va_start(argptr, fEnable);

	do
	{
		UINT id = va_arg( argptr, UINT );
		if ( !id )
			break;
		
		CheckItem(fEnable, id);
	} while( true );
}

VOID CMenuWrapper::CheckItem(BOOL fEnable, UINT uId)
{
	HMENU hSubMenu = FindItem(m_hMenu, uId);
	if ( !hSubMenu )
		return;

	if ( fEnable )
		CheckMenuItem(hSubMenu, uId, MF_CHECKED | MF_BYCOMMAND);
	else
		CheckMenuItem(hSubMenu, uId, MF_UNCHECKED | MF_BYCOMMAND);
}