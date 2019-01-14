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

CNesTileImg::CNesTileImg(std::vector<NES_TILE> & vt, NES_BLOCK & block, PBYTE pbPalette)
{
	m_bmImage.Create(16, 16);

	COLORREF * pclr = (COLORREF*) m_bmImage.LockRawBits();

	WriteTile( vt[0x100 + block.bUpLeft], 0, 0, pclr, pbPalette );
	WriteTile( vt[0x100 + block.bUpRight], 8, 0, pclr, pbPalette );
	WriteTile( vt[0x100 + block.bBtLeft], 0, 8, pclr, pbPalette );
	WriteTile( vt[0x100 + block.bBtRight], 8, 8, pclr, pbPalette );

	m_bmdc = m_bmImage.LockDC();
}

VOID CNesTileImg::WriteTile(NES_TILE & tile, INT tx, INT ty, COLORREF * pclr, PBYTE pbPalette)
{
	for(INT y = 0; y < 8; ++y)
	{
		for(INT x = 0; x < 8; ++x)
		{
			pclr[ (tx + x) + 16 * (ty + y) ] = g_crNesColor[ pbPalette[ tile.pix[x][y].clr ] ];
		}
	}
}

CNesTileImg::~CNesTileImg()
{
	m_bmImage.FreeDC();
}

CNesTileImg::operator HDC()
{
	return m_bmdc;
}