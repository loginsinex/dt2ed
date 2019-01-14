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


class CDataReader
{
	size_t				m_ptr;
	std::vector<BYTE>	m_vData;

public:
	BOOL				IsLoaded();
	VOID				LoadData(std::vector<BYTE> & vData);

	BYTE				PopByte();
	WORD				PopWord();
	DWORD				PopDword();
	size_t				PopData(std::vector<BYTE> & data, size_t size);
	BOOL				eof();
	BOOL				flush();
};

class CTilesCreator
{
	CDataReader				m_vData;

	VOID					DecompressData(std::vector<BYTE> & vData);

public:
	CTilesCreator(HINSTANCE hInstance, LPCTSTR pszResType, LPCTSTR pszRes);
	CTilesCreator(LPCTSTR pszFilename);

	BOOL					LoadTiles(COLORREF clrTransparent, CBitmap ** pbmp);
};

class CTilesImage
{
	CBitmap		&			m_bmImage;

public:
	CTilesImage(CBitmap & bmImage);

	RECT					GetObjectRect();
};
