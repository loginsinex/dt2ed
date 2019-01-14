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

class CTileDlg: public CDialog
{
	POINT							m_pt;
	POINT							m_ptb;

	CNesCanvas						m_Canvas;
	CNesCanvas						m_TileCanvas;

	CNesTiles &						m_vTiles;
	NES_BLOCK						m_nBlock;

	struct
	{
		BYTE						bPalette[16];
		BYTE						nPal;
	} m_pal;

	VOID				OnPreInit(CDialogTemplate & d);
	VOID				OnInit(LPARAM lParam);
	INT_PTR				OnNotify(LPNMHDR lpnm);
	VOID				OnOK();
	VOID				OnButton(USHORT uId);

	VOID				ShowTile();
	VOID				ShowSet();

public:
	CTileDlg(HINSTANCE hInstance, CNesTiles & vTiles);

	VOID							Let_Block(NES_BLOCK nBlock, PBYTE pbPalette, BYTE bPal);
	VOID							Get_Block(NES_BLOCK & nBlock, BYTE & bPal);
};

class CTilesEditDlg: public CDialog
{
	POINT							m_pt;
	std::vector<CNesTileImg*>		m_vpimg;
	CNesCanvas						m_Canvas;
	WORD							m_wCatched;
	std::vector<BYTE>				m_vTilesUsed;

	struct
	{
		BYTE			bPalette[16];
		BYTE			bPalTiles[64];
	} m_pal;

	struct
	{
		CNesTiles				vNTiles;
		std::vector<NES_BLOCK>	vMinor;
	} m_block;

	VOID				OnPreInit(CDialogTemplate & d);
	VOID				OnInit(LPARAM lParam);
	VOID				OnButton(USHORT id);
	INT_PTR				OnNotify(LPNMHDR lpnm);
	VOID				OnOK();

	VOID				CreateTilesImages();
	VOID				FreeTilesImages();
	VOID				UpdateTileSet();

public:
	CTilesEditDlg(HINSTANCE hInstance);
	CTilesEditDlg(HINSTANCE hInstance, std::vector<BYTE> & vTilesUsed);
	~CTilesEditDlg();

	VOID				Let_Blocks(PBYTE pbPalette, PBYTE pbPalTiles, 
									CNesTiles & vt, 
									std::vector<NES_BLOCK> & vmn,
									std::vector<NES_BLOCK> & vmj,
									PBYTE pbBgTiles
									);

	VOID				Get_Blocks(PBYTE pbPalette, PBYTE pbPalTiles, 
									CNesTiles & vt, 
									std::vector<NES_BLOCK> & vmn,
									std::vector<NES_BLOCK> & vmj,
									PBYTE pbBgTiles
									);
};

class CTilesBanksDlg: public CDialog
{
	std::map<INT, std::vector<BYTE>>		m_nBank;

	BOOL									m_fCreateOverwriteBank;
	BYTE									m_bBank;

	VOID				OnPreInit(CDialogTemplate & d);
	VOID				OnInit(LPARAM lParam);
	VOID				OnOK();
	VOID				OnButton(USHORT uId);

public:
	CTilesBanksDlg(HINSTANCE hInstance);

	VOID				Let_Info(PBYTE pbTilesBanks);
	VOID				Get_Info(BYTE & nBank, BOOL & fCreateOverwriteBank);
};