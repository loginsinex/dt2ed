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

BOOL CNesEditor::FreeRoomsImages()
{
	for(INT y = 0; y < 6; ++y)
	{
		for(INT x = 0; x < 8; ++x)
		{
			if ( m_nLevel.pbRooms[y][x] )
			{
				delete m_nLevel.pbRooms[y][x];

				m_nLevel.pbRooms[y][x] = NULL;
			}
		}
	}

	return TRUE;
}

BOOL CNesEditor::FlushLevel()
{
	if ( -1 != m_nCurrentLevel && m_fLoaded )
	{
		CopyMemory(&m_nBank.nPalette[m_nLvlBanks[m_nCurrentLevel].nPalette][0], m_nLevel.bPalette, sizeof(m_nLevel.bPalette));
		CopyMemory(&m_nBank.nLayout[m_nLvlBanks[m_nCurrentLevel].nLayout], m_nLevel.bLayout, sizeof(m_nBank.nLayout[m_nLvlBanks[m_nCurrentLevel].nLayout]));

		m_nBank.nMinor.nMinorBlocks[0][m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocks[0]].clear();
		m_nBank.nMinor.nMinorBlocks[1][m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocks[1]].clear();
		m_nBank.nMinor.nMinorBlocks[2][m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocks[2]].clear();
		m_nBank.nMinor.nMinorBlocks[3][m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocks[3]].clear();

		std::vector<BYTE> & nbp = m_nBank.nMinor.nMinorBlocksPalettes[m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocksPalettes];
		nbp.clear(); nbp.insert(nbp.begin(), 64, 0);
		CopyMemory(&nbp[0], m_nLevel.bPalTiles, nbp.size());

		size_t count = m_nLevel.nMinorBlocks.size();
		for(size_t n = 0; n < count; ++n)
		{
			NES_BLOCK & nb = m_nLevel.nMinorBlocks[n];
			m_nBank.nMinor.nMinorBlocks[0][m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocks[0]].push_back(nb.bUpLeft);
			m_nBank.nMinor.nMinorBlocks[1][m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocks[1]].push_back(nb.bUpRight);
			m_nBank.nMinor.nMinorBlocks[2][m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocks[2]].push_back(nb.bBtLeft);
			m_nBank.nMinor.nMinorBlocks[3][m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocks[3]].push_back(nb.bBtRight);
		}

		m_nBank.nMajor.nMajorBlocks[0][m_nLvlBanks[m_nCurrentLevel].nMajor.nMajorBlock[0]].clear();
		m_nBank.nMajor.nMajorBlocks[1][m_nLvlBanks[m_nCurrentLevel].nMajor.nMajorBlock[1]].clear();
		m_nBank.nMajor.nMajorBlocks[2][m_nLvlBanks[m_nCurrentLevel].nMajor.nMajorBlock[2]].clear();
		m_nBank.nMajor.nMajorBlocks[3][m_nLvlBanks[m_nCurrentLevel].nMajor.nMajorBlock[3]].clear();
		
		count = m_nLevel.nMajorBlocks.size();
		for(size_t n = 0; n < count; ++n)
		{
			NES_BLOCK & nb = m_nLevel.nMajorBlocks[n];
			m_nBank.nMajor.nMajorBlocks[0][m_nLvlBanks[m_nCurrentLevel].nMajor.nMajorBlock[0]].push_back(nb.bUpLeft);
			m_nBank.nMajor.nMajorBlocks[1][m_nLvlBanks[m_nCurrentLevel].nMajor.nMajorBlock[1]].push_back(nb.bUpRight);
			m_nBank.nMajor.nMajorBlocks[2][m_nLvlBanks[m_nCurrentLevel].nMajor.nMajorBlock[2]].push_back(nb.bBtLeft);
			m_nBank.nMajor.nMajorBlocks[3][m_nLvlBanks[m_nCurrentLevel].nMajor.nMajorBlock[3]].push_back(nb.bBtRight);
		}

		std::vector<NES_OBJECTS_FLOOR> & vnof = m_nBank.nObjects[m_nCurrentLevel];
		count = vnof.size();
		for(size_t n = 0; n < count; ++n)
		{
			if ( m_nLevel.nObjects[n].size() > 0 )
			{
				vnof[n].bFloor = n;
				vnof[n].nObj = m_nLevel.nObjects[n];
			}
			else
			{
				vnof[n].bFloor = 0xFF;
				vnof[n].nObj.clear();
			}
		}
	}

	return TRUE;
}

