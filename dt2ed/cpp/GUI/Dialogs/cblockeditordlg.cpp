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

CBlockEditorDlg::CBlockEditorDlg(HINSTANCE hInstance, BOOL fExtended, BOOL fPaste)
	: CDialog(hInstance), m_fExtended( fExtended ), m_fPaste( fPaste ), m_wCatched( 0xFFFF )
{
	m_pt.x = m_pt.y = 0;
}

CBlockEditorDlg::CBlockEditorDlg(HINSTANCE hInstance, std::vector<BYTE> & vUsedBlocks, BOOL fExtended, BOOL fPaste)
	: CDialog(hInstance), m_fExtended( fExtended ), m_fPaste( fPaste ), m_wCatched( 0xFFFF ), m_vUsedBlocks( vUsedBlocks )
{
	m_pt.x = m_pt.y = 0;
}

CBlockEditorDlg::~CBlockEditorDlg()
{
	FreeBlocks();
	FreeTilesImages();
}

VOID CBlockEditorDlg::OnPreInit(CDialogTemplate & d)
{
	d.SetDlgTitle(TEXT("Blocks list"));
	d.SetDlgStyle(DS_SHELLFONT | WS_BORDER | WS_CAPTION, 0);
	d.SetDlgPos(0, 0, 370, 300);

	d.AddPreItem(IDC_BLOCKS, TEXT("SysLevel32"), NULL, 7, 7, -7, -(7 + 15 + 7), WS_BORDER);

	if ( !m_fPaste )
		d.AddPreItem(IDC_EDIT, cButton, TEXT("Edit block #00..."), 7, -(7 + 15), 75, 15, BS_PUSHBUTTON);
	else
		d.AddPreItem(IDC_EDIT, cEdit, NULL, 7, -(7 + 15), 75, 15, ES_READONLY | ES_AUTOHSCROLL);

	if ( m_fExtended && !m_fPaste )
	{
		d.AddPreItem(IDC_EDIT + 1, cButton, TEXT("New block..."), (7 + 75 + 7), -(7 + 15), 50, 15, BS_PUSHBUTTON);
		d.AddPreItem(IDC_EDIT + 2, cButton, TEXT("Delete last..."), (7 + 75 + 7 + 50 + 7), -(7 + 15), 50, 15, BS_PUSHBUTTON);
	}


	d.AddPreItem(IDOK, cButton, MB_GetString(IDOK - 1), -(7 + 50 + 7 + 50), -(7 + 15), 50, 15, BS_DEFPUSHBUTTON);
	d.AddPreItem(IDCANCEL, cButton, MB_GetString(IDCANCEL - 1), -(7 + 50), -(7 + 15), 50, 15);
}

VOID CBlockEditorDlg::OnInit(LPARAM lParam)
{
	m_Canvas.InitBufView( 
		(CBufView*) dSendMessage(IDC_BLOCKS, BVM_GETINSTANCE), 
		512, 32 * (m_block.vMajor.size() / 16 + ( m_block.vMajor.size() % 16 ? 1 : 0 ) )
		);

	m_Canvas.SetCursorSet(0, 0xFFFFFF, 32, 32, 32, 32);
	m_Canvas.SetActiveCursor(0);
	m_Canvas.SetCursorPosition(m_pt.x, m_pt.y);

	UpdateBlocks();
}

VOID CBlockEditorDlg::OnButton(USHORT id)
{
	if ( IDC_EDIT == id )
	{
		CBlockDlg dlg(GetInstance());
		NES_BLOCK & block = m_block.vMajor[m_pt.x + 16 * m_pt.y];
		dlg.Let_Block(m_vpimg, block, m_block.bBgTiles);
		if ( dlg.Show(this) )
		{
			dlg.Get_Block( block, m_block.bBgTiles );
			UpdateBlocks();
			SetFocus(m_Canvas);
		}
	}
	else if ( ( IDC_EDIT + 1 ) == id )
	{
		NES_BLOCK block = { 0 };
		m_block.vMajor.push_back(block);
		UpdateBlocks();
		size_t max_id = m_block.vMajor.size() - 1;

		m_Canvas.SetCursorPosition( max_id % 16, max_id / 16 );
		SetFocus(m_Canvas);
	}
	else if ( ( IDC_EDIT + 2 ) == id )
	{
		if ( m_block.vMajor.size() > 1 )
		{
			m_block.vMajor.erase( m_block.vMajor.end() - 1 );
			m_Canvas.SetCursorPosition(0, 0);
			UpdateBlocks();
			SetFocus(m_Canvas);
		}
	}
}

