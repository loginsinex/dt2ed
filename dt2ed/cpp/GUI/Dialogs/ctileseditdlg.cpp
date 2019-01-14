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


// editor
CTileDlg::CTileDlg(HINSTANCE hInstance, CNesTiles & vTile)
	: CDialog(hInstance), m_vTiles(vTile)
{
	m_pt.x = m_pt.y = 0;
	m_ptb.x = m_ptb.y = 0;
}

VOID CTileDlg::OnPreInit(CDialogTemplate & d)
{
	d.SetDlgTitle(TEXT("Tile editor"));
	d.SetDlgStyle(DS_SHELLFONT | WS_BORDER | WS_CAPTION | WS_OVERLAPPED, 0);
	d.SetDlgPos(0, 0, 440, 370);

	d.AddPreItem(IDC_BLOCKS, TEXT("SysLevel32"), NULL, 7, 7, -(7 + 60 + 7), -(7 + 15 + 7), WS_BORDER);

	d.AddPreItem(IDC_BLOCKS + 1, TEXT("SysLevel32"), NULL, -(7 + 60), 7, 60, 60, WS_BORDER);

	d.AddPreItem(0x1010, cStatic, TEXT("Tile #00"), 7, -(7 + 15), 50, 15);

	d.AddPreItem(0x1020, cButton, TEXT("Pal #00"), -(7 + 60), 80, 60, 13, BS_AUTORADIOBUTTON);
	d.AddPreItem(0x1021, cButton, TEXT("Pal #01"), -(7 + 60), 95, 60, 13, BS_AUTORADIOBUTTON);
	d.AddPreItem(0x1022, cButton, TEXT("Pal #02"), -(7 + 60), 110, 60, 13, BS_AUTORADIOBUTTON);
	d.AddPreItem(0x1023, cButton, TEXT("Pal #03"), -(7 + 60), 125, 60, 13, BS_AUTORADIOBUTTON);

	d.AddPreItem(IDOK, cButton, MB_GetString(IDOK - 1), -(7 + 50 + 7 + 50), -(7 + 15), 50, 15, BS_DEFPUSHBUTTON);
	d.AddPreItem(IDCANCEL, cButton, MB_GetString(IDCANCEL - 1), -(7 + 50), -(7 + 15), 50, 15);
}

VOID CTileDlg::OnInit(LPARAM lParam)
{
	m_Canvas.InitBufView((CBufView*) dSendMessage(IDC_BLOCKS, BVM_GETINSTANCE), 512, 512);
	m_TileCanvas.InitBufView((CBufView*) dSendMessage(IDC_BLOCKS + 1, BVM_GETINSTANCE), 64, 64);

	m_Canvas.SetCursorSet(0, RGB(0xA0, 0xF0, 0xE0), 32, 32, 32, 32);
	m_TileCanvas.SetCursorSet(0, RGB(0xFF, 0xFF, 0xFF), 32, 32, 32, 32);
	m_Canvas.SetActiveCursor(0);
	m_TileCanvas.SetActiveCursor(0);
	m_TileCanvas.SetCursorPosition(0, 0);

	CheckAll(TRUE, 0x1020 + m_pal.nPal);

	ShowSet();
	ShowTile();
}

VOID CTileDlg::ShowSet()
{
	CBitmap bmpCanvas;
	bmpCanvas.Create(270, 270);
	
	std::vector<NES_BLOCK> not_used;

	HDC hdc = bmpCanvas.LockDC();

	m_vTiles.DrawTileSet(hdc, 0, 0, not_used, 1, &m_pal.bPalette[4*m_pal.nPal]);

	StretchBlt(m_Canvas, 0, 0, 512, 512, hdc, 136, 0, 128, 128, SRCCOPY);

	bmpCanvas.FreeDC();

	m_Canvas.Update();
}

VOID CTileDlg::ShowTile()
{
	std::vector<NES_TILE> vTiles;
	m_vTiles.Get_Tiles(vTiles);
	CNesTileImg img(vTiles, m_nBlock, &m_pal.bPalette[4*m_pal.nPal]);

	StretchBlt(m_TileCanvas, 0, 0, 64, 64, img, 0, 0, 16, 16, SRCCOPY);

	m_TileCanvas.Update();
}