BOOL CNesEditor::FreeLevel()
{
	ZeroMemory(m_nLevel.bPalette, sizeof(m_nLevel.bPalette));
	ZeroMemory(m_nLevel.bLayout, sizeof(m_nLevel.bLayout));
	m_nLevel.nTiles.clear();
	m_nLevel.nMinorBlocks.clear();
	m_nLevel.nMajorBlocks.clear();
	m_nLevel.nObjects.clear();
	m_nLevel.nBlocks.clear();

	FreeRoomsImages();

	return TRUE;
}

BOOL CNesEditor::CacheLevel()
{
	FreeLevel();


	CopyMemory(m_nLevel.bPalette, &m_nBank.nPalette[m_nLvlBanks[m_nCurrentLevel].nPalette][0], sizeof(m_nLevel.bPalette));
	CopyMemory(m_nLevel.bLayout, &m_nBank.nLayout[m_nLvlBanks[m_nCurrentLevel].nLayout].bRoom[0], sizeof(m_nBank.nLayout[m_nLvlBanks[m_nCurrentLevel].nLayout]));

	for(int i = 0; i < sizeof(m_nLevel.bLayout); ++i)
		_tprintf(TEXT("%02x %s"), m_nLevel.bLayout[i], !( (i + 1) % 8 ) ? TEXT("\n") : TEXT("") );

	m_nLevel.nMinorBlocks.clear();

	std::vector<BYTE> & nbp = m_nBank.nMinor.nMinorBlocksPalettes[m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocksPalettes];
	CopyMemory(m_nLevel.bPalTiles, &nbp[0], nbp.size());

	size_t count = m_nBank.nMinor.nMinorBlocks[0][m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocks[0]].size();
	for(size_t n = 0; n < count; ++n)
	{
		NES_BLOCK nb = { 0 }; 
		nb.bUpLeft = m_nBank.nMinor.nMinorBlocks[0][m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocks[0]][n];
		nb.bUpRight = m_nBank.nMinor.nMinorBlocks[1][m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocks[1]][n];
		nb.bBtLeft = m_nBank.nMinor.nMinorBlocks[2][m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocks[2]][n];
		nb.bBtRight = m_nBank.nMinor.nMinorBlocks[3][m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocks[3]][n];
		m_nLevel.nMinorBlocks.push_back(nb);
	}

	count = m_nBank.nMajor.nMajorBlocks[0][m_nLvlBanks[m_nCurrentLevel].nMajor.nMajorBlock[0]].size();
	for(size_t n = 0; n < count; ++n)
	{
		NES_BLOCK nb = { 0 }; 
		nb.bUpLeft = m_nBank.nMajor.nMajorBlocks[0][m_nLvlBanks[m_nCurrentLevel].nMajor.nMajorBlock[0]][n];
		nb.bUpRight = m_nBank.nMajor.nMajorBlocks[1][m_nLvlBanks[m_nCurrentLevel].nMajor.nMajorBlock[1]][n];
		nb.bBtLeft = m_nBank.nMajor.nMajorBlocks[2][m_nLvlBanks[m_nCurrentLevel].nMajor.nMajorBlock[2]][n];
		nb.bBtRight = m_nBank.nMajor.nMajorBlocks[3][m_nLvlBanks[m_nCurrentLevel].nMajor.nMajorBlock[3]][n];
		m_nLevel.nMajorBlocks.push_back(nb);
	}
		
	count = 6;
	for(size_t n = 0; n < 6; ++n)
	{
		size_t co = m_nBank.nObjects[m_nCurrentLevel].size();
		BOOL fFoundLine = FALSE;
		for(size_t no = 0; no < co; ++no)
		{
			NES_OBJECTS_FLOOR & nof = m_nBank.nObjects[m_nCurrentLevel][no];
			if ( nof.bFloor == n )
			{
				fFoundLine = TRUE;
				m_nLevel.nObjects.push_back(nof.nObj);
				break;
			}
		}

		if ( !fFoundLine )
		{
			std::vector<NES_OBJECT> nobj;
			m_nLevel.nObjects.push_back(nobj);
		}
	}
		
	m_nTiles[m_nCurrentLevel].Get_Tiles(m_nLevel.nTiles);
	
	return ( CacheBlocks() && CacheRoomsImages() );
}