VOID CBlockEditorDlg::OnOK()
{
	Close(1);
}

INT_PTR CBlockEditorDlg::OnNotify(LPNMHDR lpnm)
{
	if ( IDC_BLOCKS == lpnm->idFrom && CVN_REQUESTPOINT == lpnm->code )
	{
		PCVNREQPOS pcvn = (PCVNREQPOS) lpnm;

		if ( pcvn->pt.x > 16 || size_t(pcvn->pt.x + 16 * pcvn->pt.y) >= m_block.vMajor.size() )
			return 1;

		CString s;
		s.Format(TEXT("%slock #%02X..."), ( m_fPaste ? TEXT("B") : TEXT("Edit b") ), pcvn->pt.x + 16 * pcvn->pt.y);
		SetCtlText(IDC_EDIT, s);

		m_pt = pcvn->pt;
	}
	else if ( IDC_BLOCKS == lpnm->idFrom && CVN_REQUESTMODIFY == lpnm->code && !m_fPaste )
	{
		PCVNREQMODIFY pcvn = (PCVNREQMODIFY) lpnm;
		BOOL fCtrl = HIWORD(GetKeyState(VK_CONTROL));
		if ( pcvn->key == -1 && !pcvn->delta )
		{
			if ( !fCtrl && 0xFFFF != m_wCatched && m_wCatched < LOWORD(m_block.vMajor.size()) )
			{
				m_block.vMajor[m_pt.x + 16 * m_pt.y] = m_block.vMajor[m_wCatched];
				UpdateBlocks();
			}
			else if ( fCtrl )
			{
				m_wCatched = LOWORD(m_pt.x + 16 * m_pt.y);				
			}
		}
	}
	return 0;
}

VOID CBlockEditorDlg::Let_Block(BYTE bBlock)
{
	m_pt.x = bBlock % 16;
	m_pt.y = bBlock / 16;
	m_Canvas.SetCursorPosition(m_pt.x, m_pt.y);
}

VOID CBlockEditorDlg::Get_Block(BYTE & bBlock)
{
	bBlock = LOBYTE(m_pt.x + 16 * m_pt.y);
}

VOID CBlockEditorDlg::Let_Blocks(PBYTE pbPalette, PBYTE pbPalTiles, 
									std::vector<NES_TILE> & vt, 
									std::vector<NES_BLOCK> & vmn,
									std::vector<NES_BLOCK> & vmj,
									PBYTE pbBgTiles
									)
{
	CopyMemory(m_pal.bPalette, pbPalette, sizeof(m_pal.bPalette));
	CopyMemory(m_pal.bPalTiles, pbPalTiles, sizeof(m_pal.bPalTiles));
	CopyMemory(m_block.bBgTiles, pbBgTiles, sizeof(m_block.bBgTiles));

	m_block.vTiles = vt;
	m_block.vMinor = vmn;
	m_block.vMajor = vmj;

	CreateTilesImages();
}

