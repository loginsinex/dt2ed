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

const TCHAR s_sDecodedChars[] =
{
	_T('\''), _T('/'), 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00,
	0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00,
	0xFF00, _T('"'), 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, _T(','), 0xFF00, _T('-'), 0xFF00, _T('!'), 0xFF00, _T('?'), 0xFF00, 0xFF00,
	0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00,

	0xFF00, _T('0'), 0xFF00, _T('1'), 0xFF00, _T('2'), 0xFF00, _T('3'), 0xFF00, _T('4'), 0xFF00, _T('5'), 0xFF00, _T('6'), 0xFF00, _T('7'),
	0xFF00, _T('8'), 0xFF00, _T('9'), 0xFF00, _T('$'), 0xFF00, 0xFF00, _T('R'), _T('A'), _T('W'), _T('B'), 0xFF00, _T('C'), _T('.'), _T('E'),
	_T(' '), _T('S'), _T('G'), _T('T'), _T('H'), _T('U'), _T('I'), _T('V'), _T('L'), 0xFF00, _T('M'), 0xFF00, _T('O'), 0xFF00, _T('P'), 0xFF00,
	0xFF00, 0xFF00, 0xFF00, 0xFF00, _T('>'), _T('D'), _T('F'), _T('J'), _T('K'), _T('N'), _T('Q'), _T('X'), _T('Y'), _T('Z'), _T('_'), 0
};

BOOL CNesEditor::Get_LevelName(INT n, CString & sLevelName)
{
	if ( !m_fLoaded || n < 0 || n >= NES_COUNT_LEVELS )
		return FALSE;

	TCHAR pszName[13] = { 0 };
	for(size_t c = 0; c < 12; ++c)
	{
		BYTE chr = m_nGame.sLevelName[n][c];
		if ( chr < sizeof(s_sDecodedChars) / sizeof(s_sDecodedChars[0]) )
			pszName[c] = s_sDecodedChars[chr];
		else
			pszName[c] = 0xffff;
	}

	sLevelName = pszName;

	return TRUE;
}

BOOL CNesEditor::Let_LevelName(INT n, CString & sLevelName)
{
	if ( !m_fLoaded || n < 0 || n >= NES_COUNT_LEVELS )
		return FALSE;

	if ( sLevelName.GetLength() > 12 )
		return FALSE;

	TCHAR pszName[13] = { 0 };
	wsprintf(pszName, TEXT("%s"), sLevelName.GetString());
	for(INT i = 0; i < 12; ++i)
		if ( !pszName[i] ) pszName[i] = _T(' ');

	for(size_t c = 0; c < 12; ++c)
	{
		TCHAR chr = pszName[c];
		for(size_t k = 0; k < sizeof(s_sDecodedChars) / sizeof(s_sDecodedChars[0]); ++k)
		{
			if ( s_sDecodedChars[k] != 0xffff && s_sDecodedChars[k] == chr )
			{
				m_nGame.sLevelName[n][c] = k;
				break;
			}
			else
				m_nGame.sLevelName[n][c] = 0x60;
		}
	}

	return TRUE;
}

