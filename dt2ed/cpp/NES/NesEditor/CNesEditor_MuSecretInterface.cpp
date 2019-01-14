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

BOOL CNesEditor::IsValidMuSecretRoom(BYTE nRoomX, BYTE nRoomY)
{
	if ( nRoomX < 0 || nRoomY < 0 || nRoomX >= 8 || nRoomY >= 6 )
		return FALSE;

	BYTE bNumOfRoom = 0xFF;
	if ( m_nGame.nMuSecret.vobjX.size() )
		bNumOfRoom = m_nGame.nMuSecret.vobjX[0].bScr;

	if ( bNumOfRoom != 0xFF && 
		(( !m_nGame.nMuSecret.fVertical && bNumOfRoom != nRoomX ) || ( m_nGame.nMuSecret.fVertical && bNumOfRoom != nRoomY ))
		)
		return FALSE;

	size_t count = m_nLevel.nObjects[nRoomY].size();

	// find 0x22 object
	BOOL fFound22 = FALSE;
	for(size_t n = 0; n < count; ++n)
	{
		NES_OBJECT & no = m_nLevel.nObjects[nRoomY][n];
		if ( no.screen == nRoomX && no.id == 0x22 )
		{
			fFound22 = TRUE;
			break;
		}
	}

	if ( !fFound22 )
		return FALSE;

	return TRUE;
}

INT CNesEditor::GetMuObjectId(BYTE nRoomX, BYTE nRoomY, BYTE nScrX, BYTE nScrY, NES_POINT & npt)
{
	if ( -1 == m_nCurrentLevel || !m_fLoaded )
		return -1;

	if ( !IsValidMuSecretRoom(nRoomX, nRoomY) )
		return -1;

	POINT pt = { nScrX, nScrY };
	size_t count = m_nGame.nMuSecret.vobjX.size();
	RECT rcObj = m_rcObj[m_nGame.nMuSecret.bObjId];
	SIZE szObj = { rcObj.right - rcObj.left, rcObj.bottom - rcObj.top };

	const int xdelta = rcObj.left - 64, ydelta = -szObj.cy + 1;

	for(size_t n = 0; n < count; ++n)
	{
		NES_DOT & nd = m_nGame.nMuSecret.vobjX[n];

		if ( !m_nGame.nMuSecret.fVertical )
		{
			RECT rc = { nd.bPos + xdelta, m_nGame.nMuSecret.bPosY + ydelta,
				nd.bPos + xdelta + szObj.cx, m_nGame.nMuSecret.bPosY + ydelta + szObj.cy };

			if ( PtInRect(&rc, pt) )
			{
				npt.bScrX = nRoomX; npt.bScrY = nRoomY;
				npt.bPosX = nd.bPos; npt.bPosY = m_nGame.nMuSecret.bPosY;
				return n;
			}
		}
		else
		{
			RECT rc = { m_nGame.nMuSecret.bPosY, nd.bPos - szObj.cy,
				m_nGame.nMuSecret.bPosY, nd.bPos };

			if ( PtInRect(&rc, pt) )
			{
				npt.bScrX = nRoomX; npt.bScrY = nRoomY;
				npt.bPosY = nd.bPos; npt.bPosX = m_nGame.nMuSecret.bPosY;
				return n;
			}
		}
	}

	return -1;
}

BOOL CNesEditor::DeleteMuObject(int id)
{
	if ( -1 == m_nCurrentLevel || !m_fLoaded )
		return FALSE;

	NES_MU_SECRET & nmu = m_nGame.nMuSecret;
	std::vector<NES_DOT> & o = nmu.vobjX;

	if ( id >= 0 && id < int(o.size()) )
	{
		o.erase(o.begin() + id);
		return TRUE;
	}

	return FALSE;
}

BOOL CNesEditor::InsertMuObject(BYTE nRoomX, BYTE nRoomY, BYTE nPosX, BYTE nPosY)
{
	if ( -1 == m_nCurrentLevel || !m_fLoaded )
		return FALSE;

	if ( !IsValidMuSecretRoom(nRoomX, nRoomY) ||
		( m_nGame.fExtended && m_nGame.nMuSecret.vobjX.size() >= 8 ) ||
		( !m_nGame.fExtended && m_nGame.nMuSecret.vobjX.size() >= 2 )
		)
		return FALSE;

	RECT rc = m_rcObj[m_nGame.nMuSecret.bObjId];
	SIZE sz = { rc.right - rc.left, rc.bottom - rc.top };

	const int xdelta = rc.left - 64, ydelta = -sz.cy + 1;


	if ( !m_nGame.nMuSecret.fVertical )
	{
		NES_DOT nd = { 0 };
		nd.bPos = LOBYTE(int(nPosX) - xdelta); nd.bScr = nRoomX;
		m_nGame.nMuSecret.vobjX.push_back(nd);

		m_nGame.nMuSecret.bPosY = LOBYTE(int(nPosY) - ydelta);
	}
	else
	{
		NES_DOT nd = { 0 };
		nd.bPos = LOBYTE(int(nPosY) - ydelta); nd.bScr = nRoomY;
		m_nGame.nMuSecret.vobjX.push_back(nd);

		m_nGame.nMuSecret.bPosY = LOBYTE(int(nPosX) - xdelta);
	}

	return TRUE;
}

BOOL CNesEditor::SetMuObject(BYTE nRoomX, BYTE nRoomY, BYTE nPosX, BYTE nPosY, int id)
{
	if ( -1 == m_nCurrentLevel || !m_fLoaded )
		return FALSE;

	if ( id >= 0 && id < int(m_nGame.nMuSecret.vobjX.size()) )
	{
		RECT rc = m_rcObj[m_nGame.nMuSecret.bObjId];
		SIZE sz = { rc.right - rc.left, rc.bottom - rc.top };

		const int xdelta = rc.left - 64, ydelta = -sz.cy + 1;

		if ( !m_nGame.nMuSecret.fVertical )
		{
			m_nGame.nMuSecret.vobjX[id].bPos = LOBYTE(int(nPosX) - xdelta);
			m_nGame.nMuSecret.vobjX[id].bScr = nRoomX;
			m_nGame.nMuSecret.bPosY = LOBYTE(int(nPosY) - ydelta);
		}
		else
		{
			m_nGame.nMuSecret.vobjX[id].bPos = LOBYTE(int(nPosY) - ydelta);
			m_nGame.nMuSecret.vobjX[id].bScr = nRoomY;
			m_nGame.nMuSecret.bPosY = LOBYTE(int(nPosX) - xdelta);
		}
		return TRUE;
	}

	return FALSE;
}

BOOL CNesEditor::ValidateMuObjectPos(POINT & pt)
{
	RECT rc = m_rcObj[m_nGame.nMuSecret.bObjId];
	SIZE sz = { rc.right - rc.left, rc.bottom - rc.top};

	const int xdelta = rc.left - 64, ydelta = -sz.cy + 1;

	pt.x -= xdelta; pt.y -= ydelta;
	return TRUE;
}


BOOL CNesEditor::Let_MuSecret(NES_MU_SECRET & nmu)
{
	if ( !m_fLoaded || !nmu.bObjId )
		return FALSE;

	m_nGame.nMuSecret = nmu;
	return TRUE;
}

BOOL CNesEditor::Get_MuSecret(NES_MU_SECRET & nmu)
{
	if ( !m_fLoaded || !m_nGame.nMuSecret.bObjId )
		return FALSE;

	nmu = m_nGame.nMuSecret;
	return TRUE;
}