VOID CBlockEditorDlg::UpdateBlocks()
{
	if ( m_vpBlock.size() > 0 )
		FreeBlocks();

	m_Canvas.ResizeBufView(
		512, 32 * (m_block.vMajor.size() / 16 + ( m_block.vMajor.size() % 16 ? 1 : 0 ) )
		);

	size_t size = m_block.vMajor.size();

	CBitmap bmGray, bmRender;
	bmGray.Create(32, 32);
	bmRender.Create(bmGray.Width(), bmGray.Height());
	bmGray.FillRect(0, 0, bmGray.Width(), bmGray.Height(), RGB(0x20, 0x20, 0x20), 0, 0, 170);
	HDC hrender = bmRender.LockDC();

	for(size_t i = 0; i < size; ++i)
	{
		CBitmap * pbmp = new CBitmap;

		pbmp->Create(32, 32);
		HDC hdc = pbmp->LockDC();

		BitBlt(hdc, 0, 0, 16, 16, *m_vpimg[m_block.vMajor[i].bUpLeft], 0, 0, SRCCOPY);
		BitBlt(hdc, 16, 0, 16, 16, *m_vpimg[m_block.vMajor[i].bUpRight], 0, 0, SRCCOPY);
		BitBlt(hdc, 0, 16, 16, 16, *m_vpimg[m_block.vMajor[i].bBtLeft], 0, 0, SRCCOPY);
		BitBlt(hdc, 16, 16, 16, 16, *m_vpimg[m_block.vMajor[i].bBtRight], 0, 0, SRCCOPY);

		m_vpBlock.push_back(pbmp);

		if ( i < m_vUsedBlocks.size() && !m_vUsedBlocks[i] )
		{			
			BitBlt(hrender, 			
				0, 0, 
				pbmp->Width(), pbmp->Height(), 
				pbmp->LockDC(), 0, 0, SRCCOPY);
			bmRender.RenderRawBitmap(bmGray.LockRawBits(), bmGray.Width(), bmGray.Height(), 255, TRUE, 0, 0, bmGray.Width(), bmGray.Height(), 0, 0, 0);
			BitBlt(m_Canvas, 			
				pbmp->Width() * (i % 16), pbmp->Height() * (i / 16), 
				pbmp->Width(), pbmp->Height(), 
				hrender, 0, 0, SRCCOPY);
		}
		else
			BitBlt(m_Canvas, 			
				pbmp->Width() * (i % 16), pbmp->Height() * (i / 16), 
				pbmp->Width(), pbmp->Height(), 
				pbmp->LockDC(), 0, 0, SRCCOPY);

		pbmp->FreeDC();
	}

	bmRender.FreeDC();
	m_Canvas.Update();
}

VOID CBlockEditorDlg::FreeBlocks()
{
	size_t size = m_vpBlock.size();
	for(size_t i = 0; i < size; ++i)
		delete m_vpBlock[i];

	m_vpBlock.clear();
}


VOID CBlockEditorDlg::Get_Blocks(PBYTE pbPalette, PBYTE pbPalTiles, 
									std::vector<NES_TILE> & vt, 
									std::vector<NES_BLOCK> & vmn,
									std::vector<NES_BLOCK> & vmj,
									PBYTE pbBgTiles
									)
{
	CopyMemory(pbPalette, m_pal.bPalette, sizeof(m_pal.bPalette));
	CopyMemory(pbPalTiles, m_pal.bPalTiles, sizeof(m_pal.bPalTiles));
	CopyMemory(pbBgTiles, m_block.bBgTiles, sizeof(m_block.bBgTiles));
	vt = m_block.vTiles;
	vmn = m_block.vMinor;
	vmj = m_block.vMajor;
}

VOID CBlockEditorDlg::CreateTilesImages()
{
	if ( m_vpimg.size() > 0 )
		FreeTilesImages();

	size_t size = m_block.vMinor.size();

	for(size_t i = 0; i < size; ++i)
	{
#define		PAL( n ) (( m_pal.bPalTiles[( n ) / 4] >> ( 2 * ( ( ( n ) % 4 ) ) ) ) & 3)

		CNesTileImg * pimg = 
			new CNesTileImg(m_block.vTiles, m_block.vMinor[i], &m_pal.bPalette[ 4 * PAL(i) ]);

		m_vpimg.push_back(pimg);
#undef PAL
	}
}

VOID CBlockEditorDlg::FreeTilesImages()
{
	size_t size = m_vpimg.size();
	for(size_t i = 0; i < size; ++i)
		delete m_vpimg[i];

	m_vpimg.clear();
}