BYTE CNesEditor::GetTileMask(BYTE tileId)
{
	if ( tileId >= 0x8E && tileId <= 0x97 )
		return ( 0x40 >> 2 );
	else if ( tileId >= 0xF8 )
		return ( 0x80 >> 2 );
	else if ( tileId >= 0x98 && tileId <= 0x99 )
		return ( 0x20 >> 2 );
	else if ( tileId >= 0xEC && tileId <= 0xF7 )
		return ( 0xA0 >> 2 );

	return 0;
}

BOOL CNesEditor::CacheBlocks()
{
	m_nLevel.nBlocks.clear();
	m_nLevel.vTilesUsed.clear();
	m_nLevel.vTilesUsed.insert(m_nLevel.vTilesUsed.begin(), 256, 0);
	size_t size = m_nLevel.nMajorBlocks.size();

	for(size_t i = 0; i < size; ++i)
	{
		NES_BLOCK & mjb = m_nLevel.nMajorBlocks[i];
		std::vector<NES_BLOCK> & mnb = m_nLevel.nMinorBlocks;

		BYTE tiles[16] = 
		{  
			mnb[mjb.bUpLeft].bUpLeft, 
			mnb[mjb.bUpLeft].bUpRight,
			mnb[mjb.bUpLeft].bBtLeft, 
			mnb[mjb.bUpLeft].bBtRight,

			mnb[mjb.bUpRight].bUpLeft, 
			mnb[mjb.bUpRight].bUpRight,
			mnb[mjb.bUpRight].bBtLeft, 
			mnb[mjb.bUpRight].bBtRight,

			mnb[mjb.bBtLeft].bUpLeft, 
			mnb[mjb.bBtLeft].bUpRight,
			mnb[mjb.bBtLeft].bBtLeft, 
			mnb[mjb.bBtLeft].bBtRight,

			mnb[mjb.bBtRight].bUpLeft, 
			mnb[mjb.bBtRight].bUpRight,
			mnb[mjb.bBtRight].bBtLeft, 
			mnb[mjb.bBtRight].bBtRight
		};

		m_nLevel.vTilesUsed[mjb.bUpLeft]++;
		m_nLevel.vTilesUsed[mjb.bUpRight]++;
		m_nLevel.vTilesUsed[mjb.bBtLeft]++;
		m_nLevel.vTilesUsed[mjb.bBtRight]++;

		std::vector<BYTE> nMajBlock;
		nMajBlock.insert(nMajBlock.begin(), 1024, 0);

		BYTE nPal = 0;
		
#define		PAL( n ) (( m_nLevel.bPalTiles[( n ) / 4] >> ( 2 * ( ( (n ) % 4 ) ) ) ) & 3)

		nPal = PAL(mjb.bUpLeft) | GetTileMask(mjb.bUpLeft);
		WriteTile(nMajBlock,  0,  0, tiles[ 0], nPal);
		WriteTile(nMajBlock,  8,  0, tiles[ 1], nPal);
		WriteTile(nMajBlock,  0,  8, tiles[ 2], nPal);
		WriteTile(nMajBlock,  8,  8, tiles[ 3], nPal);

		nPal = PAL(mjb.bUpRight) | GetTileMask(mjb.bUpRight);
		WriteTile(nMajBlock, 16,  0, tiles[ 4], nPal);
		WriteTile(nMajBlock, 24,  0, tiles[ 5], nPal);
		WriteTile(nMajBlock, 16,  8, tiles[ 6], nPal);
		WriteTile(nMajBlock, 24,  8, tiles[ 7], nPal);

		nPal = PAL(mjb.bBtLeft) | GetTileMask(mjb.bBtLeft);
		WriteTile(nMajBlock,  0, 16, tiles[ 8], nPal);
		WriteTile(nMajBlock,  8, 16, tiles[ 9], nPal);
		WriteTile(nMajBlock,  0, 24, tiles[10], nPal);
		WriteTile(nMajBlock,  8, 24, tiles[11], nPal);

		nPal = PAL(mjb.bBtRight) | GetTileMask(mjb.bBtRight);
		WriteTile(nMajBlock, 16, 16, tiles[12], nPal);
		WriteTile(nMajBlock, 24, 16, tiles[13], nPal);
		WriteTile(nMajBlock, 16, 24, tiles[14], nPal);
		WriteTile(nMajBlock, 24, 24, tiles[15], nPal);
#undef PAL

		m_nLevel.nBlocks.push_back(nMajBlock);
	}

	return TRUE;
}

