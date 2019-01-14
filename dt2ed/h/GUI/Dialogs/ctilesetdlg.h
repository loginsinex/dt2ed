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

class CTileSetDlg: public CDialog
{
	CNesTiles &					m_nLevel;
	std::vector<NES_BLOCK> &	m_nMinor;
	CBufView *					m_pBufView;
	CWinPaneScroller			m_scroll;
	CDCBuffer					m_dc;
	BYTE						m_bPalTiles[64];


	static LRESULT CALLBACK		BufViewProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, PVOID pData);
	LRESULT						BVProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	VOID						OnPreInit(CDialogTemplate & d);
	VOID						OnInit(LPARAM lParam);
	VOID						Update();

public:
	CTileSetDlg(HINSTANCE hInstance, CNesTiles & nLevel, std::vector<NES_BLOCK> & nMinor, PBYTE nPalTiles);
};