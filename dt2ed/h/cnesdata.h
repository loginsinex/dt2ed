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

typedef enum _tagNES_DATA_TYPE
{
	dtNull,
	dtByte,
	dtWord,
	dtLong,
	dtStream
} NES_DATA_TYPE, *PNES_DATA_TYPE;

typedef struct _tagNES_DATA_PATCH
{
	NES_DATA_TYPE		dType;
	BYTE				nBank;
	WORD				ptr;
	std::vector<BYTE>	vData;
} NES_DATA_PATCH, *PNES_DATA_PATCH;

typedef struct _tagNES_DATA
{
	LPARAM				local;

	CString				sName;

	NES_DATA_PATCH		nd;
	NES_DATA_PATCH		ndExtended;
} NES_DATA, *PNES_DATA;

class CNesDataFile
{
	std::vector<NES_DATA>				m_vData;
	
	BOOL								Data2Stream(std::vector<BYTE> & vStream);
	BOOL								Stream2Data(std::vector<BYTE> & vStream);

public:
	VOID				Let_Data(std::vector<NES_DATA> & vData);
	VOID				Get_Data(std::vector<NES_DATA> & vData);

	BOOL				SaveTo(LPCTSTR pszFile);
	BOOL				OpenFrom(LPCTSTR pszFile);
	BOOL				OpenFrom(HINSTANCE hInstance, LPCTSTR pszType, LPCTSTR pszName);
};