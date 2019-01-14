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

class CNesTileImg
{
	CBitmap					m_bmImage;
	HDC						m_bmdc;

	VOID					WriteTile(NES_TILE & tile, INT x, INT y, COLORREF * pclr, PBYTE pbPalette);

public:

	CNesTileImg(std::vector<NES_TILE> & vt, NES_BLOCK & tile, PBYTE pbPalette);
	~CNesTileImg();

	operator HDC();
};