BOOL CNesEditor::Let_ExtendedLevel(BYTE nBasedOnLevel)
{
	if ( !m_nGame.fExtended || 0x06 != m_nCurrentLevel || nBasedOnLevel == 0x06 || nBasedOnLevel >= NES_COUNT_LEVELS )
		return FALSE;

	// create new banks for each data

	m_nTiles[m_nCurrentLevel] = m_nTiles[nBasedOnLevel];
	
	m_nBank.nHeader.nHeader[m_nCurrentLevel].ptrBlocks = LOBYTE(m_nBank.nHeader.nHeader.size());
	m_nBank.nHeader.nHeader[m_nCurrentLevel].ptrTiles = m_nBank.nHeader.nHeader[nBasedOnLevel].ptrTiles;
	m_nBank.nHeader.nHeader[m_nCurrentLevel].ptrPPUCorrector = m_nBank.nHeader.nHeader[nBasedOnLevel].ptrPPUCorrector;
	m_nBank.nHeader.nHeader[m_nCurrentLevel].ptrPPUSpritesCorrector = m_nBank.nHeader.nHeader[nBasedOnLevel].ptrPPUSpritesCorrector;
	
	m_nBank.nPalLoaders[m_nCurrentLevel] = m_nBank.nPalLoaders[nBasedOnLevel];	
	m_nBank.nObjHandlers[m_nCurrentLevel] = m_nBank.nObjHandlers[nBasedOnLevel];

	m_nBank.nHeader.nHeader[m_nCurrentLevel].start = m_nBank.nHeader.nHeader[nBasedOnLevel].start;
	m_nBank.nHeader.nHeader[m_nCurrentLevel].restart = m_nBank.nHeader.nHeader[nBasedOnLevel].restart;
	m_nBank.nHeader.nHeader[m_nCurrentLevel].bossrestart = m_nBank.nHeader.nHeader[nBasedOnLevel].bossrestart;

	m_nLvlBanks[m_nCurrentLevel].nLayout = LOBYTE(m_nBank.nLayout.size());
	m_nLvlBanks[m_nCurrentLevel].nPalette = LOBYTE(m_nBank.nPalette.size());
	m_nLvlBanks[m_nCurrentLevel].nMajor.nMajorBlock[0] = LOBYTE(m_nBank.nMajor.nMajorBlocks[0].size());
	m_nLvlBanks[m_nCurrentLevel].nMajor.nMajorBlock[1] = LOBYTE(m_nBank.nMajor.nMajorBlocks[1].size());
	m_nLvlBanks[m_nCurrentLevel].nMajor.nMajorBlock[2] = LOBYTE(m_nBank.nMajor.nMajorBlocks[2].size());
	m_nLvlBanks[m_nCurrentLevel].nMajor.nMajorBlock[3] = LOBYTE(m_nBank.nMajor.nMajorBlocks[3].size());

	m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocks[0] = LOBYTE(m_nBank.nMinor.nMinorBlocks[0].size());
	m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocks[1] = LOBYTE(m_nBank.nMinor.nMinorBlocks[1].size());
	m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocks[2] = LOBYTE(m_nBank.nMinor.nMinorBlocks[2].size());
	m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocks[3] = LOBYTE(m_nBank.nMinor.nMinorBlocks[3].size());

	m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocksPalettes = LOBYTE(m_nBank.nMinor.nMinorBlocksPalettes.size());

	m_nLvlBanks[m_nCurrentLevel].nBlocksPtr = LOBYTE(m_nBank.nHeader.nBlocks.size());
	
	m_nBank.nLayout.push_back(m_nBank.nLayout[m_nLvlBanks[nBasedOnLevel].nLayout]);
	m_nBank.nPalette.push_back(m_nBank.nPalette[m_nLvlBanks[nBasedOnLevel].nPalette]);
	m_nBank.nMajor.nMajorBlocks[0].push_back(m_nBank.nMajor.nMajorBlocks[0][m_nLvlBanks[nBasedOnLevel].nMajor.nMajorBlock[0]]);
	m_nBank.nMajor.nMajorBlocks[1].push_back(m_nBank.nMajor.nMajorBlocks[1][m_nLvlBanks[nBasedOnLevel].nMajor.nMajorBlock[1]]);
	m_nBank.nMajor.nMajorBlocks[2].push_back(m_nBank.nMajor.nMajorBlocks[2][m_nLvlBanks[nBasedOnLevel].nMajor.nMajorBlock[2]]);
	m_nBank.nMajor.nMajorBlocks[3].push_back(m_nBank.nMajor.nMajorBlocks[3][m_nLvlBanks[nBasedOnLevel].nMajor.nMajorBlock[3]]);

	m_nBank.nMinor.nMinorBlocks[0].push_back(m_nBank.nMinor.nMinorBlocks[0][m_nLvlBanks[nBasedOnLevel].nMinor.nMinorBlocks[0]]);
	m_nBank.nMinor.nMinorBlocks[1].push_back(m_nBank.nMinor.nMinorBlocks[1][m_nLvlBanks[nBasedOnLevel].nMinor.nMinorBlocks[1]]);
	m_nBank.nMinor.nMinorBlocks[2].push_back(m_nBank.nMinor.nMinorBlocks[2][m_nLvlBanks[nBasedOnLevel].nMinor.nMinorBlocks[2]]);
	m_nBank.nMinor.nMinorBlocks[3].push_back(m_nBank.nMinor.nMinorBlocks[3][m_nLvlBanks[nBasedOnLevel].nMinor.nMinorBlocks[3]]);

	m_nBank.nMinor.nMinorBlocksPalettes.push_back(m_nBank.nMinor.nMinorBlocksPalettes[m_nLvlBanks[nBasedOnLevel].nMinor.nMinorBlocksPalettes]);

	m_nBank.nHeader.nBlocks.push_back(m_nBank.nHeader.nBlocks[m_nLvlBanks[nBasedOnLevel].nBlocksPtr]);

	std::vector<NES_OBJECTS_FLOOR> & objlst = m_nBank.nObjects[m_nCurrentLevel];
	size_t count = objlst.size();
	for(size_t n = 0; n < count; ++n)
		objlst[n].nObj.clear();

	if ( CacheLevel() )
		if ( CacheRoomsImages() )
			return TRUE;

	return FALSE;
}

