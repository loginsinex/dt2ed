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

VOID CMainWnd::DrawCurrentLevel(BYTE bRoomOnly, BOOL fDrawBridgeUnused)
{
	RECT rc = { 0, 0, 2048, 1536 };
	BYTE wSlots = ( m_Canvas.GetActiveCursor() == 5 ? m_wCatchedSlot + 1 : 0 );

	BOOL fDrawBridge = ( m_Canvas.GetActiveCursor() == 6 );
	BOOL fDrawMu = ( m_Canvas.GetActiveCursor() == 7 );

	if ( 0xFF == bRoomOnly )
	{
		FillRect(m_Canvas, &rc, m_Canvas);

		m_wCatchedItem = -1;
		for(int y = 5; y >= 0; --y)
		{
			for(int x = 7; x >= 0; --x)
			{			
				m_nEditor.DrawRoom(m_Canvas, 256 * x, 256 * y, x, y);
				if ( fDrawMu )
					m_nEditor.DrawMuSecret(m_Canvas, 256 * x, 256 * y, x, y);
			}
		}

		for(int y = 5; y >= 0; --y)
		{
			for(int x = 7; x >= 0; --x)
			{			
				m_nEditor.DrawRoomObjects(m_Canvas, 256 * x, 256 * y, x, y, wSlots);
				if ( fDrawBridge )
					m_nEditor.DrawBridge(m_Canvas, 256 * x, 256 * y, x, y);
			}
		}
	}
	else if ( bRoomOnly < 48 )
	{
		int x = bRoomOnly % 8, y = bRoomOnly / 8;

		for(int ox = -1; ox <= 1; ++ox)
		{
			for(int oy = -1; oy <= 1; ++oy)
			{
				m_nEditor.DrawRoom(m_Canvas, 256 * (x + ox), 256 * (y + oy), x + ox, y + oy);

				if ( fDrawMu )
					m_nEditor.DrawMuSecret(m_Canvas, 256 * x, 256 * y, x, y);

				m_nEditor.DrawRoomObjects(m_Canvas, 256 * (x + ox), 256 * (y + oy), x + ox, y + oy, wSlots);
				if ( fDrawBridge )
					m_nEditor.DrawBridge(m_Canvas, 256 * x, 256 * y, x, y);
			}
		}
	}

	m_Canvas.Update();
}

INT_PTR CMainWnd::ReqTilePos(PCVNREQPOS p)
{
	BYTE room, block;

	if ( p->pt.x < 0 || p->pt.y < 0 || !m_nEditor.GetBlockFromPoint(p->pt.x, p->pt.y, room, block ) )
		return TRUE;
	else
		return FALSE;
}

INT_PTR CMainWnd::ReqRoomPos(PCVNREQPOS p)
{
	if ( p->pt.x < 0 || p->pt.y < 0 || p->pt.x >= 8 || p->pt.y >= 6 )
		return 1;

	return 0;
}

INT_PTR CMainWnd::ReqObjtPos(PCVNREQPOS p)
{
	if ( p->pt.x < 0 || p->pt.x >= (( 32 * 8 * 8 ) / 8) ||
		p->pt.y < 0 || p->pt.y >= ( 32 * 8 * 6 ) )
		return 1;

	BYTE nRoomY = LOBYTE(p->pt.y / ( 32 * 8 )), nRoomX = LOBYTE( p->pt.x / 32 );
	POINT pt = { p->pt.x - INT(nRoomX) * 32, p->pt.y - INT(nRoomY) * 256 };
	POINT opt = { 0 };

	WORD wObj = m_nEditor.GetObjectFromPos(nRoomX, nRoomY, LOBYTE(pt.x), LOBYTE(pt.y), p->pt, p->fKeybdEvent);
	if ( 0xFFFE == wObj )
		return 1;

	if ( 0xFFFF != m_wCatchedItem )
		return 0;


	if ( 0xFFFF == wObj )
		m_Canvas.SetCursorColor(2, 0x00FF00);
	else
		m_Canvas.SetCursorColor(2, 0xFF00FF);

	return 0;
}

