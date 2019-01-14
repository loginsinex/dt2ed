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

CNesTiles::CNesTiles()
{
	ZeroMemory(&m_nTiles, sizeof(m_nTiles));
}

CNesTiles::~CNesTiles()
{
	Flush();
}

VOID CNesTiles::Flush()
{
	ZeroMemory(&m_nTiles, sizeof(m_nTiles));
}

INT CNesTiles::PPU_ArrayUnpack(CNesFile & file, INT offset, std::vector<BYTE> & arr)
{
	arr.clear();

	BYTE bRepeat = file.ByteEx(offset++);

	do
	{
		BYTE b = file.ByteEx(offset++);
		if ( b == bRepeat )
		{
			BYTE repeat = file.ByteEx(offset++);
			if ( !repeat ) // EOF array
				break;
			
			b = file.ByteEx(offset++);
			for(BYTE i = 0; i < repeat; ++i)
				arr.push_back(b);
		}
		else
			arr.push_back(b);
	} while(true);

	return arr.size();
}

INT CNesTiles::PPU_WriteTiles(std::vector<BYTE> & arr, INT offset, INT line, INT line_offset)
{
	INT tiles = min( ( arr.size() - offset + 1) / 16, 16 );

	for(int i = 0; i < tiles; ++i)
	{
		int l = 16 * i;

		NES_TILE & tile = m_nTiles[16 * line + i + line_offset];
		tile.pix[0][0].clr = 0;

		
		for(int y = 0; y < 8; ++y)
		{
			for(int x = 0; x < 8; ++x)
			{
				tile.pix[x][y].clr = 
					( 1 & ( arr[offset + l + y] >> ( 7 - x ) ) | ( 1 & ( arr[offset + 8 + l + y] >> ( 7 - x ) ) ) << 1 );
			}
		}
		
	}

	return TRUE;
}

INT CNesTiles::PPU_GetBankOffset(CNesFile & file, BYTE arr_offset)
{
	WORD mixptr = file.Word(0x4010 + arr_offset);
	int bank = (( HIBYTE(mixptr) >> 6 ) & 3) + 1;
	int ramptr = ( mixptr & 0x3FFF ) | 0x8000;
	return ( ramptr - 0x8000 + ( bank * 0x4000 ) + 0x10 );	
}

BOOL CNesTiles::PatchTiles(CNesFile & file, BYTE nBank, WORD toffset)
{
	if ( !file.IsFileLoaded() )
		return FALSE;

	int line_offset = 0;

	for(BYTE nArr = 0; nArr < 8; ++nArr)
	{
		INT boffset = file.ByteEx(BANK_OFFSET(nBank) + INT(toffset + nArr));
		WORD mixptr = file.WordEx(BANK_OFFSET(1) + 0x8000 + INT(boffset));

		std::vector<BYTE> arr;

		int bank = (( HIBYTE(mixptr) >> 6 ) & 3) + 1;
		int ramptr = ( mixptr & 0x3FFF ) | 0x8000;
		int offset = ( BANK_OFFSET(bank) + ramptr );
		int size = PPU_ArrayUnpack(file, offset, arr);
		if ( size <= 0x400 )
		{
			size_t lines = size / 0x100 + 1;
			for(size_t n = 0; n < lines; ++n)
			{
				PPU_WriteTiles(arr, 0x100 * n, line_offset / 0x10 + n);
			}
		}

		line_offset += 0x40;
	}
	return TRUE;
}

BOOL CNesTiles::LoadTiles(CNesFile & file, NES_LEVEL_HEADER & nlHead)
{
	for(int arch = 0; arch < 8; ++arch)
	{
		int offset = PPU_GetBankOffset(file, file[0xC010 + nlHead.ptrTiles + arch]);

		std::vector<BYTE> arr;
		int size = PPU_ArrayUnpack(file, offset, arr) / 16;

		if ( size > 64 ) // something wrong
			continue;

		for(int line = 0; line < size / 16; ++line)
			PPU_WriteTiles(arr, line * (16 * 16), line + 4 * arch);
	}

	// check for ppu tiles corrections
	BYTE bCorrectLine = 0;
	INT nArr = 0;
	while( 0xFF != ( bCorrectLine = file[0xC010 + nlHead.ptrPPUCorrector + nArr] ) )
	{
		INT line = 0x10 | ( bCorrectLine >> 4 ), line_offset = ( bCorrectLine & 0x0F );
		int offset = PPU_GetBankOffset(file, file[0xC010 + nlHead.ptrPPUCorrector + nArr + 1]);

		std::vector<BYTE> arr;
		int size = PPU_ArrayUnpack(file, offset, arr) / 16;

		if ( size < 64 ) // check for corrupted array
			PPU_WriteTiles(arr, 0, line, line_offset);

		nArr += 2;
	}

	nArr = 0;
	// check for ppu sprites corrections
	while( 0xFF != ( bCorrectLine = file[0xC010 + nlHead.ptrPPUSpritesCorrector + nArr] ) )
	{
		INT line = ( bCorrectLine >> 4 ), line_offset = ( bCorrectLine & 0x0F );
		int offset = PPU_GetBankOffset(file, file[0xC010 + nlHead.ptrPPUSpritesCorrector + nArr + 1]);

		std::vector<BYTE> arr;
		int size = PPU_ArrayUnpack(file, offset, arr) / 16;

		if ( size < 64 ) // check for corrupted array
			PPU_WriteTiles(arr, 0, line, line_offset);

		nArr += 2;
	}

	nArr = 0;
	// check for ppu ? corrections
	while( 0xFF != ( bCorrectLine = file[0xC010 + nlHead.ptrUnknown3 + nArr] ) )
	{
		INT line = ( bCorrectLine >> 4 ), line_offset = ( bCorrectLine & 0x0F );
		int offset = PPU_GetBankOffset(file, file[0xC010 + nlHead.ptrUnknown3 + nArr + 1]);

		std::vector<BYTE> arr;
		int size = PPU_ArrayUnpack(file, offset, arr) / 16;

		if ( size < 64 ) // check for corrupted array
			PPU_WriteTiles(arr, 0, line, line_offset);

		nArr += 2;
	}

	return TRUE;
}