BOOL CNesEditor::Get_ExtendedLevel()
{
	if ( !m_nGame.fExtended || m_nCurrentLevel != 0x06 )
		return FALSE;

	BYTE bBlocksPtr = m_nLvlBanks[m_nCurrentLevel].nBlocksPtr;

	for(size_t n = 0; n < NES_COUNT_LEVELS; ++n)
	{
		if ( n != m_nCurrentLevel && m_nLvlBanks[n].nBlocksPtr == bBlocksPtr )
			return TRUE;
	}

	return FALSE;
}


CNesTiles & CNesEditor::GetTileSet()
{
	return m_nTiles[m_nCurrentLevel];
}


BOOL CNesEditor::SetCurrentLevel(INT nLevel)
{
	if ( !m_fLoaded || nLevel < 0 || nLevel >= NES_COUNT_LEVELS )
		return FALSE;


	FlushLevel();

	m_nCurrentLevel = nLevel;

	return CacheLevel();
}


BOOL CNesEditor::Let_LevelBlocks(PBYTE pbPalette, PBYTE pbPalTiles, std::vector<NES_TILE> & vTiles, std::vector<NES_BLOCK> & vMinor, std::vector<NES_BLOCK> & vMajor)
{
	if ( m_nCurrentLevel == -1 || !m_fLoaded 
		|| vTiles.size() != 512 || vMinor.size() != 256 || !vMajor.size() )
		return FALSE;

	CopyMemory(m_nLevel.bPalette, pbPalette, sizeof(m_nLevel.bPalette));
	CopyMemory(m_nLevel.bPalTiles, pbPalTiles, sizeof(m_nLevel.bPalTiles));
	
	m_nLevel.nTiles = vTiles;
	m_nLevel.nMinorBlocks = vMinor;
	m_nLevel.nMajorBlocks = vMajor;

	return ( CacheBlocks() && CacheRoomsImages() );
}

BOOL CNesEditor::Let_LevelTiles(std::vector<NES_BLOCK> & vMinor, PBYTE pbPalTiles)
{
	if ( m_nCurrentLevel == -1 || !m_fLoaded || vMinor.size() != 256 )
		return FALSE;

	m_nLevel.nMinorBlocks = vMinor;
	CopyMemory(m_nLevel.bPalTiles, pbPalTiles, sizeof(m_nLevel.bPalTiles));

	return ( CacheBlocks() && CacheRoomsImages() );
}

BOOL CNesEditor::Get_LevelBlocks(PBYTE pbPalette, PBYTE pbPalTiles, std::vector<NES_TILE> & vTiles, std::vector<NES_BLOCK> & vMinor, std::vector<NES_BLOCK> & vMajor)
{
	if ( m_nCurrentLevel == -1 || !m_fLoaded )
		return FALSE;

	CopyMemory(pbPalette, m_nLevel.bPalette, sizeof(m_nLevel.bPalette));
	CopyMemory(pbPalTiles, m_nLevel.bPalTiles, sizeof(m_nLevel.bPalTiles));
	vTiles = m_nLevel.nTiles;
	vMinor = m_nLevel.nMinorBlocks;
	vMajor = m_nLevel.nMajorBlocks;
	return TRUE;
}

BOOL CNesEditor::Get_BlocksUsing(std::vector<BYTE> & vUsedBlocks)
{
	if ( m_nCurrentLevel == -1 || !m_fLoaded )
		return FALSE;

	vUsedBlocks.clear();
	vUsedBlocks.insert(vUsedBlocks.begin(), m_nLevel.nMajorBlocks.size(), 0);

	for(INT nRoom = 0; nRoom < sizeof(m_nLevel.bLayout) / sizeof(m_nLevel.bLayout[0]); ++nRoom)
	{
		if ( m_nLevel.bLayout[nRoom] )
		{
			NES_ROOM & nRoomData = m_nRooms[m_nLevel.bLayout[nRoom] - 1];
			for(INT y = 0; y < 8; ++y)
			{
				for(INT x = 0; x < 8; ++x)
					vUsedBlocks[nRoomData.bRoom[y][x]]++;
			}

		}
	}
	return TRUE;
}

