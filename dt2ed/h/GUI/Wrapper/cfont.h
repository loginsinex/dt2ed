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

class CFont
{
	HFONT			m_hFont;

public:
	CFont();
	CFont(INT iHeight, BOOL bBold, BOOL bItalic, BOOL bStriked, BOOL bUnderline, LPTSTR lpszFontFace, UINT fQuality);
	
	void	Create(INT iHeight, BOOL bBold, BOOL bItalic, BOOL bStriked, BOOL bUnderline, LPTSTR lpszFontFace, UINT fQuality);
	void	Release();

	void	LoadFromWindow(HWND hWndFrom, INT iHeight = -1, INT bBold = -1, INT bItalic = -1, INT bStriked = -1, INT bUnderline = -1, LPTSTR lpszFontFace = NULL, UINT fQuality = -1);

	HFONT Font();
	operator HFONT();
	~CFont();
};