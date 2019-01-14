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

INT CNesEditor::LoadFile(CNesFile & file, CTilesCreator & tiles, CTilesCreator & tilesScrooge)
{
	if ( !file.IsFileLoaded() )
		return NES_LOADER_ERROR_ACCESS;

	if ( m_fLoaded )
		return NES_LOADER_ERROR_ACCESS;


	m_pFile = &file;
	
	m_nGame.fExtended = file.IsExtended();

	if ( !LoadBanks(file) )
		return FALSE;

	if ( !m_nRooms.LoadRooms(file, Get_LastRoom()) )
	{
		m_pFile = NULL;
		return FALSE;
	}

	LoadBridgeData(file);
	LoadEgyptSecretData(file);
	LoadMuSecretData(file);

	m_fLoaded = TRUE;

	file.GetData(nfBgTiles, m_nGame.bBgTiles, sizeof(m_nGame.bBgTiles));
	file.GetData(nfLevelsNames, m_nGame.sLevelName, sizeof(m_nGame.sLevelName));
	file.GetData(nfBossLives, m_nGame.nBossLives, sizeof(m_nGame.nBossLives));
	file.GetData(nfBossTypes, m_nGame.nBossTypes, sizeof(m_nGame.nBossTypes));
	file.GetData(nfMoney50k, m_nGame.nMoney[0], sizeof(m_nGame.nMoney[0]));
	file.GetData(nfMoney10k, m_nGame.nMoney[1], sizeof(m_nGame.nMoney[1]));
	file.GetData(nfMoney1000k, m_nGame.nMoney[2], sizeof(m_nGame.nMoney[2]));
	file.GetData(nfDropsPtr, m_nGame.wDrops, sizeof(m_nGame.wDrops));

	LoadSprites(tiles, tilesScrooge);

	return TRUE;
}

INT CNesEditor::StorFile(CNesFile & file)
{
	if ( !file.IsFileLoaded() )
		return NES_LOADER_ERROR_ACCESS;

	file.PutData(nfBgTiles, m_nGame.bBgTiles, sizeof(m_nGame.bBgTiles));
	file.PutData(nfLevelsNames, m_nGame.sLevelName, sizeof(m_nGame.sLevelName));
	file.PutData(nfBossLives, m_nGame.nBossLives, sizeof(m_nGame.nBossLives));
	file.PutData(nfBossTypes, m_nGame.nBossTypes, sizeof(m_nGame.nBossTypes));
	file.PutData(nfMoney50k, m_nGame.nMoney[0], sizeof(m_nGame.nMoney[0]));
	file.PutData(nfMoney10k, m_nGame.nMoney[1], sizeof(m_nGame.nMoney[1]));
	file.PutData(nfMoney1000k, m_nGame.nMoney[2], sizeof(m_nGame.nMoney[2]));
	file.PutData(nfDropsPtr, m_nGame.wDrops, sizeof(m_nGame.wDrops));

	SetCurrentLevel(m_nCurrentLevel);
	SaveBanks(file);
	m_nRooms.StoreRooms(file);
	
	if ( file.IsExtended() && HIWORD(GetKeyState(VK_SHIFT)) )
	{
		MakeBridgePatch(file);
		MakeNoScrollPatch(file);
		MakeFasterSpritesPatch(file);
	}
	
	StorBridgeData(file);
	StorEgyptSecretData(file);
	StorMuSecretData(file);

	// after SaveBanks layouts ids or other can be changed, so reread data in CacheLevel
	return ( CacheLevel() );	
}

