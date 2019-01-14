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

BOOL CNesEditor::CheckObjArgs(BYTE nRoomX, BYTE nRoomY, BYTE bObjX, BYTE bObjY)
{
	if ( m_nCurrentLevel == -1 || !m_fLoaded || nRoomX >= 8 || nRoomY >= 6 ||
		bObjX >= 32 )
		return FALSE;

	BYTE bRoom = m_nLevel.bLayout[nRoomX + 8 * nRoomY];
	if ( !bRoom )
		return FALSE;

	return TRUE;
}


WORD CNesEditor::GetObjectFromPos(BYTE nRoomX, BYTE nRoomY, BYTE bObjX, BYTE bObjY, POINT & pt, BOOL fExact)
{
	if ( !CheckObjArgs(nRoomX, nRoomY, bObjX, bObjY) )
		return 0xFFFE;

	INT id = 
		( fExact ? FindObjectExact(nRoomX, nRoomY, bObjX, bObjY) : FindObject(nRoomX, nRoomY, bObjX, bObjY) );

	if ( -1 != id )
	{
		pt.x = INT( m_nLevel.nObjects[nRoomY][id].screen ) * 32 + INT( m_nLevel.nObjects[nRoomY][id].objectX );
		pt.y = INT( nRoomY ) * 256 + INT( m_nLevel.nObjects[nRoomY][id].objectY );

		return m_nLevel.nObjects[nRoomY][id].id;
	}

	return 0xFFFF;
}

INT CNesEditor::FindObjectExact(BYTE nRoomX, BYTE nRoomY, BYTE bObjX, BYTE bObjY)
{
	size_t size = m_nLevel.nObjects[nRoomY].size();

	for(size_t i = 0; i < size; ++i)
	{
		NES_OBJECT & o = m_nLevel.nObjects[nRoomY][i];
		if ( o.screen == nRoomX )
		{
			if ( o.objectX == bObjX && o.objectY == bObjY )
				return i;
		}
	}
	return -1;	
}

INT CNesEditor::FindObject(BYTE nFRoomX, BYTE & nFRoomY, BYTE bObjX, BYTE bObjY)
{
	INT nRealX = INT(nFRoomX) * 32 + INT(bObjX), nRealY = INT(nFRoomY) * 256 + INT(bObjY);
	RECT rcArea = { nRealX - 8, nRealY - 64, nRealX + 8, nRealY + 64 };

	INT nRoomY_Bottom = min(5, ( nRealY + 64 ) / 256), nRoomY_Top = ( max(nRealY, 64) - 64 ) / 256;
	INT nRoomX_Right = min(7, ( nRealX + 8 ) / 32), nRoomX_Left = ( max(nRealX, 8) - 8 ) / 32;
	const INT xdelta = -64, ydelta = -65;


	for(INT nRoomY = nRoomY_Bottom; nRoomY >= nRoomY_Top; nRoomY--)
	{
			INT size = m_nLevel.nObjects[nRoomY].size();
			for(INT i = size - 1; i >= 0; --i)
			{
				NES_OBJECT & o = m_nLevel.nObjects[nRoomY][i];
				INT nRealOX = INT(o.screen) * 32 + INT(o.objectX), nRealOY = INT(nRoomY) * 256 + INT(o.objectY);

				if ( nRealOX >= rcArea.left && nRealOY >= rcArea.top && nRealOX < rcArea.right && nRealOY < rcArea.bottom )
				{
					int oid = ( o.id == 0x5e ? int(o.id) + ( m_nCurrentLevel + 1 ) : o.id );
					POINT ptClrDot = { (nRealX - (nRealOX + xdelta / 8)), nRealY - (nRealOY + ydelta) };
					COLORREF * pclr = ((COLORREF*) m_vpbmObj[oid]->LockRawBits());
					COLORREF clr = pclr[ ptClrDot.x * 8 + m_vpbmObj[oid]->Width() * ptClrDot.y ];
					clr &= 0x00FFFFFF;
					clr = XCHGCOLORREF( clr );

					if ( clr != RGB(168, 228, 252) ) 
					{
						nFRoomY = nRoomY;
						return i;
					}
				}
			}
	}
	return -1;	
}

BOOL CNesEditor::InsertObject(BYTE nRoomX, BYTE nRoomY, BYTE bObjX, BYTE bObjY, BYTE id)
{
	if ( !CheckObjArgs(nRoomX, nRoomY, bObjX, bObjY) )
		return FALSE;

	BOOL fInserted = FALSE;
	size_t size = m_nLevel.nObjects[nRoomY].size();

	NES_OBJECT o = { 0 };
	o.id = id;
	o.screen = nRoomX;
	o.objectX = bObjX;
	o.objectY = bObjY;

	for(size_t i = 0; i < size; ++i)
	{
		NES_OBJECT & vo = m_nLevel.nObjects[nRoomY][i];

		if ( vo.screen == nRoomX && vo.objectX >= bObjX )
		{
			m_nLevel.nObjects[nRoomY].insert(m_nLevel.nObjects[nRoomY].begin() + i, o);
			return TRUE;
		}
		else if ( vo.screen > nRoomX )
		{
			m_nLevel.nObjects[nRoomY].insert(m_nLevel.nObjects[nRoomY].begin() + i, o);
			return TRUE;
		}
	}
	
	m_nLevel.nObjects[nRoomY].push_back(o);
	return TRUE;
}

BOOL CNesEditor::SetObject(BYTE nRoomX, BYTE nRoomY, BYTE bObjX, BYTE bObjY, BYTE id)
{
	if ( !CheckObjArgs(nRoomX, nRoomY, bObjX, bObjY) )
		return FALSE;

	INT vid = FindObjectExact(nRoomX, nRoomY, bObjX, bObjY);

	if ( -1 == vid )
		return FALSE;

	m_nLevel.nObjects[nRoomY][vid].id = id;
	return TRUE;
}

BOOL CNesEditor::DeleteObject(BYTE nRoomX, BYTE nRoomY, BYTE bObjX, BYTE bObjY)
{
	if ( !CheckObjArgs(nRoomX, nRoomY, bObjX, bObjY) )
		return FALSE;

	INT vid = FindObjectExact(nRoomX, nRoomY, bObjX, bObjY);

	if ( -1 == vid )
		return FALSE;

	m_nLevel.nObjects[nRoomY].erase(m_nLevel.nObjects[nRoomY].begin() + vid);
	return TRUE;
}

BOOL CNesEditor::DeleteObject(BYTE nRoomY, BYTE id)
{
	if ( !m_fLoaded || size_t(nRoomY) >= m_nLevel.nObjects.size() )
		return FALSE;

	if ( size_t(id) >= m_nLevel.nObjects[nRoomY].size() )
		return FALSE;

	m_nLevel.nObjects[nRoomY].erase(m_nLevel.nObjects[nRoomY].begin() + id);

	return TRUE;
}

VOID CNesEditor::Let_Objects()
{

}
