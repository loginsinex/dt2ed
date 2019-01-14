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

BOOL CNesEditor::GetRoomPoint(BYTE room, std::vector<POINT> & rc)
{
	if ( -1 == m_nCurrentLevel || !m_fLoaded )
		return FALSE;

	for(BYTE y = 0; y < 6; ++y)
	{
		for(BYTE x = 0; x < 8; ++x)
		{
			if ( m_nLevel.bLayout[x + 8 * y] == room )
			{
				POINT pt = { x, y };
				rc.push_back(pt);
			}
		}
	}

	return ( rc.size() > 0 );
}

BOOL CNesEditor::GetBlockFromPoint(INT x, INT y, BYTE & room, BYTE & block)
{
	if ( -1 == m_nCurrentLevel || !m_fLoaded )
		return FALSE;

	BYTE rx = x / 8, ry = y / 8;
	BYTE bx = x % 8, by = y % 8;

	if ( rx < 0 || ry < 0 || rx >= 8 || ry >= 6 )
		return FALSE;

	if ( !m_nLevel.bLayout[rx + 8 * ry] )
		return FALSE;

	room = m_nLevel.bLayout[rx + 8 * ry] - 1;
	NES_ROOM & roomdata = m_nRooms[m_nLevel.bLayout[rx + 8 * ry] - 1];
	block = roomdata.bRoom[by][bx];

	return TRUE;
}

BOOL CNesEditor::SetBlockFromPoint(INT x, INT y, BYTE room, BYTE block)
{
	if ( -1 == m_nCurrentLevel || !m_fLoaded || x < 0 || y < 0 || x >= 8 || y >= 8 )
		return FALSE;

	if ( block >= (BYTE) m_nLevel.nMajorBlocks.size() )
		return FALSE;

	std::vector<POINT> rc;

	if ( !GetRoomPoint(room, rc) )
		return FALSE;

	NES_ROOM & roomdata = m_nRooms[room - 1];
	roomdata.bRoom[y][x] = block;

	for(size_t i = 0; i < rc.size(); ++i)
		WriteImage(m_nLevel.pbRooms[rc[i].y][rc[i].x], 32 * x, 32 * y, block);

	return TRUE;
}


BOOL CNesEditor::InsertRoom(BYTE nRoomX, BYTE nRoomY)
{
	if ( m_nCurrentLevel == -1 || !m_fLoaded || nRoomX >= 8 || nRoomY >= 6 || !m_nGame.fExtended )
		return FALSE;

	INT room = INT(nRoomX) + 8 * INT(nRoomY);

	if ( m_nLevel.bLayout[room] > 0 )
		return FALSE;

	WORD new_room = m_nRooms.InsertRoom();

	if ( 0xFFFF == new_room )
		return FALSE;

	m_nLevel.bLayout[room] = LOBYTE(new_room);

	return CacheRoomsImages();
}

BOOL CNesEditor::SetRoom(BYTE nRoomX, BYTE nRoomY, BYTE bRoomId)
{
	if ( m_nCurrentLevel == -1 || !m_fLoaded || nRoomX >= 8 || nRoomY >= 6 )
		return FALSE;

	BYTE max_id = m_nRooms.RoomsCount();
	if ( bRoomId > max_id )
		return FALSE;


	INT room = INT(nRoomX) + 8 * INT(nRoomY);

	m_nLevel.bLayout[room] = bRoomId;


	return CacheRoomsImages();
}

INT CNesEditor::GetCountRoomsLinks(BYTE bRoomId)
{
	if ( m_nCurrentLevel == -1 || !m_fLoaded || !bRoomId || bRoomId > m_nRooms.RoomsCount() )
		return 0;

	INT count = 0;
	std::map<INT, PNES_LAYOUT> lo;
	Get_Layouts(lo);
	
	std::map<PNES_LAYOUT, BOOL> plo;

	for(std::map<INT, PNES_LAYOUT>::iterator v = lo.begin(); v != lo.end(); v++)
	{
		if ( plo.find(v->second) != plo.end() )
			continue;

		plo[v->second] = TRUE;

		for(INT i = 0; i < sizeof(v->second->bRoom); ++i)
			count += ( v->second->bRoom[i] == bRoomId ? 1 : 0 );
	}

	return count;
}

BOOL CNesEditor::DeleteRoom(BYTE nRoomX, BYTE nRoomY)
{
	if ( m_nCurrentLevel == -1 || !m_fLoaded || nRoomX >= 8 || nRoomY >= 6 )
		return FALSE;

	INT room = INT(nRoomX) + 8 * INT(nRoomY);
	if ( !m_nLevel.bLayout[room] )
		return FALSE;

	m_nLevel.bLayout[room] = 0;

	return CacheRoomsImages();
}

BOOL CNesEditor::Get_RoomData(BYTE bRoom, NES_ROOM & nRoom)
{
	if ( m_nCurrentLevel == -1 || bRoom >= m_nRooms.RoomsCount() )
		return FALSE;

	nRoom = m_nRooms[bRoom];
	return TRUE;
}

BOOL CNesEditor::Let_RoomData(BYTE bRoom, NES_ROOM & nRoom)
{
	if ( m_nCurrentLevel == -1 || bRoom >= m_nRooms.RoomsCount() )
		return FALSE;

	m_nRooms[bRoom] = nRoom;
	return CacheRoomsImages();
}


size_t CNesEditor::Get_LastRoom()
{
	if ( !m_pFile->IsFileLoaded() )
		return 0;

	BYTE bRoom = 0;

	std::map<INT, PNES_LAYOUT> lo;

	Get_Layouts(lo);

	for(std::map<INT, PNES_LAYOUT>::iterator v = lo.begin(); v != lo.end(); v++)
	{
		for(INT i = 0; i < sizeof(v->second->bRoom); ++i)
			bRoom = max(bRoom, v->second->bRoom[i]);
	}

	return bRoom;
}

VOID CNesEditor::Get_Layouts(std::map<INT, PNES_LAYOUT> & mlo)
{
	for(INT i = 0; i < NES_COUNT_LEVELS; ++i)
		mlo[i] = &m_nBank.nLayout[ m_nLvlBanks[i].nLayout ];
}

VOID CNesEditor::Let_Layouts(std::map<INT, PNES_LAYOUT> & lo)
{
	for(std::map<INT, PNES_LAYOUT>::iterator v = lo.begin(); v != lo.end(); v++)
		CopyMemory(&m_nBank.nLayout[ m_nLvlBanks[v->first].nLayout ], v->second, sizeof(NES_LAYOUT));
}