BOOL CNesEditor::CacheRoomsImages()
{
	FreeRoomsImages();

	for(INT y = 0; y < 6; ++y)
	{
		for(INT x = 0; x < 8; ++x)
		{
			if ( m_nLevel.bLayout[x + 8 * y] )
			{
				m_nLevel.pbRooms[y][x] = new CBitmap;
				m_nLevel.pbRooms[y][x]->Create(256, 256);
				NES_ROOM & room = m_nRooms[m_nLevel.bLayout[x + 8 * y] - 1];

				for(INT by = 0; by < 8; ++by)
				{
					for(INT bx = 0; bx < 8; ++bx)
					{
						WriteImage(m_nLevel.pbRooms[y][x], 32 * bx, 32 * by, room.bRoom[by][bx]);
					}
				}
			}
		}
	}
	return TRUE;
}

BOOL CNesEditor::WriteImage(CBitmap * pbmp, INT bx, INT by, INT block, BYTE npal)
{
	COLORREF * pb = (COLORREF*) pbmp->LockRawBits();
	const INT width = pbmp->Width();

	if ( block >= (INT) m_nLevel.nMajorBlocks.size() )
		return FALSE;

	BYTE bShowMask = ( ( ( m_nGame.bDrawMask & NEDF_SHOWTILEMASK ) == NEDF_SHOWTILEMASK ) ? 0xFF : 0 );

	for(INT y = 0; y < 32; ++y)
	{
		for(INT x = 0; x < 32; ++x)
		{
			size_t k = x + 32 * y;
			if ( m_nLevel.nBlocks[block].size() <= k )
				continue;

			BYTE b = m_nLevel.nBlocks[block][k];
			BYTE mask = (bShowMask & ( b & 0xF0 ));
			b &= 0x0F;

			COLORREF & px = pb[(bx + x) + width * (by + y)];
			WRGBQUAD & w = *((WRGBQUAD*) &px);
			px = g_crNesColor[ m_nLevel.bPalette[ b ] ];
			if ( mask )
			{
				if ( mask & 0x20 ) w.rgbRed |= 0xc0; //LOBYTE(100 * INT( 0x80 - INT(w.rgbRed)) / 255) + w.rgbRed;
				if ( mask & 0x40 ) w.rgbGreen |= 0xc0; //LOBYTE(100 * INT( 0x80 - INT(w.rgbGreen)) / 255) + w.rgbGreen;
				if ( mask & 0x80 ) w.rgbBlue |= 0xc0; //LOBYTE(100 * INT( 0x80 - INT(w.rgbBlue)) / 255) + w.rgbBlue;
			}
		}
	}

	return TRUE;
}

BOOL CNesEditor::WriteTile(std::vector<BYTE> & ppMajorBlock, INT tx, INT ty, INT tile, BYTE nPal)
{
	tile += 256;

	for(INT y = 0; y < 8; ++y)
	{
		for(INT x = 0; x < 8; ++x)
		{
			ppMajorBlock[tx + x + 32 * (ty + y)] = 4 * nPal + m_nLevel.nTiles[tile].pix[x][y].clr;
		}
	}

	return TRUE;
}