INT_PTR CMainWnd::ReqScroogePos(PCVNREQPOS p)
{
	POINT pt = p->pt;
	pt.x *= 128;

	if ( pt.x < 0 || pt.x >= 8 * ( 32 * 8 ) ||
		pt.y < 0 || pt.y >= 6 * ( 32 * 8 ) )
		return 1;

	BYTE nRoomY = LOBYTE(pt.y / ( 32 * 8 )), nRoomX = LOBYTE( pt.x / ( 32 * 8 ) );
	BYTE nPosY = LOBYTE( pt.y % ( 32 * 8 )), nPosX = LOBYTE( pt.x % ( 32 * 8 ) );
	BYTE room, block;

	if ( !m_nEditor.GetBlockFromPoint(INT(nRoomX) * 8, INT(nRoomY) * 8, room, block ) )
		return 1;

	BYTE id = 0;
	switch(p->id)
	{
	case 3: id = 0; break;
	case 4: id = 1; break;
	case 8: id = 2; break;
	}

	NES_POINT ptCurrent = { 0 }, ptNew = { nPosX, nRoomX, nPosY, nRoomY };
	if ( m_nEditor.Get_ScroogePos(ptCurrent, id) 
		&& m_nEditor.Let_ScroogePos(ptNew, id) )
	{
		DrawCurrentLevel( LOBYTE( INT(ptCurrent.bScrX) + 8 * INT(ptCurrent.bScrY) ) );
		DrawCurrentLevel( LOBYTE( INT(ptNew.bScrX) + 8 * INT(ptNew.bScrY) ) );
	}

	return 0;
}

INT_PTR CMainWnd::ReqSlotPos(PCVNREQPOS p)
{
	if ( p->pt.x < 0 || p->pt.y < 0 || p->pt.x >= (8 * 256) || p->pt.y >= (6 * 256) )
		return 1;

	BYTE bRoom, bBlock;
	if ( !m_nEditor.GetBlockFromPoint(p->pt.x / 32, p->pt.y / 32, bRoom, bBlock) )
		return 1;

	if ( m_nEditor.GetSlotFromPos(m_wCatchedSlot, p->pt.x, p->pt.y) != -1 )
		m_Canvas.SetCursorColor(5, RGB(255, 255, 0));
	else
		m_Canvas.SetCursorColor(5, 0xA0F230);

	return 0;
}

VOID CMainWnd::ReqSlotModify(PCVNREQMODIFY p)
{
	NES_POINT spt = { 0 };
	spt.bPosX = LOBYTE(p->pt.x % 256); spt.bPosY = LOBYTE(p->pt.y % 256);
	spt.bScrX = LOBYTE(p->pt.x / 256); spt.bScrY = LOBYTE(p->pt.y / 256);

	switch(p->key)
	{
	case VK_INSERT:
		{
			if ( m_nEditor.InsertNewSlot(m_wCatchedSlot, spt) )
				DrawCurrentLevel();
			break;
		}
	case VK_DELETE:
		{
			INT idSlot = m_nEditor.GetSlotFromPos(m_wCatchedSlot, p->pt.x, p->pt.y);
			if ( idSlot != -1 )
			{
				if ( m_nEditor.DeleteSlot(m_wCatchedSlot, idSlot) )
					DrawCurrentLevel();
			}
			break;
		}
	}
}


VOID CMainWnd::ReqTileModify(PCVNREQMODIFY p)
{
	BYTE room, block;
	if ( m_nEditor.GetBlockFromPoint(p->pt.x, p->pt.y, room, block) )
	{
		room++;
						
		switch(p->key)
		{
		case VK_INSERT:
			{
				std::vector<NES_BLOCK> vmn, vmj;
				std::vector<NES_TILE> vt;
				BYTE bPal[16] = { 0 }, bPalTiles[256] = { 0 }, bBgTiles[0x0C] = { 0 };
				m_nEditor.Get_LevelBlocks(bPal, bPalTiles, vt, vmn, vmj);
				m_nEditor.Get_BgTiles(bBgTiles);

				CBlockEditorDlg dlg(GetInstance(), m_File.IsExtended(), TRUE);
				dlg.Let_Blocks(bPal, bPalTiles, vt, vmn, vmj, bBgTiles);
				dlg.Let_Block(block);

				if ( dlg.Show(this) )
				{
					dlg.Get_Block(block);

					if ( m_nEditor.SetBlockFromPoint(p->pt.x % 8, p->pt.y % 8, room, block ) )
					{							
						std::vector<POINT> rc;

						if ( m_nEditor.GetRoomPoint(room, rc) )
						{
							for(size_t i = 0; i < rc.size(); ++i)
								DrawCurrentLevel( LOBYTE(rc[i].x + 8 * rc[i].y) );
						}
					}

				}

				break;
			}
		case -1:
			{
				BOOL fCtrl = HIWORD(GetKeyState(VK_CONTROL));
				if ( !p->delta && fCtrl )
				{
					m_wCatchedItem = block;
				}
				else if ( !p->delta && m_wCatchedItem != 0xFFFF )
				{
					block = LOBYTE(m_wCatchedItem);
				}
				else
					block = LOBYTE( INT(block) + p->delta );

				if ( m_nEditor.SetBlockFromPoint(p->pt.x % 8, p->pt.y % 8, room, block ) )
				{							
					std::vector<POINT> rc;

					if ( m_nEditor.GetRoomPoint(room, rc) )
					{
//						m_nEditor.GetBlockFromPoint(p->pt.x, p->pt.y, room, block);

						for(size_t i = 0; i < rc.size(); ++i)
							DrawCurrentLevel( LOBYTE(rc[i].x + 8 * rc[i].y) );
					}
				}
				break;
			}
		}
	}
}