// extended methods
BOOL CNesEditor::LoadBanks(CNesFile & file)
{
	if ( !file.IsFileLoaded() )
		return FALSE;

	BOOL fResult = TRUE;

	std::map<INT, size_t>		nHeader;
	std::map<INT, size_t>		nBlocks;
	std::map<INT, size_t>		nPalette;

	std::map<INT, size_t>		nMinor[4];
	std::map<INT, size_t>		nMajor[4];
	std::map<INT, size_t>		nMinorPalettes;

	std::map<INT, size_t>		nLayout;

	std::map<INT, size_t>		nObjects;

	const BYTE nRoomsBank = file[0x1C936];
	const BYTE nBankBlocks = file[0x1C9BB];
	const BYTE nBankObjects = file[NES_OBJECTS_BANKID_PTR];
	const INT ptrObjectsArray = file.Word(NES_OBJECTS_PTR_PTR);

	for(INT level = 0; level < NES_COUNT_LEVELS; ++level)
	{
		BYTE level_offset = file[NES_LEVELS_LIST + level];
		NES_LEVEL_HEADER lh = { 0 };
		NES_LEVEL_BLOCKS lb = { 0 };

		file.GetDataEx(NES_LEVELS_PTRS + level_offset, &lh, sizeof(lh));
		file.GetDataEx(BANK_OFFSET(4) + lh.ptrBlocks, &lb, sizeof(lb));

		
/*
	USHORT ptrTiles;
	USHORT ptrPalette;
	BYTE bStartScrX;
	BYTE bStartPosX;
	BYTE bStartScrY;
	BYTE bStartPosY;
	BYTE bRestartScrX;
	BYTE bRestartPosX;
	BYTE bRestartScrY;
	BYTE bRestartPosY;
	BYTE bUnused[4];
	WORD ptrUnused;
	WORD ptrPPUCorrector;
	WORD ptrPPUSpritesCorrector;
	WORD ptrUnknown3;
	WORD ptrBlocks;
	BYTE bgSound;
*/
		_tprintf(TEXT("* Level %0d *\n"), level);
		_tprintf(TEXT(" * ptrTiles: 0x%08x\n"), lh.ptrTiles);
		_tprintf(TEXT(" * ptrPalette: 0x%08x\n"), lh.ptrPalette);
		_tprintf(TEXT(" * Screen X: 0x%08x\n"), lh.start.bScrX);
		_tprintf(TEXT(" * Pos X: 0x%08x\n"), lh.start.bPosX);
		_tprintf(TEXT(" * Screen Y: 0x%08x\n"), lh.start.bScrY);
		_tprintf(TEXT(" * Pos Y: 0x%08x\n"), lh.start.bPosY);
		_tprintf(TEXT(" * Point Screen X: 0x%08x\n"), lh.restart.bScrX);
		_tprintf(TEXT(" * Point Pos X: 0x%08x\n"), lh.restart.bPosX);
		_tprintf(TEXT(" * Point Screen Y: 0x%08x\n"), lh.restart.bScrY);
		_tprintf(TEXT(" * Point Pos Y: 0x%08x\n"), lh.restart.bPosY);
		_tprintf(TEXT(" * Boss Screen X: 0x%08x\n"), lh.bossrestart.bScrX);
		_tprintf(TEXT(" * Boss Pos X: 0x%08x\n"), lh.bossrestart.bPosX);
		_tprintf(TEXT(" * Boss Screen Y: 0x%08x\n"), lh.bossrestart.bScrY);
		_tprintf(TEXT(" * Boss Pos Y: 0x%08x\n"), lh.bossrestart.bPosY);
		_tprintf(TEXT(" * ptrUnused: 0x%08x\n"), lh.ptrUnused);
		_tprintf(TEXT(" * PPU correction ptr: 0x%08x\n"), lh.ptrPPUCorrector);
		_tprintf(TEXT(" * PPU sprites correction ptr: 0x%08x\n"), lh.ptrPPUSpritesCorrector);
		_tprintf(TEXT(" * PPU correction 2 ptr: 0x%08x\n"), lh.ptrUnknown3);
		_tprintf(TEXT(" * ptrBlocks: 0x%08x\n"), lh.ptrBlocks);
		_tprintf(TEXT(" * Sound: 0x%08x\n"), lh.bgSound);

		_tprintf(TEXT(" * ptrLayout: 0x%08x\n"), lb.ptrLayout);
		_tprintf(TEXT(" * ptrMajorBlocks1: 0x%08x\n"), lb.ptrMajorBlocks1);
		_tprintf(TEXT(" * ptrMajorBlocks2: 0x%08x\n"), lb.ptrMajorBlocks2);
		_tprintf(TEXT(" * ptrMajorBlocks3: 0x%08x\n"), lb.ptrMajorBlocks3);
		_tprintf(TEXT(" * ptrMajorBlocks4: 0x%08x\n"), lb.ptrMajorBlocks4);
		_tprintf(TEXT(" * ptrMinorBlocks1: 0x%08x\n"), lb.ptrMinorBlocks1);
		_tprintf(TEXT(" * ptrMinorBlocks2: 0x%08x\n"), lb.ptrMinorBlocks2);
		_tprintf(TEXT(" * ptrMinorBlocks3: 0x%08x\n"), lb.ptrMinorBlocks3);
		_tprintf(TEXT(" * ptrMinorBlocks4: 0x%08x\n"), lb.ptrMinorBlocks4);
		_tprintf(TEXT(" * ptrMinorBlocksPalettes: 0x%08x\n"), lb.ptrMinorBlocksPalettes);
		_tprintf(TEXT("\n\n"));

		m_nTiles[level].LoadTiles(file, lh);
		file.GetDataEx(BANK_OFFSET(5) + 0xBDC5, m_nBank.nPalLoaders, sizeof(m_nBank.nPalLoaders));
		file.GetData(0x10010 + 0xC273, m_nBank.nObjHandlers, sizeof(m_nBank.nObjHandlers));

		if ( nHeader.find(level_offset) == nHeader.end() )
		{
			nHeader[level_offset] = m_nBank.nHeader.nHeader.size();
			m_nBank.nHeader.nHeader.push_back(lh);

			if ( nBlocks.find(lh.ptrBlocks) == nBlocks.end() )
			{
				nBlocks[lh.ptrBlocks] = m_nBank.nHeader.nBlocks.size();

				m_nBank.nHeader.nBlocks.push_back(lb);

				if ( nLayout.find(lb.ptrLayout) == nLayout.end() )
				{
					nLayout[lb.ptrLayout] = m_nBank.nLayout.size();

					NES_LAYOUT nl = { 0 };
					file.GetDataEx(BANK_OFFSET(nRoomsBank) + lb.ptrLayout, &nl, sizeof(nl));

					m_nBank.nLayout.push_back(nl);
				}

				if ( nMinor[0].find(lb.ptrMinorBlocks1) == nMinor[0].end() )
				{
					nMinor[0][lb.ptrMinorBlocks1] = m_nBank.nMinor.nMinorBlocks[0].size();

					std::vector<BYTE> v;
					v.insert(v.begin(), 256, 0);
					file.GetDataEx(BANK_OFFSET(4) + lb.ptrMinorBlocks1, &v[0], v.size());
					m_nBank.nMinor.nMinorBlocks[0].push_back(v);
				}

				if ( nMinor[1].find(lb.ptrMinorBlocks2) == nMinor[1].end() )
				{
					nMinor[1][lb.ptrMinorBlocks2] = m_nBank.nMinor.nMinorBlocks[1].size();

					std::vector<BYTE> v;
					v.insert(v.begin(), 256, 0);
					file.GetDataEx(BANK_OFFSET(4) + lb.ptrMinorBlocks2, &v[0], v.size());
					m_nBank.nMinor.nMinorBlocks[1].push_back(v);
				}

				if ( nMinor[2].find(lb.ptrMinorBlocks3) == nMinor[2].end() )
				{
					nMinor[2][lb.ptrMinorBlocks3] = m_nBank.nMinor.nMinorBlocks[2].size();

					std::vector<BYTE> v;
					v.insert(v.begin(), 256, 0);
					file.GetDataEx(BANK_OFFSET(4) + lb.ptrMinorBlocks3, &v[0], v.size());
					m_nBank.nMinor.nMinorBlocks[2].push_back(v);
				}

				if ( nMinor[3].find(lb.ptrMinorBlocks4) == nMinor[3].end() )
				{
					nMinor[3][lb.ptrMinorBlocks4] = m_nBank.nMinor.nMinorBlocks[3].size();

					std::vector<BYTE> v;
					v.insert(v.begin(), 256, 0);
					file.GetDataEx(BANK_OFFSET(4) + lb.ptrMinorBlocks4, &v[0], v.size());
					m_nBank.nMinor.nMinorBlocks[3].push_back(v);
				}

				if ( nMinorPalettes.find(lb.ptrMinorBlocksPalettes) == nMinorPalettes.end() )
				{
					nMinorPalettes[lb.ptrMinorBlocksPalettes] = m_nBank.nMinor.nMinorBlocksPalettes.size();

					std::vector<BYTE> v;
					v.insert(v.begin(), 64, 0);
					file.GetDataEx(BANK_OFFSET(4) + lb.ptrMinorBlocksPalettes, &v[0], v.size());
					m_nBank.nMinor.nMinorBlocksPalettes.push_back(v);
				}

				if ( lb.ptrMajorBlocks2 <= lb.ptrMajorBlocks1 )
				{
					fResult = FALSE;
					break;
				}

				size_t cMajor = lb.ptrMajorBlocks2 - lb.ptrMajorBlocks1;


				if ( nMajor[0].find(lb.ptrMajorBlocks1) == nMajor[0].end() )
				{
					nMajor[0][lb.ptrMajorBlocks1] = m_nBank.nMajor.nMajorBlocks[0].size();

					std::vector<BYTE> v;
					v.insert(v.begin(), cMajor, 0);
					file.GetDataEx(BANK_OFFSET(nBankBlocks) + lb.ptrMajorBlocks1, &v[0], v.size());
					m_nBank.nMajor.nMajorBlocks[0].push_back(v);
				}

				if ( nMajor[1].find(lb.ptrMajorBlocks2) == nMajor[1].end() )
				{
					nMajor[1][lb.ptrMajorBlocks2] = m_nBank.nMajor.nMajorBlocks[1].size();

					std::vector<BYTE> v;
					v.insert(v.begin(), cMajor, 0);
					file.GetDataEx(BANK_OFFSET(nBankBlocks) + lb.ptrMajorBlocks2, &v[0], v.size());
					m_nBank.nMajor.nMajorBlocks[1].push_back(v);
				}

				if ( nMajor[2].find(lb.ptrMajorBlocks3) == nMajor[2].end() )
				{
					nMajor[2][lb.ptrMajorBlocks3] = m_nBank.nMajor.nMajorBlocks[2].size();

					std::vector<BYTE> v;
					v.insert(v.begin(), cMajor, 0);
					file.GetDataEx(BANK_OFFSET(nBankBlocks) + lb.ptrMajorBlocks3, &v[0], v.size());
					m_nBank.nMajor.nMajorBlocks[2].push_back(v);
				}

				if ( nMajor[3].find(lb.ptrMajorBlocks4) == nMajor[3].end() )
				{
					nMajor[3][lb.ptrMajorBlocks4] = m_nBank.nMajor.nMajorBlocks[3].size();

					std::vector<BYTE> v;
					v.insert(v.begin(), cMajor, 0);
					file.GetDataEx(BANK_OFFSET(nBankBlocks) + lb.ptrMajorBlocks4, &v[0], v.size());
					m_nBank.nMajor.nMajorBlocks[3].push_back(v);
				}
			}

			if ( nPalette.find(lh.ptrPalette) == nPalette.end() )
			{
				nPalette[lh.ptrPalette] = m_nBank.nPalette.size();

				std::vector<BYTE> pal;
				pal.insert(pal.begin(), 32, 0);
				file.GetDataEx( BANK_OFFSET(0) + lh.ptrPalette, &pal[0], pal.size() );
				m_nBank.nPalette.push_back(pal);
			}

		}

		// WARNING: ptrBlocks have NOT offsets here! It contains number of bank

		NES_LEVEL_HEADER dlh = lh;
		m_nLvlBanks[level].nHeaderPtr = nHeader[level_offset];
		lh.ptrBlocks = m_nLvlBanks[level].nBlocksPtr = nBlocks[lh.ptrBlocks];
		lh.ptrPalette = m_nLvlBanks[level].nPalette = nPalette[lh.ptrPalette];	
		lb.ptrLayout = m_nLvlBanks[level].nLayout = nLayout[lb.ptrLayout];

		lb.ptrMinorBlocks1 = m_nLvlBanks[level].nMinor.nMinorBlocks[0] = nMinor[0][lb.ptrMinorBlocks1];
		lb.ptrMinorBlocks2 = m_nLvlBanks[level].nMinor.nMinorBlocks[1] = nMinor[1][lb.ptrMinorBlocks2];
		lb.ptrMinorBlocks3 = m_nLvlBanks[level].nMinor.nMinorBlocks[2] = nMinor[2][lb.ptrMinorBlocks3];
		lb.ptrMinorBlocks4 = m_nLvlBanks[level].nMinor.nMinorBlocks[3] = nMinor[3][lb.ptrMinorBlocks4];
		lb.ptrMinorBlocksPalettes = m_nLvlBanks[level].nMinor.nMinorBlocksPalettes = nMinorPalettes[lb.ptrMinorBlocksPalettes];

		lb.ptrMajorBlocks1 = m_nLvlBanks[level].nMajor.nMajorBlock[0] = nMajor[0][lb.ptrMajorBlocks1];
		lb.ptrMajorBlocks2 = m_nLvlBanks[level].nMajor.nMajorBlock[1] = nMajor[1][lb.ptrMajorBlocks2];
		lb.ptrMajorBlocks3 = m_nLvlBanks[level].nMajor.nMajorBlock[2] = nMajor[2][lb.ptrMajorBlocks3];
		lb.ptrMajorBlocks4 = m_nLvlBanks[level].nMajor.nMajorBlock[3] = nMajor[3][lb.ptrMajorBlocks4];

		m_nBank.nHeader.nHeader[nHeader[level_offset]] = lh;
		m_nBank.nHeader.nBlocks[nBlocks[dlh.ptrBlocks]] = lb;

		WORD nObjPtrList[6] = { 0 };
		file.GetDataEx(BANK_OFFSET(nBankObjects) + ptrObjectsArray + 6 * sizeof(WORD) * level, nObjPtrList, sizeof(nObjPtrList));

		const size_t nObjEof = sizeof(nObjPtrList) / sizeof(nObjPtrList[0]);

		for(size_t no = 0; no < nObjEof; ++no)
		{
			NES_OBJECTS_FLOOR nof;
				
			INT ptr = BANK_OFFSET(nBankObjects) + INT(nObjPtrList[no]);

			if ( file.ByteEx(ptr++) != 0xFF )	// invalid pointer
				continue;

			nof.bFloor = file.ByteEx(ptr++);
			nof.fLastFloor = ( no == nObjEof - 1 );

			if ( nof.bFloor != 0xFF )
			{
				while(file.ByteEx(ptr) != 0xFF && nof.nObj.size() < 255 )
				{
					NES_OBJECT nob = { 0 };
					file.GetDataEx(ptr, &nob, sizeof(nob));
					nof.nObj.push_back(nob);
					ptr += 3;
				}
			}
			m_nBank.nObjects[level].push_back(nof);
		}
	}

	// load hidden slots
	INT ptrSteps = file.Word(nfHiddensSlotsPtrsArray), maxlevel = ( m_nGame.fExtended ? NES_COUNT_LEVELS : 1 );
	WORD wSteps[NES_COUNT_LEVELS][8] = { 0 };
	file.GetData(BANK_OFFSET(file.Bank(nfHiddensSlotsPtrsArray)) + ptrSteps, &wSteps, sizeof(wSteps));
	
	for(INT lvl = 0; lvl < maxlevel; ++lvl)
	{
		for(INT i = 0; i < 7; ++i)
		{
			INT ptr = wSteps[lvl][i];

			if ( !ptr )
				continue;

			do
			{
				NES_POINT pt = { 0 };
				file.GetData(BANK_OFFSET(file.Bank(nfHiddensSlotsPtrsArray)) + ptr, &pt, sizeof(pt));
				if ( !pt.bPosX && !pt.bScrX )
					break;

				m_nGame.nSteps[lvl][i].push_back(pt);
				ptr += sizeof(pt);
			} while(ptr < 0xBFFF);
		}
	}

	return fResult;
}

