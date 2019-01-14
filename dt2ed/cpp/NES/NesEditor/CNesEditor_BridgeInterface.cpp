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


BOOL CNesEditor::Get_LevelBridge(NES_BRIDGE & nb)
{
	if ( m_nCurrentLevel == -1 || !m_fLoaded )
		return FALSE;

	if ( m_nCurrentLevel == 0 
		|| m_nBank.nObjHandlers[0] == m_nBank.nObjHandlers[m_nCurrentLevel] )
	{
		nb = m_nBank.nBridgeData;
		return TRUE;
	}

	return FALSE;
}

BOOL CNesEditor::Let_LevelBridge(NES_BRIDGE & nb)
{
	if ( m_nCurrentLevel == -1 || !m_fLoaded )
		return FALSE;

	if ( m_nCurrentLevel == 0 
		|| m_nBank.nObjHandlers[0] == m_nBank.nObjHandlers[m_nCurrentLevel] )
	{
		m_nBank.nBridgeData = nb;
		return TRUE;
	}

	return FALSE;
}