BOOL CNesEditor::Get_TilesUsing(std::vector<BYTE> & vUsedTiles)
{
	if ( !m_fLoaded || -1 == m_nCurrentLevel )
		return FALSE;

	vUsedTiles = m_nLevel.vTilesUsed;
	return TRUE;
}

BOOL CNesEditor::Get_TileBankInfo(PBYTE pbTilesBanks)
{
	if ( !m_fLoaded || -1 == m_nCurrentLevel )
		return FALSE;

	for(INT i = 0; i < NES_COUNT_LEVELS; ++i)
		pbTilesBanks[i] = m_nLvlBanks[i].nMinor.nMinorBlocks[0];

	return TRUE;
}

BOOL CNesEditor::Let_TileBankInfo(BYTE bTilesBank, BOOL fCreateNewBank)
{
	if ( !m_fLoaded || -1 == m_nCurrentLevel )
		return FALSE;

	if ( fCreateNewBank )
	{
		m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocks[0] = LOBYTE(m_nBank.nMinor.nMinorBlocks[0].size());
		m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocks[1] = LOBYTE(m_nBank.nMinor.nMinorBlocks[1].size());
		m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocks[2] = LOBYTE(m_nBank.nMinor.nMinorBlocks[2].size());
		m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocks[3] = LOBYTE(m_nBank.nMinor.nMinorBlocks[3].size());
		m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocksPalettes = LOBYTE(m_nBank.nMinor.nMinorBlocksPalettes.size());

		std::vector<BYTE> vEmpty;
		m_nBank.nMinor.nMinorBlocks[0].push_back(vEmpty);
		m_nBank.nMinor.nMinorBlocks[1].push_back(vEmpty);
		m_nBank.nMinor.nMinorBlocks[2].push_back(vEmpty);
		m_nBank.nMinor.nMinorBlocks[3].push_back(vEmpty);
		m_nBank.nMinor.nMinorBlocksPalettes.push_back(vEmpty);
	}
	else
	{
		if ( bTilesBank >= LOBYTE(m_nBank.nMinor.nMinorBlocks[0].size()) )
			return FALSE;

		m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocks[0] = bTilesBank;
		m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocks[1] = bTilesBank;
		m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocks[2] = bTilesBank;
		m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocks[3] = bTilesBank;
		m_nLvlBanks[m_nCurrentLevel].nMinor.nMinorBlocksPalettes = bTilesBank;

		m_nLevel.nMinorBlocks.clear();
		size_t count = m_nBank.nMinor.nMinorBlocks[0][bTilesBank].size();
		for(size_t n = 0; n < count; ++n)
		{
			NES_BLOCK bl = { 
				m_nBank.nMinor.nMinorBlocks[0][bTilesBank][n],
				m_nBank.nMinor.nMinorBlocks[1][bTilesBank][n],
				m_nBank.nMinor.nMinorBlocks[2][bTilesBank][n],
				m_nBank.nMinor.nMinorBlocks[3][bTilesBank][n]
			};

			m_nLevel.nMinorBlocks.push_back(bl);
		}

		CopyMemory(m_nLevel.bPalTiles, &m_nBank.nMinor.nMinorBlocksPalettes[bTilesBank][0], sizeof(m_nLevel.bPalTiles));

		RemoveUnusedLinks();
	}

	return ( CacheBlocks() && CacheRoomsImages() );
}

BOOL CNesEditor::Get_LevelData(PBYTE pbPalette, BYTE & bSound, BYTE & bBossLife, BYTE & bBossType)
{
	if ( -1 == m_nCurrentLevel )
		return FALSE;

	bSound = m_nBank.nHeader.nHeader[m_nLvlBanks[m_nCurrentLevel].nHeaderPtr].bgSound;
	CopyMemory(pbPalette, m_nLevel.bPalette, sizeof(m_nLevel.bPalette));

	bBossLife = m_nGame.nBossLives[m_nCurrentLevel];
	bBossType = m_nGame.nBossTypes[m_nCurrentLevel];

	return TRUE;
}

BOOL CNesEditor::Let_LevelData(PBYTE pbPalette, BYTE bSound, BYTE bBossLife, BYTE bBossType)
{
	if ( -1 == m_nCurrentLevel )
		return FALSE;

	m_nBank.nHeader.nHeader[m_nLvlBanks[m_nCurrentLevel].nHeaderPtr].bgSound = bSound;
	CopyMemory(m_nLevel.bPalette, pbPalette, sizeof(m_nLevel.bPalette));
	m_nGame.nBossLives[m_nCurrentLevel] = bBossLife;
	m_nGame.nBossTypes[m_nCurrentLevel] = bBossType;

	return TRUE;
}