BOOL CNesTiles::Get_Hash(PBYTE pbByte)
{
	BYTE md5[16] = { 0 };
	MD5_CTX context;
	MD5Init(&context);
	MD5Update(&context, (PBYTE) m_nTiles, sizeof(m_nTiles));
	MD5Final(&context);
	
	if ( IsBadWritePtr(pbByte, sizeof(context.digest)) )
		return FALSE;

	CopyMemory(pbByte, context.digest, sizeof(context.digest));
	return TRUE;
}

VOID CNesTiles::FreeTiles()
{
	Flush();
}

BOOL CNesTiles::Get_Tiles(std::vector<NES_TILE> & vTiles)
{
	vTiles.clear();
	for(INT i = 0; i < 512; ++i)
		vTiles.push_back(m_nTiles[i]);

	return TRUE;
}

BOOL CNesTiles::DrawTile(HDC hDC, INT tx, INT ty, INT tile, PBYTE pbPalette)
{
	if ( tile < -256 || tile >= 256 )
		return FALSE;

	COLORREF clr[4] = { 0, 0x808080, 0xA0A0A0, 0xFFFFFF };
	if ( pbPalette )
	{
		clr[0] = XCHGCOLORREF( g_crNesColor[ pbPalette[0] ] );
		clr[1] = XCHGCOLORREF( g_crNesColor[ pbPalette[1] ] );
		clr[2] = XCHGCOLORREF( g_crNesColor[ pbPalette[2] ] );
		clr[3] = XCHGCOLORREF( g_crNesColor[ pbPalette[3] ] );
	}

	NES_TILE & tiles = m_nTiles[tile + 256];
	for(int y = 0; y < 8; ++y)
	{
		for(int x = 0; x < 8; ++x)
		{
			SetPixel(hDC, tx + x, ty + y, clr[tiles.pix[x][y].clr]);
		}
	}
	return TRUE;
}

BOOL CNesTiles::DrawSet(HDC hDC, INT x, INT y, BOOL fCharSet, INT scale, PBYTE pbPalette)
{
	CBitmap bmCanvas;
	bmCanvas.Create(128, 128);
	HDC hbmDC = bmCanvas.LockDC();

	int delta = ( fCharSet ? 0 : -256 );

	for(INT i = 0; i < 256; ++i)
		DrawTile(hbmDC, 8 * (i % 16), 8 * (i / 16), i - delta, pbPalette);

	if ( scale > 1 )
	{
		StretchBlt(hDC, x, y, scale * bmCanvas.Width(), scale * bmCanvas.Height(),
			hbmDC, 0, 0, bmCanvas.Width(), bmCanvas.Height(), SRCCOPY);
	}
	else
		BitBlt(hDC, x, y, bmCanvas.Width(), bmCanvas.Height(), hbmDC, 0, 0, SRCCOPY);

	return TRUE;
}