BOOL CNesEditor::SaveObjects(CNesFile & file)
{
	if ( !file.IsFileLoaded() )
		return FALSE;

	std::map<BYTE, INT>		nObjects;

	const BYTE nBankObjects = file[NES_OBJECTS_BANKID_PTR];
	const INT ptrObjectsArray = file.Word(NES_OBJECTS_PTR_PTR);
	const INT ptrObjects = ptrObjectsArray + 6 * sizeof(WORD) * NES_COUNT_LEVELS;

	INT ptr = ptrObjects;

	for(INT level = 0; level < NES_COUNT_LEVELS; ++level)
	{
		INT romptr = BANK_OFFSET(nBankObjects) + ptr;
		
		WORD wptr[6] = { 0 };

		std::map<BYTE, WORD> nobj_list;

		size_t count = m_nBank.nObjects[level].size();
		for(size_t n = 0; n < count; ++n)
		{
			NES_OBJECTS_FLOOR & nof = m_nBank.nObjects[level][n];
			wptr[n] = ptr;

			if ( nof.bFloor != 0xFF )
			{
				if ( nobj_list.find(nof.bFloor) == nobj_list.end() )
				{
					file.ByteEx(romptr++, 0xFF);
					file.ByteEx(romptr++, nof.bFloor);

					size_t co = m_nBank.nObjects[level][n].nObj.size();
					for(size_t no = 0; no < co; ++no)
					{
						file.PutDataEx(romptr, &m_nBank.nObjects[level][n].nObj[no], sizeof(m_nBank.nObjects[level][n].nObj[no]));

						if ( m_nBank.nObjects[level][n].nObj[no].id == 0x5e )
						{
							switch(level)
							{
							case 0x01:	// boss for level #2 [bermuda]
								{
									file[nfBoss2XOffset] = BYTE(m_nBank.nObjects[level][n].nObj[no].screen) + 1;
									break;
								}
							case 0x03:	// boss for level #4 [mu]
								{
									file[nfBoss4XOffset] = BYTE(m_nBank.nObjects[level][n].nObj[no].screen) - 1;
									break;
								}
							}
						}

						romptr += 3;
					}
					file.ByteEx(romptr, 0xFF);
					nobj_list[nof.bFloor] = ptr;

					ptr += ( 2 + sizeof(NES_OBJECT) * co );					
				}
				else
					wptr[n] = nobj_list[nof.bFloor];
			}
		}
		file.WordEx(romptr, 0xFFFF);
		ptr += 2;

		file.PutDataEx(BANK_OFFSET(nBankObjects) + ptrObjectsArray + sizeof(wptr) * level, wptr, sizeof(wptr));
	}

	return TRUE;
}