VOID CMainWnd::ReqRoomModify(PCVNREQMODIFY p)
{
	switch(p->key)
	{
	case VK_INSERT: 
		{
			if ( !m_nEditor.InsertRoom(LOBYTE(p->pt.x), LOBYTE(p->pt.y)) )
			{
				ShowError(TEXT("Unable to insert new room. Possible insufficient space"));
			}
			else
			{
				DrawCurrentLevel(LOBYTE(p->pt.x + 8 * p->pt.y));
			}
			break;
		}
	case VK_DELETE:
		{
			if ( !m_nEditor.DeleteRoom(LOBYTE(p->pt.x), LOBYTE(p->pt.y)) )
			{
				ShowError(TEXT("Unable to delete this room"));
			}
			else
			{
				DrawCurrentLevel();
			}
			break;
		}
	case -1:
		{
			if ( !p->delta )
			{
				if ( HIWORD(GetKeyState(VK_CONTROL)) )
				{
					BYTE room = 0, block = 0;
					if ( m_nEditor.GetBlockFromPoint(8 * p->pt.x, 8 * p->pt.y, room, block) )
					{
						m_wCatchedItem = room;
					}
					else
						m_wCatchedItem = 0xFFFF;
				}
				else if ( m_wCatchedItem != 0xFFFF )
				{
					BYTE room = 0, block = 0;
					if ( m_nEditor.GetBlockFromPoint(8 * p->pt.x, 8 * p->pt.y, room, block) )
					{
						NES_ROOM nRoom = { 0 };
						if ( m_nEditor.Get_RoomData(LOBYTE(m_wCatchedItem), nRoom) )
							if ( m_nEditor.Let_RoomData(room, nRoom) )
							{
								DrawCurrentLevel();
							}
					}
				}
			}
			else
			{
				BYTE room = 0, block = 0;
				if ( m_nEditor.GetBlockFromPoint(8 * p->pt.x, 8 * p->pt.y, room, block) )
				{
					room = LOBYTE(INT(room) + p->delta + 1);
					m_nEditor.SetRoom(LOBYTE(p->pt.x), LOBYTE(p->pt.y), room);
				}
				else
					m_nEditor.SetRoom(LOBYTE(p->pt.x), LOBYTE(p->pt.y), 1);
				
				DrawCurrentLevel();
			}			
			break;
		}
	}
}