BOOL CNesTiles::DrawTileSet(HDC hDC, INT x, INT y, std::vector<NES_BLOCK> & nbMinorBlock, INT scale, PBYTE pbPalette)
{
	CBitmap bmCanvas;
	bmCanvas.Create(600, 600);
	HDC hbmDC = bmCanvas.LockDC();

	for(INT i = 0; i < 256; ++i)	// first set
		DrawTile(hbmDC, 8 * (i % 16), 8 * (i / 16), i - 256, pbPalette);

	for(INT i = 0; i < 256; ++i)
		DrawTile(hbmDC, 136 + 8 * (i % 16), 8 * (i / 16), i, pbPalette);
	
	size_t count = nbMinorBlock.size();
	for(size_t i = 0; i < count; ++i)
	{
		const int x = 272 + 17 * ( i % 16 ), y = 17 * ( i / 16 );

		DrawTile(hbmDC, x +  0, y +  0, nbMinorBlock[i].bUpLeft, NULL);
		DrawTile(hbmDC, x +  8, y +  0, nbMinorBlock[i].bUpRight, NULL);
		DrawTile(hbmDC, x +  0, y +  8, nbMinorBlock[i].bBtLeft, NULL);
		DrawTile(hbmDC, x +  8, y +  8, nbMinorBlock[i].bBtRight, NULL);
	}

	if ( scale > 1 )
	{
		StretchBlt(hDC, x, y, scale * bmCanvas.Width(), scale * bmCanvas.Height(),
			hbmDC, 0, 0, bmCanvas.Width(), bmCanvas.Height(), SRCCOPY);
	}
	else
		BitBlt(hDC, x, y, bmCanvas.Width(), bmCanvas.Height(), hbmDC, 0, 0, SRCCOPY);

	bmCanvas.FreeDC();

	return TRUE;
}

/*
#ifdef _DEBUG
#define		PTRINFO(name, ram, rom)		_tprintf(TEXT(" * %s ptr: RAM: 0x%08x, ROM: 0x%05X\n"), TEXT(name), (ram), (rom))
#endif

#ifdef _DEBUG
	_tprintf(TEXT("Level %d pointers:\n"), m_iLevel + 1);
	PTRINFO("Header", NES_LEVELS_PTRS + level_offset + 0x8000 - 0x10, NES_LEVELS_PTRS + level_offset);
	PTRINFO("Blocks", m_nlHead.ptrBlocks, 0x8010 + m_nlHead.ptrBlocks);
	PTRINFO("Palette", m_nlHead.ptrPalette, m_nlHead.ptrPalette - 0x7FF0);

	PTRINFO("PPU corrector", m_nlHead.ptrPPUCorrector, 0xC010 + m_nlHead.ptrPPUCorrector);
	PTRINFO("PPU sprites corrector", m_nlHead.ptrPPUSpritesCorrector, 0xC010 + m_nlHead.ptrPPUSpritesCorrector);
	PTRINFO("Tiles", m_nlHead.ptrTiles, 0xC010 + m_nlHead.ptrTiles);

	PTRINFO("Layout", m_nlBlocks.ptrLayout, nBankRoomsOffset + m_nlBlocks.ptrLayout);
	PTRINFO("Minor blocks #1", m_nlBlocks.ptrMinorBlocks1, 0x8010 + m_nlBlocks.ptrMinorBlocks1);
	PTRINFO("Minor blocks #2", m_nlBlocks.ptrMinorBlocks2, 0x8010 + m_nlBlocks.ptrMinorBlocks2);
	PTRINFO("Minor blocks #3", m_nlBlocks.ptrMinorBlocks3, 0x8010 + m_nlBlocks.ptrMinorBlocks3);
	PTRINFO("Minor blocks #4", m_nlBlocks.ptrMinorBlocks4, 0x8010 + m_nlBlocks.ptrMinorBlocks4);

	PTRINFO("Major blocks #1", m_nlBlocks.ptrMajorBlocks1, nBankBlocksOffset + m_nlBlocks.ptrMajorBlocks1);
	PTRINFO("Major blocks #2", m_nlBlocks.ptrMajorBlocks2, nBankBlocksOffset + m_nlBlocks.ptrMajorBlocks2);
	PTRINFO("Major blocks #3", m_nlBlocks.ptrMajorBlocks3, nBankBlocksOffset + m_nlBlocks.ptrMajorBlocks3);
	PTRINFO("Major blocks #4", m_nlBlocks.ptrMajorBlocks4, nBankBlocksOffset + m_nlBlocks.ptrMajorBlocks4);

	PTRINFO("Minor blocks palettes sets", m_nlBlocks.ptrMinorBlocksPalettes, 0x8010 + m_nlBlocks.ptrMinorBlocksPalettes);

	INT nBankObjId = file[NES_OBJECTS_BANKID_PTR];
	INT nBankObjOffset = 0x4000 * nBankObjId + 0x10 - 0x8000;

	INT nOffsetPtr = file.Word(NES_OBJECTS_PTR_PTR) + nBankObjOffset;

	for(int i = 0; i < 6; ++i)
	{
		INT ptr = file.WordEx(nOffsetPtr + 12 * m_iLevel + 2 * i);
		_tprintf(TEXT(" * Objects #%d: 0x%04X, 0x%08X\n"), i + 1,  ptr, ptr + nBankObjOffset);
	}

	_tprintf(TEXT("EOF\n\n"));
#endif
*/