BOOL CNesEditor::SaveBanks(CNesFile & file)
{
	if ( !SaveObjects(file) )
		return FALSE;

	if ( m_nGame.fExtended )
	{
		RemoveUnusedRooms();
		RemoveUnusedLinks();
	}

	std::map<BYTE, INT>		nHeader;
	std::map<BYTE, INT>		nBlocks;
	std::map<BYTE, INT>		nPalette;

	std::map<BYTE, INT>		nMinor[4];
	std::map<BYTE, INT>		nMajor[4];
	std::map<BYTE, INT>		nMinorPalettes;

	std::map<BYTE, INT>		nLayout;

	std::map<BYTE, INT>		nObjects;

	const BYTE nRoomsBank = file[0x1C936];
	const BYTE nBankBlocks = file[0x1C9BB];

	const INT ptrMajorBlocks = ( !file.IsExtended() ? 0x8D3A : 0x8000 );
	const INT ptrPalettes = 0xBE1F;
	const INT ptrRooms = ( !file.IsExtended() ? 0x9C2A : 0x8010 );
	const INT ptrptrBlocks = 0x8002;
	const INT ptrMinorBlocks = ( ptrptrBlocks + sizeof(NES_LEVEL_BLOCKS) * m_nBank.nHeader.nBlocks.size() );
	const INT ptrLevels = 0xAE07;
	const INT ptrLevelsOffsets = 0xAE00;

	size_t count = 0;
	INT ptr = 0;

	// fix ptrs data
	for(INT level = 0; level < NES_COUNT_LEVELS; ++level)
	{
		m_nBank.nHeader.nBlocks[m_nLvlBanks[level].nBlocksPtr].ptrLayout = m_nLvlBanks[level].nLayout;

		m_nBank.nHeader.nBlocks[m_nLvlBanks[level].nBlocksPtr].ptrMajorBlocks1 = m_nLvlBanks[level].nMajor.nMajorBlock[0];
		m_nBank.nHeader.nBlocks[m_nLvlBanks[level].nBlocksPtr].ptrMajorBlocks2 = m_nLvlBanks[level].nMajor.nMajorBlock[1];
		m_nBank.nHeader.nBlocks[m_nLvlBanks[level].nBlocksPtr].ptrMajorBlocks3 = m_nLvlBanks[level].nMajor.nMajorBlock[2];
		m_nBank.nHeader.nBlocks[m_nLvlBanks[level].nBlocksPtr].ptrMajorBlocks4 = m_nLvlBanks[level].nMajor.nMajorBlock[3];

		m_nBank.nHeader.nBlocks[m_nLvlBanks[level].nBlocksPtr].ptrMinorBlocks1 = m_nLvlBanks[level].nMinor.nMinorBlocks[0];
		m_nBank.nHeader.nBlocks[m_nLvlBanks[level].nBlocksPtr].ptrMinorBlocks2 = m_nLvlBanks[level].nMinor.nMinorBlocks[1];
		m_nBank.nHeader.nBlocks[m_nLvlBanks[level].nBlocksPtr].ptrMinorBlocks3 = m_nLvlBanks[level].nMinor.nMinorBlocks[2];
		m_nBank.nHeader.nBlocks[m_nLvlBanks[level].nBlocksPtr].ptrMinorBlocks4 = m_nLvlBanks[level].nMinor.nMinorBlocks[3];

		m_nBank.nHeader.nBlocks[m_nLvlBanks[level].nBlocksPtr].ptrMinorBlocksPalettes = m_nLvlBanks[level].nMinor.nMinorBlocksPalettes;

		m_nBank.nHeader.nHeader[m_nLvlBanks[level].nHeaderPtr].ptrPalette = m_nLvlBanks[level].nPalette;
		m_nBank.nHeader.nHeader[m_nLvlBanks[level].nHeaderPtr].ptrBlocks = m_nLvlBanks[level].nBlocksPtr;
	}

	Banks nBank = m_nBank;

	for(INT i = 0; i < 4; ++i)
		count = max(count, nBank.nMinor.nMinorBlocks[i].size());

	count = max(count, nBank.nMinor.nMinorBlocksPalettes.size());
	ptr = ptrMinorBlocks;
	for(size_t n = 0; n < count; ++n)
	{
		for(INT i = 0; i < 4; ++i)
		{
			if ( n < nBank.nMinor.nMinorBlocks[i].size() )
			{
				file.PutDataEx(BANK_OFFSET(4) + ptr, &nBank.nMinor.nMinorBlocks[i][n][0], nBank.nMinor.nMinorBlocks[i][n].size());
				nMinor[i][LOBYTE(n)] = ptr;
				ptr += nBank.nMinor.nMinorBlocks[i][n].size();
			}
		}

		if ( n < nBank.nMinor.nMinorBlocksPalettes.size() )
		{
			file.PutDataEx(BANK_OFFSET(4) + ptr, &nBank.nMinor.nMinorBlocksPalettes[n][0], nBank.nMinor.nMinorBlocksPalettes[n].size());
			nMinorPalettes[LOBYTE(n)] = ptr;
			ptr += nBank.nMinor.nMinorBlocksPalettes[n].size();
		}
	}

	count = nBank.nMajor.nMajorBlocks[0].size();
	ptr = ptrMajorBlocks;
	for(size_t n = 0; n < count; ++n)
	{
		for(INT i = 0; i < 4; ++i)
		{
			file.PutDataEx(BANK_OFFSET(nBankBlocks) + ptr, &nBank.nMajor.nMajorBlocks[i][n][0], nBank.nMajor.nMajorBlocks[i][n].size());
			nMajor[i][LOBYTE(n)] = ptr;
			ptr += nBank.nMajor.nMajorBlocks[i][n].size();
		}
	}

	count = nBank.nPalette.size();
	ptr = ptrPalettes;
	for(size_t n = 0; n < count; ++n)
	{
		file.PutDataEx(BANK_OFFSET(0) + ptr, &nBank.nPalette[n][0], nBank.nPalette[n].size());
		nPalette[LOBYTE(n)] = ptr;
		ptr += nBank.nPalette[n].size();

		if ( ptr >= 0xBEDF && ptr < 0xBF7F )
			ptr = 0xBF7F;		// skip some sprites palettes
	}

	count = nBank.nLayout.size();
	ptr = ptrRooms;
	for(size_t n = 0; n < count; ++n)
	{
		file.PutDataEx(BANK_OFFSET(nRoomsBank) + ptr, &nBank.nLayout[n].bRoom[0], sizeof(nBank.nLayout[n]));
		nLayout[LOBYTE(n)] = ptr;
		ptr += sizeof(nBank.nLayout[n]);
	}
	file.WordEx(BANK_OFFSET(nRoomsBank) + 0x8000, ptr - 0x8040);	// fix offset to rooms data

	count = nBank.nHeader.nBlocks.size();
	ptr = ptrptrBlocks;
	for(size_t n = 0; n < count; ++n)
	{
		nBank.nHeader.nBlocks[n].ptrLayout = LOWORD(nLayout[LOBYTE(nBank.nHeader.nBlocks[n].ptrLayout)]);

		nBank.nHeader.nBlocks[n].ptrMajorBlocks1 = LOWORD(nMajor[0][LOBYTE(nBank.nHeader.nBlocks[n].ptrMajorBlocks1)]);
		nBank.nHeader.nBlocks[n].ptrMajorBlocks2 = LOWORD(nMajor[1][LOBYTE(nBank.nHeader.nBlocks[n].ptrMajorBlocks2)]);
		nBank.nHeader.nBlocks[n].ptrMajorBlocks3 = LOWORD(nMajor[2][LOBYTE(nBank.nHeader.nBlocks[n].ptrMajorBlocks3)]);
		nBank.nHeader.nBlocks[n].ptrMajorBlocks4 = LOWORD(nMajor[3][LOBYTE(nBank.nHeader.nBlocks[n].ptrMajorBlocks4)]);

		nBank.nHeader.nBlocks[n].ptrMinorBlocks1 = LOWORD(nMinor[0][LOBYTE(nBank.nHeader.nBlocks[n].ptrMinorBlocks1)]);
		nBank.nHeader.nBlocks[n].ptrMinorBlocks2 = LOWORD(nMinor[1][LOBYTE(nBank.nHeader.nBlocks[n].ptrMinorBlocks2)]);
		nBank.nHeader.nBlocks[n].ptrMinorBlocks3 = LOWORD(nMinor[2][LOBYTE(nBank.nHeader.nBlocks[n].ptrMinorBlocks3)]);
		nBank.nHeader.nBlocks[n].ptrMinorBlocks4 = LOWORD(nMinor[3][LOBYTE(nBank.nHeader.nBlocks[n].ptrMinorBlocks4)]);

		nBank.nHeader.nBlocks[n].ptrMinorBlocksPalettes = LOWORD(nMinorPalettes[LOBYTE(nBank.nHeader.nBlocks[n].ptrMinorBlocksPalettes)]);

		file.PutDataEx(BANK_OFFSET(4) + ptr, &nBank.nHeader.nBlocks[n], sizeof(nBank.nHeader.nBlocks[n]));
		nBlocks[LOBYTE(n)] = ptr;

		ptr += sizeof(nBank.nHeader.nBlocks[n]);

	}

	count = nBank.nHeader.nHeader.size();
	ptr = ptrLevels;

	for(size_t n = 0; n < count; ++n)
	{
		nBank.nHeader.nHeader[n].ptrPalette = LOWORD(nPalette[LOBYTE(nBank.nHeader.nHeader[n].ptrPalette)]);
		nBank.nHeader.nHeader[n].ptrBlocks = LOWORD(nBlocks[LOBYTE(nBank.nHeader.nHeader[n].ptrBlocks)]);
		file.PutDataEx(BANK_OFFSET(0) + ptr, &nBank.nHeader.nHeader[n], sizeof(nBank.nHeader.nHeader[n]));
		nHeader[LOBYTE(n)] = ptr;

		ptr += sizeof(nBank.nHeader.nHeader[n]);
	}

	for(INT level = 0; level < NES_COUNT_LEVELS; ++level)
	{
		file.ByteEx(BANK_OFFSET(0) + ptrLevelsOffsets + level, LOBYTE( nHeader[m_nLvlBanks[level].nHeaderPtr] - ptrLevels ));
	}

	// store palettes loaders pointers
	file.PutDataEx(BANK_OFFSET(5) + 0xBDC5, m_nBank.nPalLoaders, sizeof(m_nBank.nPalLoaders));
	file.PutData(0x10010 + 0xC273, m_nBank.nObjHandlers, sizeof(m_nBank.nObjHandlers));

	// store hidden slots
	INT ptrSteps = file.Word(nfHiddensSlotsPtrsArray), maxlevel = ( m_nGame.fExtended ? NES_COUNT_LEVELS : 1 );
	WORD wSteps[NES_COUNT_LEVELS][8] = { 0 }, wFirstStep = 0xFFFF;
	file.GetDataEx(BANK_OFFSET(file.Bank(nfHiddensSlotsPtrsArray)) + ptrSteps, &wSteps, sizeof(wSteps));
	for(int lvl = 0; lvl < maxlevel; ++lvl)
		for(size_t n = 0; n < 7; ++n)
			if ( wSteps[lvl][n] ) 
				wFirstStep = min(wSteps[lvl][n], wFirstStep);

	for(int lvl = 0; lvl < maxlevel; ++lvl)
	{
		for(INT i = 0; i < 7; ++i)
		{
			wSteps[lvl][i] = wFirstStep;
			size_t count = m_nGame.nSteps[lvl][i].size();

			for(size_t n = 0; n < count; ++n)
			{
				NES_POINT & pt = m_nGame.nSteps[lvl][i][n];

				if ( !pt.bPosX && !pt.bPosY && !pt.bScrX && !pt.bScrY )
					continue;

				file.PutDataEx(BANK_OFFSET(file.Bank(nfHiddensSlotsPtrsArray)) + INT(wFirstStep), &pt, sizeof(pt));
				wFirstStep += sizeof(pt);
			}

			file.WordEx(BANK_OFFSET(file.Bank(nfHiddensSlotsPtrsArray)) + INT(wFirstStep), 0);
			wFirstStep += 2;
		}
	}

	file.PutDataEx(BANK_OFFSET(file.Bank(nfHiddensSlotsPtrsArray)) + ptrSteps, &wSteps, sizeof(wSteps));

	return TRUE;
}

