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

/*
room
DL ( = #F0 ), DH ( = #00 )
DL ( = #0A ), DH ( = #1D )

$0E = 0;
$0E = ( ( room & 3 ) << 6 ) + DL;
$0F = ( room >> 2 ) + ( #80 + DH );

((1 & 3) << 6 ) + DL = #40 + DL;
( 1 >> 2 ) + ( #80 + DH ) = #80 + DH;

addr = ( #80 + DH ) << 16 + ( #40 + DL ) = #8040 + ( #100 * DH + DL )
*/

BOOL CNesRooms::LoadRooms(CNesFile & file, size_t count)
{
	if ( !file.IsFileLoaded() )
		return FALSE;

	m_vRooms.clear();

	INT nBankId = file[nfRoomsBank];

	INT wPtrOffset = file.WordEx(BANK_OFFSET(nBankId) + file.Ptr(nfRoomsOffset));

	INT ptr = BANK_OFFSET(nBankId) + ( 0x8040 + wPtrOffset ),
	// INT ptr = ( !file.IsExtended() ? NES_ROOMS_DEFAULT_PTR : NES_ROOMS_UOROM_PTR ) + nBankOffset + 6 * 48,
		nBankEOF = 0x10 + 0x4000 * (( ptr + 0x4000 - 0x10 ) / 0x4000) + 0x1000;

	count = min(0x100, count);

	size_t i = 0;
	while(i < count && ptr + NES_ROOM_SIZE < nBankEOF)
	{
		NES_ROOM room = { 0 };
		file.GetDataEx(ptr, &room, NES_ROOM_SIZE);
		m_vRooms.push_back(room);
		ptr += NES_ROOM_SIZE;
		i++;
	}

	return TRUE;
}

BOOL CNesRooms::StoreRooms(CNesFile & file)
{
	if ( !file.IsFileLoaded() )
		return FALSE;

	INT nBankId = file[nfRoomsBank];
	INT wPtrOffset = file.WordEx(BANK_OFFSET(nBankId) + file.Ptr(nfRoomsOffset));

	INT ptr = BANK_OFFSET(nBankId) + ( 0x8040 + wPtrOffset ), 
		nBankEOF = 0x10 + 0x4000 * (( ptr + 0x4000 - 0x10 ) / 0x4000) + 0x1000;

	int size = min(0x100, m_vRooms.size());

	for(int i = 0; i < size; ++i)
	{
		if (ptr + NES_ROOM_SIZE >= nBankEOF)
			break;

		NES_ROOM & nRoom = m_vRooms[i];
		file.PutDataEx(ptr, &nRoom, NES_ROOM_SIZE);

		ptr += NES_ROOM_SIZE;
	}

	return TRUE;
}


NES_ROOM & CNesRooms::operator[](int id)
{
	if ( id >= 0 && id < (int) m_vRooms.size() )
		return m_vRooms[id];
	else
		return m_bEmptyRoom;
}

WORD CNesRooms::InsertRoom()
{
	if ( !m_vRooms.size() || m_vRooms.size() >= 255 )
		return -1;

	NES_ROOM room = { 0 };
	m_vRooms.push_back(room);

	return LOWORD(m_vRooms.size());
}

WORD CNesRooms::DeleteRoom(BYTE room)
{
	if ( m_vRooms.size() < 2 || room > m_vRooms.size() || !room )
		return -1;

	m_vRooms.erase(m_vRooms.begin() + (room - 1));

	return room;
}

BOOL CNesRooms::DeleteRooms(std::vector<BYTE> & vRooms)
{
	size_t count = vRooms.size();
	if ( !count )
		return FALSE;

	std::map<BYTE, INT> mRooms;
	for(size_t n = 0; n < count; ++n)
		mRooms[vRooms[n]]++;

	std::vector<NES_ROOM> vNewRooms;
	size_t crooms = m_vRooms.size();
	for(size_t n = 0; n < crooms; ++n)
	{
		if ( mRooms.find( n + 1 ) == mRooms.end() )
			vNewRooms.push_back(m_vRooms[n]);
	}

	m_vRooms = vNewRooms;

	return TRUE;
}

BYTE CNesRooms::RoomsCount()
{
	return LOBYTE(m_vRooms.size());
}