// -- block

VOID CBlockDlg::OnPreInit(CDialogTemplate & d)
{
	d.SetDlgTitle(m_fSelTileMode ? TEXT("Background tile editor (GLOBAL SETTINGS!)") : TEXT("Block editor"));
	d.SetDlgStyle(DS_SHELLFONT | WS_BORDER | WS_CAPTION | WS_OVERLAPPED, 0);
	d.SetDlgPos(0, 0, 440, 370);

	d.AddPreItem(IDC_BLOCKS, TEXT("SysLevel32"), NULL, 7, 7, -(7 + 60 + 7), -(7 + 15 + 7), WS_BORDER);
	
	if ( !m_fSelTileMode )
	{
		d.AddPreItem(IDC_BLOCKS + 1, TEXT("SysLevel32"), NULL, -(7 + 60), 7, 60, 60, WS_BORDER);
		d.AddPreItem(0x1011, cStatic, NULL, -(7 + 60), 80, 60, 60, SS_BITMAP | WS_BORDER, FALSE, TRUE, FALSE);
		d.AddPreItem(0x1012, cButton, TEXT("Edit bg..."), -(7 + 60), 150, 60, 15, BS_PUSHBUTTON, FALSE, TRUE, FALSE);
	}

	d.AddPreItem(0x1010, cStatic, TEXT("Tile #00"), 7, -(7 + 15), 50, 15);

	d.AddPreItem(IDOK, cButton, MB_GetString(IDOK - 1), -(7 + 50 + 7 + 50), -(7 + 15), 50, 15, BS_DEFPUSHBUTTON);
	d.AddPreItem(IDCANCEL, cButton, MB_GetString(IDCANCEL - 1), -(7 + 50), -(7 + 15), 50, 15);
}

VOID CBlockDlg::OnInit(LPARAM lParam)
{
	m_cvTiles.InitBufView( (CBufView*) dSendMessage(IDC_BLOCKS, BVM_GETINSTANCE), 512, 512 );
	m_cvBlock.InitBufView( (CBufView*) dSendMessage(IDC_BLOCKS + 1, BVM_GETINSTANCE), 64, 64 );

	m_cvTiles.SetCursorSet(0, 0xFF00FF, 32, 32, 32, 32);
	m_cvBlock.SetCursorSet(0, 0xFF00FF, 32, 32, 32, 32);

	size_t size = m_vpTiles.size();
	for(size_t i = 0; i < size; ++i)
	{
		int x = ( i % 16 ), y = ( i / 16 );
		StretchBlt(m_cvTiles, x * 32, y * 32, 32, 32, *m_vpTiles[i], 0, 0, 16, 16, SRCCOPY);
	}

	if ( size )
	{
		UpdateBlock();
		m_cvTiles.Update();

		m_cvTiles.SetActiveCursor(0);
		m_cvBlock.SetActiveCursor(0);

		m_cvTiles.SetCursorPosition( m_nBlock.bUpLeft % 16, m_nBlock.bUpLeft / 16 );
	}
}

VOID CBlockDlg::OnOK()
{
	Close(1);
}

VOID CBlockDlg::OnButton(USHORT uId)
{
	INT seltile = m_pt.x + 16 * m_pt.y;

	if ( seltile >= 0xEC && seltile < 0xF8 )
	{
		CBlockDlg dlg(GetInstance(), m_bgTiles[seltile - 0xEC]);
		NES_BLOCK bl = { 0 };
		
		bl.bUpLeft = m_bgTiles[seltile - 0xEC];

		dlg.Let_Block(m_vpTiles, bl, m_bgTiles);

		if ( dlg.Show(this) )
		{
			m_bgTiles[seltile - 0xEC] = LOBYTE(dlg.m_pt.x + 16 * dlg.m_pt.y);
		}

		SetFocus(m_cvTiles);
	}
}