BOOL CNesEditor::RemoveUnusedLinks()
{
#ifdef DEBUG
#define REMOVE_GARBAGE(t, mb1, mb2)	{										\
	BYTE bLinks[NES_COUNT_LEVELS] = { 0 };									\
	for(INT i = 0; i < NES_COUNT_LEVELS; ++i)								\
		bLinks[i] = m_nLvlBanks[i].mb1;										\
	CNesGarbageRemoval<t> vLayout(m_nBank.mb2, bLinks);						\
	_tprintf(TEXT( #mb2 ) TEXT(" removed %d items.\n"), vLayout.RemoveUnused());	 \
	for(INT i = 0; i < NES_COUNT_LEVELS; ++i)								\
		m_nLvlBanks[i].mb1 = bLinks[i];										\
									}
#else
#define REMOVE_GARBAGE(t, mb1, mb2)	{										\
	BYTE bLinks[NES_COUNT_LEVELS] = { 0 };									\
	for(INT i = 0; i < NES_COUNT_LEVELS; ++i)								\
		bLinks[i] = m_nLvlBanks[i].mb1;										\
	CNesGarbageRemoval<t> vLayout(m_nBank.mb2, bLinks);						\
	vLayout.RemoveUnused();	 \
	for(INT i = 0; i < NES_COUNT_LEVELS; ++i)								\
		m_nLvlBanks[i].mb1 = bLinks[i];										\
									}
#endif

	REMOVE_GARBAGE(NES_LAYOUT,			nLayout,	nLayout);
	REMOVE_GARBAGE(std::vector<BYTE>,	nPalette,	nPalette);

	REMOVE_GARBAGE(std::vector<BYTE>,	nMinor.nMinorBlocks[0], nMinor.nMinorBlocks[0]);
	REMOVE_GARBAGE(std::vector<BYTE>,	nMinor.nMinorBlocks[1], nMinor.nMinorBlocks[1]);
	REMOVE_GARBAGE(std::vector<BYTE>,	nMinor.nMinorBlocks[2], nMinor.nMinorBlocks[2]);
	REMOVE_GARBAGE(std::vector<BYTE>,	nMinor.nMinorBlocks[3], nMinor.nMinorBlocks[3]);

	REMOVE_GARBAGE(std::vector<BYTE>,	nMinor.nMinorBlocksPalettes, nMinor.nMinorBlocksPalettes);

	REMOVE_GARBAGE(std::vector<BYTE>,	nMajor.nMajorBlock[0], nMajor.nMajorBlocks[0]);
	REMOVE_GARBAGE(std::vector<BYTE>,	nMajor.nMajorBlock[1], nMajor.nMajorBlocks[1]);
	REMOVE_GARBAGE(std::vector<BYTE>,	nMajor.nMajorBlock[2], nMajor.nMajorBlocks[2]);
	REMOVE_GARBAGE(std::vector<BYTE>,	nMajor.nMajorBlock[3], nMajor.nMajorBlocks[3]);
#undef REMOVE_GARBAGE

	return TRUE;
}