INT_PTR CTileDlg::OnNotify(LPNMHDR lpnm)
{
	if ( CVN_REQUESTPOINT == lpnm->code )
	{
		PCVNREQPOS pc = ( PCVNREQPOS ) lpnm;
		if ( pc->pt.x < 0 || pc->pt.y < 0 )
			return 1;

		BYTE tileId = LOBYTE(m_pt.x + 16 * m_pt.y);
		BYTE tileCr = LOBYTE(m_ptb.x + 2 * m_ptb.y);

		switch(pc->nm.idFrom)
		{
		case IDC_BLOCKS:
			{
				if ( pc->pt.x >= 16 || pc->pt.y >= 16 )
					return 1;

				m_pt = pc->pt;
				tileId = LOBYTE(m_pt.x + 16 * m_pt.y);

				switch(tileCr)
				{
				case 0: m_nBlock.bUpLeft = tileId; break;
				case 1: m_nBlock.bUpRight = tileId; break;
				case 2: m_nBlock.bBtLeft = tileId; break;
				case 3: m_nBlock.bBtRight = tileId;
				}

				ShowTile();

				break;
			}
		case IDC_BLOCKS + 1:
			{
				if ( pc->pt.x >= 2 || pc->pt.y >= 2 )
					return 1;

				m_ptb = pc->pt;
				tileCr = LOBYTE(m_ptb.x + 2 * m_ptb.y);

				switch(tileCr)
				{
				case 0: tileId = m_nBlock.bUpLeft; break;
				case 1: tileId = m_nBlock.bUpRight; break;
				case 2: tileId = m_nBlock.bBtLeft; break;
				case 3: tileId = m_nBlock.bBtRight; break;
				}

				m_Canvas.SetCursorPosition(tileId % 16, tileId / 16);
				break;
			}
		}

		CString s;
		s.Format(TEXT("Tile #%02X"), tileId);
		SetCtlText(0x1010, s);
	}
	return 0;
}

VOID CTileDlg::OnOK()
{
	Close(1);
}

VOID CTileDlg::OnButton(USHORT uId)
{
	if ( uId >= 0x1020 && uId <= 0x1023 )
	{
		m_pal.nPal = LOBYTE( uId - 0x1020 );
		ShowSet();
		ShowTile();
	}
}

VOID CTileDlg::Let_Block(NES_BLOCK nBlock, PBYTE pbPalette, BYTE bPal)
{
	m_nBlock = nBlock;
	CopyMemory(m_pal.bPalette, pbPalette, sizeof(m_pal.bPalette));
	m_pal.nPal = bPal;
}

VOID CTileDlg::Get_Block(NES_BLOCK & nBlock, BYTE & bPal)
{
	nBlock = m_nBlock;
	bPal = m_pal.nPal;
}


// list
CTilesEditDlg::CTilesEditDlg(HINSTANCE hInstance)
	: CDialog(hInstance)
{

}

CTilesEditDlg::CTilesEditDlg(HINSTANCE hInstance, std::vector<BYTE> & vTilesUsed)
	: CDialog(hInstance), m_vTilesUsed( vTilesUsed )
{

}

CTilesEditDlg::~CTilesEditDlg()
{
	FreeTilesImages();
}


VOID CTilesEditDlg::OnPreInit(CDialogTemplate & d)
{
	d.SetDlgTitle(TEXT("Tiles list"));
	d.SetDlgStyle(DS_SHELLFONT | WS_BORDER | WS_CAPTION, 0);
	d.SetDlgPos(0, 0, 370, 370);

	d.AddPreItem(IDC_BLOCKS, TEXT("SysLevel32"), NULL, 7, 7, -7, -(7 + 15 + 7), WS_BORDER);
	d.AddPreItem(IDC_EDIT, cButton, TEXT("Edit tile #00..."), 7, -(7 + 15), 75, 15, BS_PUSHBUTTON);
	
	d.AddPreItem(IDOK, cButton, MB_GetString(IDOK - 1), -(7 + 50 + 7 + 50), -(7 + 15), 50, 15, BS_DEFPUSHBUTTON);
	d.AddPreItem(IDCANCEL, cButton, MB_GetString(IDCANCEL - 1), -(7 + 50), -(7 + 15), 50, 15);
}

VOID CTilesEditDlg::UpdateTileSet()
{
	CreateTilesImages();
	size_t count = m_block.vMinor.size();

	CBitmap bmGray, bmRender;
	bmGray.Create(16, 16);
	bmRender.Create(bmGray.Width(), bmGray.Height());
	bmGray.FillRect(0, 0, bmGray.Width(), bmGray.Height(), RGB(0x20, 0x20, 0x20), 0, 0, 170);
	HDC hrender = bmRender.LockDC();
	
	for(size_t n = 0; n < count; ++n)
	{
		INT x = n % 16, y = n / 16;

		if ( n < m_vTilesUsed.size() && !m_vTilesUsed[n] )
		{
			BitBlt(hrender, 			
				0, 0, 
				bmRender.Width(), bmRender.Height(), 
				(*m_vpimg[n]), 0, 0, SRCCOPY);
			bmRender.RenderRawBitmap(bmGray.LockRawBits(), bmGray.Width(), bmGray.Height(), 255, TRUE, 0, 0, bmGray.Width(), bmGray.Height(), 0, 0, 0);

			StretchBlt(m_Canvas, x * 32, y * 32, 32, 32, 
				hrender, 0, 0, 16, 16, SRCCOPY);
		}
		else
			StretchBlt(m_Canvas, x * 32, y * 32, 32, 32, 
				(*m_vpimg[n]), 0, 0, 16, 16, SRCCOPY);
	}

	bmRender.FreeDC();

	m_Canvas.Update();
}