BOOL CNesEditor::DrawRoomObjects(HDC hDC, INT x, INT y, INT nRoomX, INT nRoomY, BYTE fDrawSteps)
{
	if ( nRoomX < 0 || nRoomY < 0 || nRoomX >= 8 || nRoomY >= 6 )
		return FALSE;

	if ( m_nLevel.pbRooms[nRoomY][nRoomX] || ( NEDF_SHOWOUTSIDEOBJECTS & m_nGame.bDrawMask ))
	{
		const int xdelta = -64, ydelta = -65;

		if ( size_t(nRoomY) < m_nLevel.nObjects.size() )
		{
			size_t size = m_nLevel.nObjects[nRoomY].size();

			for(size_t i = 0; i < size; ++i)
			{
				NES_OBJECT & o = m_nLevel.nObjects[nRoomY][i];
				INT ox = INT(o.objectX) * 8;
				INT oid = o.id;

				if ( 0 == ( NEDF_SHOWSPECOBJ & m_nGame.bDrawMask ) && oid >= 0x99 )
					continue;

				if ( oid >= 0x9d )
				{
					oid -= 0x9d;
					if ( oid / 8 + m_nCurrentLevel >= NES_COUNT_LEVELS )
					{
						if ( !m_nGame.fExtended )
						{
							oid = 0x100;
						}
						else
						{
							oid = oid - 8 * ( NES_COUNT_LEVELS - m_nCurrentLevel );
							if ( oid / 8 >= NES_COUNT_LEVELS )
								oid = 0x100;
							else
								oid += 0x9d;
						}
					}
					else
						oid += 0x9d + m_nCurrentLevel * 8;
				}

				if ( oid == 0x5e )
					oid = m_nGame.nBossTypes[m_nCurrentLevel];

				if ( o.screen == nRoomX )
				{
					CBitmap & bmo = *m_vpbmObj[oid];

					if ( fDrawSteps && oid == fDrawSteps + 0x7B )
					{
						CBitmap bmBg;
						bmBg.Create(bmo.Width(), bmo.Height());
						bmBg.FillRect(0, 0, bmBg.Width(), bmBg.Height(), RGB(0, 255, 0), 0, 0);
						bmBg.RenderRawBitmap(bmo.LockRawBits(), bmo.Width(), bmo.Height(), 150, FALSE,
							0, 0, bmBg.Width(), bmBg.Height(), 0, 0, 0);

						COLORREF clrTransparent = *((COLORREF*) bmBg.LockRawBits());
						clrTransparent = XCHGCOLORREF( clrTransparent ) & 0x00FFFFFF;

						TransparentBlt(hDC, x + ox + xdelta, y + INT(o.objectY) + ydelta, 
							bmBg.Width(), bmBg.Height(), bmBg.LockDC(), 0, 0, bmBg.Width(), bmBg.Height(), clrTransparent);
						bmBg.FreeDC();
					}
					else
					{
						TransparentBlt(hDC, x + ox + xdelta, y + INT(o.objectY) + ydelta, 
							bmo.Width(), bmo.Height(), bmo.LockDC(), 0, 0, bmo.Width(), bmo.Height(), RGB(168, 228, 252));
						bmo.FreeDC();
					}

					if ( 0 != ( NEDF_SHOWSPECOBJ & m_nGame.bDrawMask )  )
					{
						RECT rc = { x + ox, y + INT(o.objectY), 0, 0 };
						rc.right = rc.left + 5; rc.bottom = rc.top + 5;
						FillRect(hDC, &rc, GetStockBrush(BLACK_BRUSH));
					}
				}				
			}
		}
		
		if ( fDrawSteps )
		{
			CBitmap & bmo = *m_vpbmObj[0x89];
			CFont fnt(-10, TRUE, FALSE, FALSE, FALSE, TEXT("Tahoma"), 0);

			HGDIOBJ holdfn = SelectObject(hDC, fnt);
			COLORREF clrText = GetTextColor(hDC);
			int bkMode = GetBkMode(hDC);
			SetTextColor(hDC, RGB(255, 255, 255));
			SetBkMode(hDC, TRANSPARENT);
			int lvl = ( m_nGame.fExtended ? m_nCurrentLevel : 0 );

			size_t count = m_nGame.nSteps[lvl][fDrawSteps - 1].size();
			for(size_t n = 0; n < count; ++n)
			{
				NES_POINT & st = m_nGame.nSteps[lvl][fDrawSteps - 1][n];

				if ( nRoomX == st.bScrX && nRoomY == st.bScrY )
				{
					TransparentBlt(hDC, x + st.bPosX + xdelta, y + st.bPosY + ydelta, 
						bmo.Width(), bmo.Height(), bmo.LockDC(), 0, 0, bmo.Width(), bmo.Height(), RGB(168, 228, 252));

					RECT rcText = { x + st.bPosX - 8, y + st.bPosY - 8, 0, 0 };
					rcText.right = rcText.left + 32; rcText.bottom = rcText.top + 32;

					CString s;
					s.Format(TEXT("#%d"), n + 1);

					DrawText(hDC, s, s.GetLength(), &rcText, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

					bmo.FreeDC();
				}
			}

			SetBkMode(hDC, bkMode);
			SetTextColor(hDC, clrText);
			SelectObject(hDC, holdfn);
		}

		NES_LEVEL_HEADER & lh = m_nBank.nHeader.nHeader[m_nLvlBanks[m_nCurrentLevel].nHeaderPtr];
		POINT pt = { 0 };
		INT id = 0;
		if ( nRoomX == lh.start.bScrX &&
			 nRoomY == lh.start.bScrY )
		{
			id = 1;
			
			pt.x = x + lh.start.bPosX + xdelta;
			pt.y = y + lh.start.bPosY + ydelta - 16;
		}
		
		if ( nRoomX == lh.restart.bScrX &&
			 nRoomY == lh.restart.bScrY )
		{
			id = 2;

			pt.x = x + lh.restart.bPosX + xdelta;
			pt.y = y + lh.restart.bPosY + ydelta - 16;
		}

		if ( nRoomX == lh.bossrestart.bScrX &&
			 nRoomY == lh.bossrestart.bScrY )
		{
			id = 3;

			pt.x = x + lh.bossrestart.bPosX + xdelta;
			pt.y = y + lh.bossrestart.bPosY + ydelta - 16;
		}

		if ( id >= 1 && id <= 3 )
		{
			CBitmap & bmo = *m_vpbmScr[id];
			TransparentBlt(hDC, pt.x, pt.y, bmo.Width(), bmo.Height(), bmo.LockDC(), 0, 0, bmo.Width(), bmo.Height(), RGB(168, 228, 252));
			bmo.FreeDC();
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CNesEditor::DrawRoom(HDC hDC, INT x, INT y, INT nRoomX, INT nRoomY)
{
	if ( nRoomX < 0 || nRoomY < 0 || nRoomX >= 8 || nRoomY >= 6 )
		return FALSE;

	if ( m_nLevel.pbRooms[nRoomY][nRoomX] )
	{
		BitBlt(hDC, x, y, x + 256, y + 256, m_nLevel.pbRooms[nRoomY][nRoomX]->LockDC(), 0, 0, SRCCOPY);
		m_nLevel.pbRooms[nRoomY][nRoomX]->FreeDC();

		if ( 0 != ( m_nGame.bDrawMask & NEDF_SHOWROOMBORDERS ) )
		{
			HPEN hGrPen = CreatePen(PS_SOLID, 1, RGB(0x80, 0x80, 0x80));
			HGDIOBJ hOldbr = SelectObject(hDC, GetStockBrush(NULL_BRUSH));
			HGDIOBJ hOldpn = SelectObject(hDC, GetStockPen(WHITE_PEN));
			RECT rc = { x, y, x + 256, y + 256 };
			Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);
			SelectObject(hDC, hGrPen);
			SetRect(&rc, x, y, x + 255, y + 255);
			Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);
			SelectObject(hDC, hOldpn);
			SelectObject(hDC, hOldbr);
			::DeleteObject(hGrPen);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CNesEditor::DrawBridge(HDC hDC, INT x, INT y, INT nRoomX, INT nRoomY)
{
	if ( nRoomX < 0 || nRoomY < 0 || nRoomX >= 8 || nRoomY >= 6 )
		return FALSE;

	NES_BRIDGE & nb = m_nBank.nBridgeData;
	WORD bStartRoom = nb.ptLeft.bScrX;
	WORD bEndRoom = nb.ptRight.bScrX;
	const int xdelta = -56, ydelta = -51;

	if ( m_nLevel.pbRooms[nRoomY][nRoomX] && m_nBank.nBridgeData.ptLeft.bScrY == nRoomY &&
		nRoomX >= bStartRoom && nRoomX <= bEndRoom )
	{
		INT bStartX = 0, bEndX = 16, oy = nb.ptLeft.bPosY / 16;
		if ( nRoomX == bStartRoom )
			bStartX = nb.ptLeft.bPosX / 16;
		if ( nRoomX == bEndRoom )
			bEndX = (nb.ptRight.bPosX + 1) / 16;

		CBitmap & bmo = *m_vpbmObj[nb.bObjectId];
		HDC hdc = bmo.LockDC();

		for(int ox = bStartX; ox < bEndX; ++ox)
		{
			POINT pt = { x + 16*ox, y + 16*oy };
			TransparentBlt(hDC, pt.x + xdelta, pt.y + ydelta, 
				bmo.Width(), bmo.Height(), hdc,
				0, 0, bmo.Width(), bmo.Height(), RGB(168, 228, 252));
		}
		bmo.FreeDC();

		return TRUE;
	}

	return FALSE;
}

BOOL CNesEditor::DrawMuSecret(HDC hDC, INT x, INT y, INT nRoomX, INT nRoomY)
{
	if ( !m_fLoaded )
		return FALSE;
	
	if ( !IsValidMuSecretRoom(nRoomX, nRoomY) )
		return FALSE;

	NES_MU_SECRET & nmu = m_nGame.nMuSecret;

	CBitmap bg;
	CBitmap & bmo = *m_vpbmObj[nmu.bObjId];
	RECT rc = m_rcObj[nmu.bObjId];
	SIZE sz = { rc.right - rc.left, rc.bottom - rc.top};
	const int xdelta = rc.left - 64, ydelta = -sz.cy + 1;

	bg.Create(sz.cx, sz.cx);
	bg.FillRect(0, 0, bg.Width(), bg.Height(), RGB(0xff, 0, 0), 0, 0);
	bg.RenderRawBitmap(bmo.LockRawBits(), bmo.Width(), bmo.Height(), 170, FALSE,
		rc.left, rc.top, sz.cx, sz.cy, 0, 0, 0);

	if ( !nmu.fVertical )
	{
		
		size_t vcount = nmu.vobjX.size();
		HDC hbmdc = bg.LockDC(), hobjdc = bmo.LockDC();
		for(size_t n = 0; n < vcount; ++n)
		{
			BitBlt(hDC, x + nmu.vobjX[n].bPos + xdelta, y + nmu.bPosY + ydelta, sz.cx, sz.cy, hbmdc, 0, 0, SRCCOPY);
		}
		bmo.FreeDC();
		bg.FreeDC();
	}
	else
	{
		size_t vcount = nmu.vobjX.size();
		HDC hbmdc = bg.LockDC(), hobjdc = bmo.LockDC();
		for(size_t n = 0; n < vcount; ++n)
		{
			BitBlt(hDC, x + nmu.bPosY, y + nmu.vobjX[n].bPos - sz.cy, sz.cx, sz.cy, hbmdc, 0, 0, SRCCOPY);
		}
		bmo.FreeDC();
		bg.FreeDC();
	}

	return TRUE;
}

BOOL CNesEditor::SetDrawMask(BYTE bMask)
{
	m_nGame.bDrawMask = bMask;
	if ( !m_fLoaded )
		return FALSE;

	return ( CacheRoomsImages() );
}