VOID CMainWnd::ReqObjtModify(PCVNREQMODIFY p)
{
	BYTE nRoomX = LOBYTE(p->pt.x / 32), nRoomY = LOBYTE(p->pt.y / 256);
	BYTE bObjX = LOBYTE(p->pt.x % 32), bObjY = LOBYTE(p->pt.y % 256);

	switch(p->key)
	{
	case VK_INSERT:
		{
			BYTE bPal[16] = { 0 }, b1 = 0, b2 = 0, bBossObject = 0;
			m_nEditor.Get_LevelData(bPal, b1, b2, bBossObject);
			m_nEditor.Get_PaletteEx(bPal);
			
			CObjectInsert oi(GetInstance(), m_Tiles, m_nGraphics, bPal, bBossObject);

			if ( !oi.Show(this) )
				break;

			BYTE bobj = 0;
			oi.Get_Object(bobj);

			if ( bobj >= 0x9d )
			{
				bobj -= m_nCurrentLevel * 8;
				if ( bobj < 0x9d )
					bobj += 8 * NES_COUNT_LEVELS;
			}

			if ( m_File.IsExtended() && m_nEditor.InsertObject(nRoomX, nRoomY, bObjX, bObjY, bobj) )
			{
				DrawCurrentLevel(nRoomX + 8 * nRoomY);
				m_Canvas.SetCursorColor(2, 0xFF00FF);
			}
			else
			{
				ShowError(TEXT("Unable to insert new object. Possible insufficient space"));
			}
			break;
		}
	case VK_DELETE:
		{
			if ( !m_File.IsExtended() || !m_nEditor.DeleteObject(nRoomX, nRoomY, bObjX, bObjY) )
			{
				ShowError(TEXT("Unable to delete object. Object not selected?"));
			}
			else
			{
				DrawCurrentLevel(nRoomX + 8 * nRoomY);
				m_Canvas.SetCursorColor(2, 0x00FF00);
			}
			break;
		}
	case -1:
		{
			POINT curpt = { 0 };
			if ( p->delta )
			{				
				INT id = m_nEditor.GetObjectFromPos(nRoomX, nRoomY, bObjX, bObjY, curpt, TRUE);
				if ( 0xFFFF != id )
				{
					id += p->delta;

					if ( id < 0 ) id += 256;
					if ( id >= 256 ) id -= 256;

					if ( m_nEditor.SetObject(nRoomX, nRoomY, bObjX, bObjY, LOBYTE(id)) )
					{
						DrawCurrentLevel(nRoomX + 8 * nRoomY);
						m_Canvas.SetCursorColor(2, 0xFF00FF);
					}
				}
			}
			else if ( !p->delta && HIWORD(GetKeyState(VK_CONTROL))  && m_File.IsExtended() )
			{
				m_wCatchedItem = m_nEditor.GetObjectFromPos(nRoomX, nRoomY, bObjX, bObjY, curpt, TRUE);
				if ( 0xFFFF != m_wCatchedItem )
				{					
					CVNREQMODIFY c = *p;
					c.key = VK_DELETE;
					ReqObjtModify(&c);
					m_Canvas.SetCursorColor(2, 0xFF0000);
				}
			}
			else if ( !p->delta && 0xFFFF != m_wCatchedItem  && m_File.IsExtended() )
			{
				if ( !m_nEditor.InsertObject(nRoomX, nRoomY, bObjX, bObjY, LOBYTE(m_wCatchedItem)) )
				{
					ShowError(TEXT("Unable to insert new object. Possible insufficient space"));
				}
				else
				{
					m_wCatchedItem = 0xFFFF;
					DrawCurrentLevel(nRoomX + 8 * nRoomY);
					m_Canvas.SetCursorColor(2, 0xFF00FF);
				}
			}
			break;
		}
	}
}

INT_PTR CMainWnd::ReqBrdgPos(PCVNREQPOS p)
{
	BYTE bRoom, bBlock;
	if ( !m_nEditor.GetBlockFromPoint(p->pt.x / 2, p->pt.y / 2, bRoom, bBlock) 
		|| !m_File.IsExtended() && p->pt.y >= 16 )
		return 1;

	NES_BRIDGE nb = { 0 };
	if ( !m_nEditor.Get_LevelBridge(nb) )
		return 1;

	if ( p->pt.x == MAKEWORD(nb.ptLeft.bPosX, nb.ptLeft.bScrX) / 16 &&
		p->pt.y == MAKEWORD(nb.ptLeft.bPosY, nb.ptLeft.bScrY) / 16 )
		m_Canvas.SetCursorColor(6, XCHGCOLORREF(0xF2F230));
	else
		m_Canvas.SetCursorColor(6, 0xF2F230);

	return 0;
}