VOID CTilesEditDlg::OnInit(LPARAM lParam)
{
	m_Canvas.InitBufView((CBufView*) dSendMessage(IDC_BLOCKS, BVM_GETINSTANCE), 512, 512);
	m_Canvas.SetCursorSet(0, RGB(255, 255, 255), 32, 32, 32, 32);

	m_Canvas.SetActiveCursor(0);
	UpdateTileSet();
}

VOID CTilesEditDlg::OnButton(USHORT id)
{
	if ( IDC_EDIT == id )
	{
		BYTE b = LOBYTE(m_pt.x + 16 * m_pt.y);
		BYTE nPal = (( m_pal.bPalTiles[( b ) / 4] >> ( 2 * ( ( ( b ) % 4 ) ) ) ) & 3 );

		CTileDlg dlg(GetInstance(), m_block.vNTiles);
		dlg.Let_Block(m_block.vMinor[b], m_pal.bPalette, nPal);

		if ( dlg.Show(this) )
		{
			BYTE pal;
			dlg.Get_Block(m_block.vMinor[b], pal);
			BYTE & pb = m_pal.bPalTiles[( b ) / 4];
			pb &= ~(3 << (2 * ( b % 4 )));
			pb |= ( pal << (2 * ( b % 4 ) ) );

			UpdateTileSet();
			SetFocus(m_Canvas);
		}
	}
}

INT_PTR CTilesEditDlg::OnNotify(LPNMHDR lpnm)
{
	if ( CVN_REQUESTPOINT == lpnm->code && IDC_BLOCKS == lpnm->idFrom )
	{
		PCVNREQPOS pc = (PCVNREQPOS) lpnm;
		if ( pc->pt.x < 0 || pc->pt.y < 0 || pc->pt.x >= 16 || pc->pt.y >= 16 )
			return 1;

		CString s;
		s.Format(TEXT("Edit tile #%02X..."), pc->pt.x + pc->pt.y * 16);
		SetCtlText(IDC_EDIT, s);
		m_pt = pc->pt;
		return 0;
	} 
	else if ( CVN_REQUESTMODIFY == lpnm->code && IDC_BLOCKS == lpnm->idFrom )
	{
		PCVNREQMODIFY pc = (PCVNREQMODIFY) lpnm;
		if ( pc->pt.x < 0 || pc->pt.y < 0 || pc->pt.x >= 16 || pc->pt.y >= 16 )
			return 0;

		BOOL fCtrl = HIWORD(GetKeyState(VK_CONTROL));
		if ( pc->key == -1 && !pc->delta )
		{
			if ( !fCtrl && 0xFFFF != m_wCatched )
			{
				m_block.vMinor[m_pt.x + 16 * m_pt.y] = m_block.vMinor[m_wCatched];
				UpdateTileSet();
			}
			else if ( fCtrl )
			{
				m_wCatched = LOWORD(m_pt.x + 16 * m_pt.y);				
			}
		}
	}

	return 0;
}

VOID CTilesEditDlg::OnOK()
{
	Close(1);
}


VOID CTilesEditDlg::CreateTilesImages()
{
	if ( m_vpimg.size() > 0 )
		FreeTilesImages();

	size_t size = m_block.vMinor.size();
	std::vector<NES_TILE> vTiles;
	m_block.vNTiles.Get_Tiles( vTiles );

	for(size_t i = 0; i < size; ++i)
	{
#define		PAL( n ) (( m_pal.bPalTiles[( n ) / 4] >> ( 2 * ( ( ( n ) % 4 ) ) ) ) & 3)

		CNesTileImg * pimg = 
			new CNesTileImg(vTiles, m_block.vMinor[i], &m_pal.bPalette[ 4 * PAL(i) ]);

		m_vpimg.push_back(pimg);
#undef PAL
	}
}
	
VOID CTilesEditDlg::FreeTilesImages()
{
	size_t size = m_vpimg.size();
	for(size_t i = 0; i < size; ++i)
		delete m_vpimg[i];

	m_vpimg.clear();
}

