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

BOOL CNesEditor::InsertNewSlot(INT idSet, NES_POINT & npt)
{
	if ( idSet < 0 || idSet >= 7 || !m_fLoaded || !m_nGame.fExtended )
		return FALSE;

	size_t count = m_nGame.nSteps[m_nCurrentLevel][idSet].size();

	for(size_t n = 0; n < count; ++n)
	{
		NES_POINT & pt = m_nGame.nSteps[m_nCurrentLevel][idSet][n];

		if ( !pt.bPosX && !pt.bPosY && !pt.bScrX && !pt.bScrY )
		{
			pt = npt;
			return TRUE;
		}
	}

	m_nGame.nSteps[m_nCurrentLevel][idSet].push_back(npt);

	return TRUE;
}

BOOL CNesEditor::DeleteSlot(INT idSet, INT idSlot)
{
	if ( idSet < 0 || idSet >= 7 || !m_fLoaded || idSlot < 0  || !m_nGame.fExtended )
		return FALSE;

	INT count = m_nGame.nSteps[m_nCurrentLevel][idSet].size();
	if ( idSlot >= count )
		return FALSE;

	NES_POINT & pt = m_nGame.nSteps[m_nCurrentLevel][idSet][idSlot];
	pt.bPosX = pt.bPosY = pt.bScrX = pt.bScrY = 0;

	return TRUE;
}

INT CNesEditor::GetSlotFromPos(INT idSet, INT x, INT y)
{
	if ( idSet < 0 || idSet >= 7 || !m_fLoaded )
		return FALSE;

	int lvl = ( m_nGame.fExtended ? m_nCurrentLevel : 0 );

	int count = m_nGame.nSteps[lvl][idSet].size();
	POINT spt = { x, y };

	for(int n = count - 1; n >= 0; --n)
	{
		NES_POINT & pt = m_nGame.nSteps[lvl][idSet][n];

		if ( !pt.bPosX && !pt.bPosY && !pt.bScrX && !pt.bScrY )
			continue;

		INT sx = INT(pt.bScrX) * 256 + INT(pt.bPosX),
			sy = INT(pt.bScrY) * 256 + INT(pt.bPosY);

		RECT rc = { sx - 8, sy - 16, sx + 8, sy + 8 };

		if ( PtInRect(&rc, spt) )
			return n;
	}

	return -1;
}

BOOL CNesEditor::IsValidSlot(INT idSet, INT idSlot)
{
	if ( idSet < 0 || idSet >= 7 || !m_fLoaded )
		return FALSE;

	int lvl = ( m_nGame.fExtended ? m_nCurrentLevel : 0 );

	int count = m_nGame.nSteps[lvl][idSet].size();
	if ( idSlot >= count )
		return FALSE;

	NES_POINT & pt = m_nGame.nSteps[lvl][idSet][idSlot];
	if ( !pt.bPosX && !pt.bPosY && !pt.bScrX && !pt.bScrY )
		return FALSE;

	return TRUE;
}
