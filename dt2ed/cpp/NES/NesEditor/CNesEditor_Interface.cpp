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

BOOL CNesEditor::Get_Costs(PBYTE pbCosts)
{
	if ( !m_fLoaded )
		return FALSE;

	CopyMemory(pbCosts, m_nGame.nMoney, sizeof(m_nGame.nMoney));
	return TRUE;
}

BOOL CNesEditor::Let_Costs(PBYTE pbCosts)
{
	if ( !m_fLoaded )
		return FALSE;

	CopyMemory(m_nGame.nMoney, pbCosts, sizeof(m_nGame.nMoney));
	return TRUE;
}

BOOL CNesEditor::Get_Drops(PWORD pwDrops)
{
	if ( !m_fLoaded )
		return FALSE;

	CopyMemory(pwDrops, m_nGame.wDrops, sizeof(m_nGame.wDrops));
	return TRUE;
}

BOOL CNesEditor::Let_Drops(PWORD pwDrops)
{
	if ( !m_fLoaded )
		return FALSE;

	CopyMemory(m_nGame.wDrops, pwDrops, sizeof(m_nGame.wDrops));
	return TRUE;
}

BOOL CNesEditor::Get_PaletteEx(PBYTE pbPalette)
{
	if ( -1 == m_nCurrentLevel )
		return FALSE;

	CopyMemory(pbPalette, &m_nBank.nPalette[ m_nLvlBanks[m_nCurrentLevel].nPalette ][16], 16);
	return TRUE;
}

BOOL CNesEditor::Get_EgyptSecret(PBYTE pbCode)
{
	if ( !m_fLoaded )
		return FALSE;

	CopyMemory(pbCode, m_nGame.bEgyptCode, sizeof(m_nGame.bEgyptCode));
	return TRUE;
}

BOOL CNesEditor::Let_EgyptSecret(PBYTE pbCode)
{
	if ( !m_fLoaded )
		return FALSE;

	CopyMemory(m_nGame.bEgyptCode, pbCode, sizeof(m_nGame.bEgyptCode));
	return TRUE;
}

BOOL CNesEditor::Get_ScroogePos(NES_POINT & pt, BYTE id)
{
	if ( !m_fLoaded || -1 == m_nCurrentLevel )
		return FALSE;

	NES_LEVEL_HEADER & lh = m_nBank.nHeader.nHeader[m_nLvlBanks[m_nCurrentLevel].nHeaderPtr];

	switch(id)
	{
	case 0: pt = lh.start; break;
	case 1: pt = lh.restart; break;
	case 2: pt = lh.bossrestart; break;
	}

	return TRUE;
}

BOOL CNesEditor::Let_ScroogePos(NES_POINT & pt, BYTE id)
{
	if ( !m_fLoaded || -1 == m_nCurrentLevel )
		return FALSE;

	NES_LEVEL_HEADER & lh = m_nBank.nHeader.nHeader[m_nLvlBanks[m_nCurrentLevel].nHeaderPtr];

	switch(id)
	{
	case 0: lh.start = pt; break;
	case 1: lh.restart = pt; break;
	case 2: lh.bossrestart = pt; break;
	}

	return TRUE;
}

BOOL CNesEditor::Let_BgTiles(PBYTE pbBgTiles)
{
	if ( !m_fLoaded )
		return FALSE;

	CopyMemory(m_nGame.bBgTiles, pbBgTiles, sizeof(m_nGame.bBgTiles));
	return TRUE;
}

BOOL CNesEditor::Get_BgTiles(PBYTE pbBgTiles)
{
	if ( !m_fLoaded )
		return FALSE;

	CopyMemory(pbBgTiles, m_nGame.bBgTiles, sizeof(m_nGame.bBgTiles));
	return TRUE;
}
