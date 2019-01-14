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

#define		NES_ROOMS_PTR			0x11D5A
#define		NES_ROOMS_EOF_PTR		0x14E10
#define		NES_ROOM_SIZE			(8 * 8)

#define		NES_ROOMS_BANKID_PTR	0x1CA86
#define		NES_ROOMS_DEFAULT_PTR	0x9C2A
#define		NES_ROOMS_UOROM_PTR		0x8010

typedef struct _tagNES_ROOM
{
	BYTE		bRoom[8][8];
} NES_ROOM, *PNES_ROOM;

class CNesRooms
{
	std::vector<NES_ROOM>		m_vRooms;
	NES_ROOM					m_bEmptyRoom;

public:
	BOOL		LoadRooms(CNesFile & file, size_t count);
	BOOL		StoreRooms(CNesFile & file);
	NES_ROOM & operator[](int id);
	WORD		InsertRoom();
	WORD		DeleteRoom(BYTE room);
	BOOL		DeleteRooms(std::vector<BYTE> & vRooms);
	BYTE		RoomsCount();
};