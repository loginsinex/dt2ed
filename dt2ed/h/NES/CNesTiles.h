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

#pragma pack(push, 1)
typedef struct _tagNES_PIXEL
{
	BYTE		clr:2;
} NES_PIXEL, *PNES_PIXEL;

typedef struct _tagNES_TILE
{
	NES_PIXEL		pix[8][8];
} NES_TILE, *PNES_TILE;
#pragma pack(pop)

class CNesTiles
{
	NES_TILE				m_nTiles[512];

	BOOL					DrawBlock(HDC hDC, INT x, INT y, NES_BLOCK & n);
	BOOL					DrawTile(HDC, INT x, INT y, INT tile, PBYTE pbPalette);
	INT						PPU_ArrayUnpack(CNesFile & file, INT ptr, std::vector<BYTE> & arr);
	INT						PPU_WriteTiles(std::vector<BYTE> & arr, int offset, int line, int line_offset = 0);
	INT						PPU_GetBankOffset(CNesFile & file, BYTE arr_offset);

	VOID					Flush();

public:
	CNesTiles();
	~CNesTiles();

	BOOL					LoadTiles(CNesFile & file, NES_LEVEL_HEADER & nlHead);
	BOOL					DrawTileSet(HDC hDC, INT x, INT y, std::vector<NES_BLOCK> & nbMinorBlock, INT scale = 1, PBYTE pbPalette = NULL);
	BOOL					DrawSet(HDC hDC, INT x, INT y, BOOL fCharSet, INT scale = 1, PBYTE pbPalette = NULL);
	BOOL					PatchTiles(CNesFile & file, BYTE nBank, WORD offset);
	BOOL					Get_Tiles(std::vector<NES_TILE> & vTiles);
	VOID					FreeTiles();
	BOOL					Get_Hash(PBYTE pbHash);
};