VOID CTilesEditDlg::Let_Blocks(PBYTE pbPalette, PBYTE pbPalTiles,
									CNesTiles & vt, 
									std::vector<NES_BLOCK> & vmn,
									std::vector<NES_BLOCK> & vmj,
									PBYTE pbBgTiles
									)
{	
	CopyMemory(m_pal.bPalette, pbPalette, sizeof(m_pal.bPalette));
	CopyMemory(m_pal.bPalTiles, pbPalTiles, sizeof(m_pal.bPalTiles));

	m_block.vNTiles = vt;
	m_block.vMinor = vmn;

	CreateTilesImages();
}

VOID CTilesEditDlg::Get_Blocks(PBYTE pbPalette, PBYTE pbPalTiles,
									CNesTiles & vt, 
									std::vector<NES_BLOCK> & vmn,
									std::vector<NES_BLOCK> & vmj,
									PBYTE pbBgTiles
									)
{
	CopyMemory(pbPalette, m_pal.bPalette, sizeof(m_pal.bPalette));
	CopyMemory(pbPalTiles, m_pal.bPalTiles, sizeof(m_pal.bPalTiles));

	vmn = m_block.vMinor;
}

// -- tiles banks
CTilesBanksDlg::CTilesBanksDlg(HINSTANCE hInstance)
	: CDialog(hInstance), m_fCreateOverwriteBank( TRUE ), m_bBank( 0xFF )
{

}

VOID CTilesBanksDlg::OnPreInit(CDialogTemplate & d)
{
	d.SetDlgTitle(TEXT("Tiles bank"));
	d.SetDlgStyle(DS_SHELLFONT | WS_BORDER | WS_CAPTION, 0);
	d.SetDlgPos(0, 0, 220, 110);

	d.AddPreItem(-1, cButton, NULL, 7, 7, -7, -(7 + 15 + 7), BS_GROUPBOX, TRUE, TRUE, FALSE);

	d.AddPreItem(0x1020, cButton, TEXT("Place to new bank"), 15, 15, 100, 13, BS_AUTORADIOBUTTON);
	d.AddPreItem(0x1021, cButton, TEXT("Merge with levels"), 15, 40, 100, 13, BS_AUTORADIOBUTTON);
	d.AddPreItem(0x1022, cCombobox, NULL, 120, 40, -15, 200, CBS_DROPDOWNLIST, TRUE, FALSE);

	d.AddPreItem(IDOK, cButton, MB_GetString(IDOK - 1), -(7 + 50 + 7 + 50), -(7 + 15), 50, 15, BS_DEFPUSHBUTTON);
	d.AddPreItem(IDCANCEL, cButton, MB_GetString(IDCANCEL - 1), -(7 + 50), -(7 + 15), 50, 15);
}

VOID CTilesBanksDlg::OnInit(LPARAM lParam)
{
	CheckAll(TRUE, 0x1020);

	CComboBox cb;
	cb.AssignWindow(Ctl(0x1022));

	for(std::map<INT, std::vector<BYTE>>::iterator v = m_nBank.begin(); v != m_nBank.end(); v++)
	{
		CString sLvl;

		size_t count = v->second.size();
		for(size_t n = 0; n < count; ++n)
		{
			CString s;
			s.Format(TEXT("%d%s"), v->second[n] + 1, ( n == count - 1 ? TEXT("") : TEXT(", ") ) );
			sLvl += s;
		}

		cb.AddString(sLvl, v->first);
	}

	cb.SetCurSel(0);
}

VOID CTilesBanksDlg::OnButton(USHORT uId)
{
	EnableAll( uId == 0x1021, 0x1022, 0 );
}

VOID CTilesBanksDlg::OnOK()
{
	if ( Checked( 0x1020 ) )
	{
		m_bBank = 0xFF;
		m_fCreateOverwriteBank = TRUE;
	}
	else
	{
		CComboBox cb;
		cb.AssignWindow(Ctl(0x1022));

		INT id = cb.GetParam(cb.GetCurSel());
		if ( m_nBank.find(id) != m_nBank.end() )
		{
			m_bBank = LOBYTE(id);
			m_fCreateOverwriteBank = FALSE;

			if ( !Confirm(TEXT("WARNING! Current tileset will be overwritten by values from specified tileset!\nDo you want to continue?")) )
				return;
		}
		else
		{
			ShowError(TEXT("Select valid bank"));
			return;
		}
	}

	Close(1);
}

VOID CTilesBanksDlg::Let_Info(PBYTE pbTilesBanks)
{
	for(BYTE i = 0; i < NES_COUNT_LEVELS; ++i)
		m_nBank[ pbTilesBanks[i] ].push_back(i);
}

VOID CTilesBanksDlg::Get_Info(BYTE & nBank, BOOL & fCreateOverwriteBank)
{
	nBank = m_bBank;
	fCreateOverwriteBank = m_fCreateOverwriteBank;
}