VOID CMainWnd::ReqBrdgModify(PCVNREQMODIFY p)
{
	if ( p->key != -1 )
		return;

	BYTE bRoom, bBlock;
	if ( !m_nEditor.GetBlockFromPoint(p->pt.x / 2, p->pt.y / 2, bRoom, bBlock) 
		|| !m_File.IsExtended() && p->pt.y >= 16 )
		return;

	NES_BRIDGE nb = { 0 };
	if ( !m_nEditor.Get_LevelBridge(nb) )
		return;

	BOOL fUpdate = TRUE;
	if ( p->pt.x == MAKEWORD(nb.ptLeft.bPosX, nb.ptLeft.bScrX) / 16 &&
		p->pt.y == MAKEWORD(nb.ptLeft.bPosY, nb.ptLeft.bScrY) / 16 && p->delta )
	{
		int len = (MAKEWORD(nb.ptRight.bPosX, nb.ptRight.bScrX) 
			- MAKEWORD(nb.ptLeft.bPosX, nb.ptLeft.bScrX) + 1) / 16;

		len += p->delta;
		if ( len < 1 )
			len = 1;
		else if ( len >= 8 * 16 )
			len = 8 * 16 - 1;

		len = LOWORD(len*16) - 1;
		nb.ptRight.bPosX = LOBYTE( MAKEWORD(nb.ptLeft.bPosX, nb.ptLeft.bScrX) + len );
		nb.ptRight.bScrX = HIBYTE( MAKEWORD(nb.ptLeft.bPosX, nb.ptLeft.bScrX) + len );

	}
	else if ( !p->delta )
	{
		BYTE nRoomX = LOBYTE(p->pt.x / 16), nRoomY = LOBYTE(p->pt.y / 16);
		BYTE nPosX = LOBYTE(p->pt.x % 16), nPosY = LOBYTE(p->pt.y % 16);

		int len = MAKEWORD(nb.ptRight.bPosX, nb.ptRight.bScrX) 
			- MAKEWORD(nb.ptLeft.bPosX, nb.ptLeft.bScrX);

		nb.ptLeft.bPosX = 16 * nPosX; nb.ptLeft.bPosY = 16 * nPosY;
		nb.ptLeft.bScrX = nRoomX; nb.ptLeft.bScrY = nRoomY;
		nb.ptRight.bPosX = LOBYTE( MAKEWORD(nb.ptLeft.bPosX, nb.ptLeft.bScrX) + len );
		nb.ptRight.bScrX = HIBYTE( MAKEWORD(nb.ptLeft.bPosX, nb.ptLeft.bScrX) + len );
	}
	else
		fUpdate = FALSE;

	if ( fUpdate && m_nEditor.Let_LevelBridge(nb) )
		DrawCurrentLevel(0xFF, TRUE);
}

INT_PTR CMainWnd::ReqMscrPos(PCVNREQPOS p)
{
	BYTE nRoomX = LOBYTE(p->pt.x / 256), nRoomY = LOBYTE(p->pt.y / 256);
	BYTE bRoom, bBlock;
	if ( !m_nEditor.GetBlockFromPoint(nRoomX * 8, nRoomY * 8, bRoom, bBlock) 
		|| !m_nEditor.IsValidMuSecretRoom(nRoomX, nRoomY))
		return 1;

	NES_POINT npt = { 0 };

	if ( -1 != m_nEditor.GetMuObjectId(nRoomX, nRoomY, LOBYTE(p->pt.x % 256), LOBYTE(p->pt.y % 256), npt) )
		m_Canvas.SetCursorColor(7, XCHGCOLORREF(0x00ffff));
	else
		m_Canvas.SetCursorColor(7, 0x529133);

	return 0;
}

VOID CMainWnd::ReqMscrModify(PCVNREQMODIFY p)
{
	BYTE nRoomX = LOBYTE(p->pt.x / 256), nRoomY = LOBYTE(p->pt.y / 256);
	
	if ( !m_nEditor.IsValidMuSecretRoom(nRoomX, nRoomY) )
		return;

	NES_POINT npt = { 0 };

	int id = m_nEditor.GetMuObjectId(nRoomX, nRoomY, LOBYTE(p->pt.x % 256), LOBYTE(p->pt.y % 256), npt);

	BOOL fUpdate = FALSE;
	switch(p->key)
	{
	case VK_INSERT: fUpdate = m_nEditor.InsertMuObject(nRoomX, nRoomY, LOBYTE(p->pt.x % 256), LOBYTE(p->pt.y % 256)); break;
	case VK_DELETE: fUpdate = m_nEditor.DeleteMuObject(id); break;
	}

	if ( fUpdate )
	{
		m_Canvas.SetCursorPosition(p->pt.x, p->pt.y);
		DrawCurrentLevel(nRoomX + 8 * nRoomY);
	}
}