BOOL CNesEditor::RemoveUnusedRooms()
{
	std::vector<BYTE> vRooms;
	std::vector<BYTE> vUnusedRooms;
	std::vector<BYTE> vNewRoomsList;

	std::map<BYTE, std::vector<WORD>> mRooms;

	size_t count = m_nBank.nLayout.size();
	for(size_t n = 0; n < count; ++n)
	{
		NES_LAYOUT & nl = m_nBank.nLayout[n];
		vRooms.insert(vRooms.end(), sizeof(nl.bRoom), 0);

		for(size_t i = 0; i < sizeof(nl.bRoom); ++i)
		{
			vRooms[n * sizeof(nl.bRoom) + i] = nl.bRoom[i];
			if ( nl.bRoom[i] )
				mRooms[nl.bRoom[i]].push_back(n * sizeof(nl.bRoom) + i);
		}
	}

	BYTE max_id = m_nRooms.RoomsCount();
	for(BYTE n = 1; n <= max_id; ++n)
	{
		if ( mRooms.find(n) == mRooms.end() )
		{
			vUnusedRooms.push_back(n);
		}
	}

	vNewRoomsList.insert(vNewRoomsList.begin(), vRooms.size(), 0);
	BYTE room_id = 1;

	for(std::map<BYTE, std::vector<WORD>>::iterator v = mRooms.begin(); v != mRooms.end(); v++)
	{
		if ( v->second.size() > 0 )
		{
			size_t count = v->second.size();
			for(size_t n = 0; n < count; ++n)
				vNewRoomsList[v->second[n]] = room_id;

			room_id++;
		}
	}

#ifdef DEBUG
	_tprintf(TEXT("Unused rooms: "));
	for(size_t c = 0; c < vUnusedRooms.size(); ++c)
	{
		_tprintf(TEXT("%02x %s"), vUnusedRooms[c], !((c + 1) % 8) ? TEXT("\n") : TEXT(""));
	}
	_tprintf(TEXT("\n"));
#endif

	count = m_nBank.nLayout.size();
	for(size_t n = 0; n < count; ++n)
	{
		NES_LAYOUT & nl = m_nBank.nLayout[n];
		CopyMemory(nl.bRoom, &vNewRoomsList[n * sizeof(nl.bRoom)], sizeof(nl.bRoom));
	}

#ifdef DEBUG
	_tprintf(TEXT("New layouts: "));
	for(size_t c = 0; c < vNewRoomsList.size(); ++c)
	{
		_tprintf(TEXT("%02x %s%s"), vNewRoomsList[c], !((c + 1) % 8) ? TEXT("\n") : TEXT(""), !((c + 1) % 48) ? TEXT("\n") : TEXT(""));
	}
	_tprintf(TEXT("\n"));
#endif


	return m_nRooms.DeleteRooms(vUnusedRooms);
}