VOID CBlockDlg::UpdateBlock()
{
	StretchBlt(m_cvBlock, 0, 0, 32, 32, *m_vpTiles[m_nBlock.bUpLeft], 0, 0, 16, 16, SRCCOPY);
	StretchBlt(m_cvBlock, 32, 0, 32, 32, *m_vpTiles[m_nBlock.bUpRight], 0, 0, 16, 16, SRCCOPY);
	StretchBlt(m_cvBlock, 0, 32, 32, 32, *m_vpTiles[m_nBlock.bBtLeft], 0, 0, 16, 16, SRCCOPY);
	StretchBlt(m_cvBlock, 32, 32, 32, 32, *m_vpTiles[m_nBlock.bBtRight], 0, 0, 16, 16, SRCCOPY);

	m_cvBlock.Update();
}

CBlockDlg::CBlockDlg(HINSTANCE hInstance)
	: CDialog(hInstance), m_fSelTileMode( 0 )
{
	m_pt.x = m_pt.y = 0;
	m_ptb = m_pt;
}

CBlockDlg::CBlockDlg(HINSTANCE hInstance, BYTE bBgTile)
	: CDialog(hInstance), m_fSelTileMode( 1 )
{
	m_pt.x = bBgTile % 16;
	m_pt.y = bBgTile / 16;
}


VOID CBlockDlg::Let_Block(std::vector<CNesTileImg*> & vpimg, NES_BLOCK & nBlock, PBYTE pbgTiles)
{
	m_vpTiles = vpimg;
	m_nBlock = nBlock;
	CopyMemory(m_bgTiles, pbgTiles, sizeof(m_bgTiles));
}

VOID CBlockDlg::Get_Block(NES_BLOCK & nBlock, PBYTE pbgTiles)
{
	nBlock = m_nBlock;
	CopyMemory(pbgTiles, m_bgTiles, sizeof(m_bgTiles));
}

INT_PTR CBlockDlg::OnNotify(LPNMHDR lpnm)
{
	switch(lpnm->code)
	{
	case CVN_REQUESTPOINT:
		{
			PCVNREQPOS pcvn = (PCVNREQPOS) lpnm;
			switch(lpnm->idFrom)
			{
			case IDC_BLOCKS + 0:
				{
					if ( pcvn->pt.x < 0 || pcvn->pt.y < 0 ||
						pcvn->pt.x >= 16 || pcvn->pt.y >= 16 )
						return 1;

					m_pt = pcvn->pt;

					INT seltile = m_pt.x + 16 * m_pt.y;
					INT tile = m_ptb.x + 2 * m_ptb.y;
					switch(tile)
					{
					case 0: m_nBlock.bUpLeft = LOBYTE(seltile); break;
					case 1: m_nBlock.bUpRight = LOBYTE(seltile); break;
					case 2: m_nBlock.bBtLeft = LOBYTE(seltile); break;
					case 3: m_nBlock.bBtRight = LOBYTE(seltile); break;
					}

					CString s;
					s.Format(TEXT("Tile #%02X"), seltile);
					SetCtlText(0x1010, s);

					UpdateBlock();

					// Visible(0x1011, seltile >= 0xEC && seltile < 0xF8);
					Visible(0x1012, seltile >= 0xEC && seltile < 0xF8);

					return 0;
				}
			case IDC_BLOCKS + 1:
				{
					if ( pcvn->pt.x < 0 || pcvn->pt.y < 0 ||
						pcvn->pt.x >= 2 || pcvn->pt.y >= 2 )
						return 1;

					m_ptb = pcvn->pt;

					INT tile = m_ptb.x + 2 * m_ptb.y;

					BYTE block = 0;
					switch(tile)
					{
					case 0: block = m_nBlock.bUpLeft; break;
					case 1: block = m_nBlock.bUpRight; break;
					case 2: block = m_nBlock.bBtLeft; break;
					case 3: block = m_nBlock.bBtRight; break;
					}

					m_cvTiles.SetCursorPosition( block % 16, block / 16 );
					return 0;
				}
			}
			break;
		}
	}

	return 0;
}