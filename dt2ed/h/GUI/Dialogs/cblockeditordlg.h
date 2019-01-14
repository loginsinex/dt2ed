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

#define		IDC_BLOCKS				0x1001
#define		IDC_EDIT				0x1002

class CBlockDlg: public CDialog
{
	std::vector<CNesTileImg*>		m_vpTiles;
	NES_BLOCK						m_nBlock;
	CNesCanvas						m_cvTiles;
	CNesCanvas						m_cvBlock;
	POINT							m_pt;
	POINT							m_ptb;
	BOOL							m_fSelTileMode;
	BYTE							m_bgTiles[0x0C];

	VOID							OnPreInit(CDialogTemplate & d);
	VOID							OnInit(LPARAM lParam);
	INT_PTR							OnNotify(LPNMHDR lpnm);
	VOID							OnOK();
	VOID							OnButton(USHORT uId);
	VOID							UpdateBlock();

	CBlockDlg(HINSTANCE hInstance, BYTE bBgBlock);

public:
	CBlockDlg(HINSTANCE hInstance);

	VOID							Let_Block(std::vector<CNesTileImg*> & vpimg, NES_BLOCK & nBlock, PBYTE pbgTiles);
	VOID							Get_Block(NES_BLOCK & nBlock, PBYTE pbgTiles);
};

class CBlockEditorDlg: public CDialog
{
	POINT							m_pt;
	std::vector<CNesTileImg*>		m_vpimg;
	std::vector<CBitmap*>			m_vpBlock;
	std::vector<BYTE>				m_vUsedBlocks;
	
	CNesCanvas						m_Canvas;
	BOOL							m_fExtended;
	BOOL							m_fPaste;
	WORD							m_wCatched;

	struct
	{
		BYTE			bPalette[16];
		BYTE			bPalTiles[64];
	} m_pal;

	struct
	{
		std::vector<NES_TILE>	vTiles;
		std::vector<NES_BLOCK>	vMinor;
		std::vector<NES_BLOCK>	vMajor;
		BYTE					bBgTiles[0x0C];
	} m_block;

	VOID				OnPreInit(CDialogTemplate & d);
	VOID				OnInit(LPARAM lParam);
	VOID				OnButton(USHORT id);
	INT_PTR				OnNotify(LPNMHDR lpnm);
	VOID				OnOK();

	VOID				CreateTilesImages();
	VOID				FreeTilesImages();

	VOID				UpdateBlocks();
	VOID				FreeBlocks();
	

public:
	CBlockEditorDlg(HINSTANCE hInstance, BOOL fCanExtended = FALSE, BOOL fPaste = FALSE);
	CBlockEditorDlg(HINSTANCE hInstance, std::vector<BYTE> & vUsedBlocks, BOOL fCanExtended = FALSE, BOOL fPaste = FALSE);
	~CBlockEditorDlg();

	VOID				Let_Blocks(PBYTE pbPalette, PBYTE pbPalTiles, 
									std::vector<NES_TILE> & vt, 
									std::vector<NES_BLOCK> & vmn,
									std::vector<NES_BLOCK> & vmj,
									PBYTE pbBgTiles
									);

	VOID				Get_Blocks(PBYTE pbPalette, PBYTE pbPalTiles, 
									std::vector<NES_TILE> & vt, 
									std::vector<NES_BLOCK> & vmn,
									std::vector<NES_BLOCK> & vmj,
									PBYTE pbBgTiles
									);

	VOID				Let_Block(BYTE bBlock);
	VOID				Get_Block(BYTE & bBlock);
};