BOOL CNesEditor::LoadMuSecretData(CNesFile & file)
{
	if ( !file.IsFileLoaded() )
		return FALSE;

	BYTE nBankMuSecret = file.Bank(nfMuSecretDataPtr);

	m_nGame.nMuSecret.bObjId = file[nfMuSecretObjectId]; // file.ByteEx(BANK_OFFSET(nBankMuSecret) + 0xB47C);
	m_nGame.nMuSecret.bPosY = file[nfMuSecretPosY]; // file.ByteEx(BANK_OFFSET(nBankMuSecret) + 0xB486);

	WORD ramptr = file.Word(nfMuSecretPosType); // WordEx(BANK_OFFSET(nBankMuSecret) + 0xB483);
	if ( 0x4DE == ramptr )
		m_nGame.nMuSecret.fVertical = FALSE;
	else if ( 0x4AE == ramptr )
		m_nGame.nMuSecret.fVertical = TRUE;
	else
	{
		m_nGame.nMuSecret.bObjId = 0;
		m_nGame.nMuSecret.bPosY = 0;
		return FALSE;
	}

	size_t count = 1 + (file[nfMuSecretCounter] /*.ByteEx(BANK_OFFSET(nBankMuSecret) + 0xB48A */ ) / 3;
	if ( count > 8 )
	{
		m_nGame.nMuSecret.bObjId = 0;
		m_nGame.nMuSecret.bPosY = 0;
		return FALSE;
	}

	WORD ptr = file.Word(nfMuSecretDataPtr); // WordEx(BANK_OFFSET(nBankMuSecret) + 0xB48F);
	for(size_t n = 0; n < count; ++n)
	{
		BYTE bData[3] = { 0 };
		file.GetDataEx(BANK_OFFSET(nBankMuSecret) + INT(ptr) + sizeof(bData) * n, bData, sizeof(bData));

		NES_DOT nd;
		nd.bPos = bData[0]; nd.bScr = bData[1];
		m_nGame.nMuSecret.vobjX.push_back(nd);
	}

	return TRUE;
}

BOOL CNesEditor::StorMuSecretData(CNesFile & file)
{
	if ( !file.IsFileLoaded() )
		return FALSE;

	if ( !m_nGame.nMuSecret.bObjId ||
		( m_nGame.nMuSecret.vobjX.size() > 2 && !file.IsExtended() ) ||
		( m_nGame.nMuSecret.vobjX.size() >= 8 && file.IsExtended() ) ||
		!m_nGame.nMuSecret.vobjX.size()
		)
		return FALSE;

	BYTE nBankMuSecret = file.Bank(nfMuSecretDataPtr);

	WORD ptr = file.Word(nfMuSecretDataPtr); // WordEx(BANK_OFFSET(nBankMuSecret) + 0xB48F);
	if ( m_nGame.nMuSecret.vobjX.size() > 2 )
		ptr = file.Ptr(nfMuSecretDataPtrVal); // 0x9B80;

	file[nfMuSecretCounter] = LOBYTE( ( m_nGame.nMuSecret.vobjX.size() - 1 ) * 3 );
	file[nfMuSecretCounter2] = LOBYTE( ( m_nGame.nMuSecret.vobjX.size() - 1 ) * 3 );
	file[nfMuSecretObjectId] = m_nGame.nMuSecret.bObjId;
	file[nfMuSecretObjectId2] = m_nGame.nMuSecret.bObjId;

	// file.ByteEx(BANK_OFFSET(nBankMuSecret) + 0xB48A, LOBYTE( ( m_nGame.nMuSecret.vobjX.size() - 1 ) * 3 ));
	// file.ByteEx(BANK_OFFSET(nBankMuSecret) + 0xB44C, LOBYTE( ( m_nGame.nMuSecret.vobjX.size() - 1 ) * 3 ));
	// file.ByteEx(BANK_OFFSET(nBankMuSecret) + 0xB47C, m_nGame.nMuSecret.bObjId);
	// file.ByteEx(BANK_OFFSET(nBankMuSecret) + 0xB453, m_nGame.nMuSecret.bObjId);

	file[nfMuSecretPosY] = m_nGame.nMuSecret.bPosY;
	file[nfMuSecretPosY2] = m_nGame.nMuSecret.bPosY + 1;

	// file.ByteEx(BANK_OFFSET(nBankMuSecret) + 0xB486, m_nGame.nMuSecret.bPosY);
	// file.ByteEx(BANK_OFFSET(nBankMuSecret) + 0xB45A, m_nGame.nMuSecret.bPosY);

	file.Word(nfMuSecretDataReaderPtr1, ptr + 0); // WordEx(BANK_OFFSET(nBankMuSecret) + 0xB48F, ptr + 0);
	file.Word(nfMuSecretDataReaderPtr2, ptr + 1); // WordEx(BANK_OFFSET(nBankMuSecret) + 0xB497, ptr + 1);
	file.Word(nfMuSecretDataReaderPtr3, ptr + 2); // WordEx(BANK_OFFSET(nBankMuSecret) + 0xB49E, ptr + 2);

	file.Word(nfMuSecretDataReader2Ptr1, ptr + 0); // WordEx(BANK_OFFSET(nBankMuSecret) + 0xB45F, ptr + 0);
	file.Word(nfMuSecretDataReader2Ptr2, ptr + 1); // WordEx(BANK_OFFSET(nBankMuSecret) + 0xB465, ptr + 1);

	if ( !m_nGame.nMuSecret.fVertical )
	{
		file.Word(nfMuSecretPosType, 0x4DE); // WordEx(BANK_OFFSET(nBankMuSecret) + 0xB483, 0x4DE);
		file.Word(nfMuSecretPosType2, 0x4AE); // WordEx(BANK_OFFSET(nBankMuSecret) + 0xB48C, 0x4AE);
		file.Word(nfMuSecretPosType3, 0x4BE); // WordEx(BANK_OFFSET(nBankMuSecret) + 0xB494, 0x4BE);

		file.Word(nfMuSecretPos2Type1, 0x4DE); // WordEx(BANK_OFFSET(nBankMuSecret) + 0xB45C, 0x4DE);
		file.Word(nfMuSecretPos2Type2, 0x4AE); // WordEx(BANK_OFFSET(nBankMuSecret) + 0xB462, 0x4AE);
		file.Word(nfMuSecretPos2Type3, 0x4BE); // WordEx(BANK_OFFSET(nBankMuSecret) + 0xB468, 0x4BE);
	}
	else
	{
		file.Word(nfMuSecretPosType, 0x4AE); // WordEx(BANK_OFFSET(nBankMuSecret) + 0xB483, 0x4AE);
		file.Word(nfMuSecretPosType2, 0x4DE); // WordEx(BANK_OFFSET(nBankMuSecret) + 0xB48C, 0x4DE);
		file.Word(nfMuSecretPosType3, 0x4EE); // WordEx(BANK_OFFSET(nBankMuSecret) + 0xB494, 0x4EE);

		file.Word(nfMuSecretPos2Type1, 0x4AE); // WordEx(BANK_OFFSET(nBankMuSecret) + 0xB45C, 0x4AE);
		file.Word(nfMuSecretPos2Type2, 0x4DE); // WordEx(BANK_OFFSET(nBankMuSecret) + 0xB462, 0x4DE);
		file.Word(nfMuSecretPos2Type3, 0x4EE); // WordEx(BANK_OFFSET(nBankMuSecret) + 0xB468, 0x4EE);
	}

	BYTE bMask = 0;
	size_t count = m_nGame.nMuSecret.vobjX.size();
	for(size_t n = 0; n < count; ++n)
	{
		NES_DOT & nd = m_nGame.nMuSecret.vobjX[n];
		BYTE bData[3] = { nd.bPos, nd.bScr, ( 1 << n ) };
		file.PutDataEx(BANK_OFFSET(nBankMuSecret) + sizeof(bData) * n + int(ptr), bData, sizeof(bData));
		bMask = (( bMask << 1 ) | 1);
	}

	file[nfMuSecretMask] = bMask; // .ByteEx(BANK_OFFSET(nBankMuSecret) + 0xB4AA, bMask);

	return TRUE;
}

BOOL CNesEditor::LoadEgyptSecretData(CNesFile & file)
{
	if ( !file.IsFileLoaded() )
		return FALSE;

	file.GetData(nfEgyptCode, m_nGame.bEgyptCode, sizeof(m_nGame.bEgyptCode));
	// Ex(BANK_OFFSET(6) + 0xA54A, m_nGame.bEgyptCode, sizeof(m_nGame.bEgyptCode));

	return TRUE;
}

BOOL CNesEditor::StorEgyptSecretData(CNesFile & file)
{
	if ( !file.IsFileLoaded() || !m_fLoaded )
		return FALSE;

//	file.PutDataEx(BANK_OFFSET(6) + 0xA54A, m_nGame.bEgyptCode, sizeof(m_nGame.bEgyptCode));
	file.PutData(nfEgyptCode, m_nGame.bEgyptCode, sizeof(m_nGame.bEgyptCode));

	return TRUE;
}

BOOL CNesEditor::LoadBridgeData(CNesFile & file)
{
	if ( !file.IsFileLoaded() )
		return FALSE;

	NES_BRIDGE & nb = m_nBank.nBridgeData;

	if ( file.IsExtended() )
		nb.ptLeft.bScrY = file[nfBridgeLeftScrY]; // ByteEx(BANK_OFFSET(nBankBridge) + 0x9BA3);
	else
		nb.ptLeft.bScrY = 0;

	nb.ptLeft.bPosY = file[nfBridgeLeftY]; // .ByteEx(BANK_OFFSET(nBankBridge) + 0xA027);
	nb.ptLeft.bScrX = file[nfBridgeLeftScrX]; // .ByteEx(BANK_OFFSET(nBankBridge) + 0xA00B);
	nb.ptLeft.bPosX = file[nfBridgeLeftX]; // .ByteEx(BANK_OFFSET(nBankBridge) + 0xA011);
	nb.ptRight.bScrX = file[nfBridgeRightScrX]; // file.ByteEx(BANK_OFFSET(nBankBridge) + 0xA017);
	nb.ptRight.bPosX = file[nfBridgeRightX]; // file.ByteEx(BANK_OFFSET(nBankBridge) + 0xA01D);

	nb.bBgTile = file[nfBridgeBgTile]; // .ByteEx(BANK_OFFSET(nBankBridge) + 0xA034);
	nb.bObjectId = file[nfBridgeObjectId]; // .ByteEx(BANK_OFFSET(nBankBridge) + 0xA053);

	return TRUE;
}

BOOL CNesEditor::StorBridgeData(CNesFile & file)
{
	if ( !file.IsFileLoaded() )
		return FALSE;

	NES_BRIDGE & nb = m_nBank.nBridgeData;

	if ( file.IsExtended() )
	{
		file[nfBridgeLeftScrY] = nb.ptLeft.bScrY; // .ByteEx(BANK_OFFSET(nBankBridge) + 0x9BA3, nb.ptLeft.bScrY);
		file[nfBridgeLeftScrY2] = nb.ptLeft.bScrY; // file.ByteEx(BANK_OFFSET(nBankBridge) + 0xA02B, nb.ptLeft.bScrY);
	}

	file[nfBridgeLeftY] = nb.ptLeft.bPosY; // .ByteEx(BANK_OFFSET(nBankBridge) + 0xA027, nb.ptLeft.bPosY);
	file[nfBridgeLeftScrX] = nb.ptLeft.bScrX; // .ByteEx(BANK_OFFSET(nBankBridge) + 0xA00B, nb.ptLeft.bScrX);
	file[nfBridgeLeftX] = nb.ptLeft.bPosX; // .ByteEx(BANK_OFFSET(nBankBridge) + 0xA011, nb.ptLeft.bPosX);
	
	file[nfBridgeRightScrX] = nb.ptRight.bScrX; // .ByteEx(BANK_OFFSET(nBankBridge) + 0xA017, nb.ptRight.bScrX);
	file[nfBridgeRightX] = nb.ptRight.bPosX; // .ByteEx(BANK_OFFSET(nBankBridge) + 0xA01D, nb.ptRight.bPosX);

	file[nfBridgeBgTile] = nb.bBgTile; // .ByteEx(BANK_OFFSET(nBankBridge) + 0xA034, nb.bBgTile);
	file[nfBridgeObjectId] = nb.bObjectId; // .ByteEx(BANK_OFFSET(nBankBridge) + 0xA053, nb.bObjectId);

	return TRUE;
}
