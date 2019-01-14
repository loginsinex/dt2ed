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

BOOL CNesEditor::SetExtendedNes()
{
	if ( !m_pFile->SetFileExtended() )
		return FALSE;

	m_nGame.fExtended = TRUE;

	INT nBankId = (*m_pFile)[NES_OBJECTS_BANKID_PTR];
	INT nBankOffset = 0x4000 * nBankId + 0x10 - 0x8000;

	INT nOffsetPtr = m_pFile->Word(NES_OBJECTS_PTR_PTR) + nBankOffset;
	
	const INT nNewOffset = 0x1C010;
	const INT nNewRamPtr = 0x8000;
	const INT nOffsetDelta = m_pFile->Word(NES_OBJECTS_PTR_PTR) - nNewRamPtr;
	const INT nNewBankId = 0x07;
	const INT nNewBankOffset = 0x4000 * nNewBankId + 0x10 - 0x8000;
	
	INT nBankRoomsId = (*m_pFile)[0x1C936];
	INT nBankRoomsOffset = 0x4000 * nBankRoomsId + 0x10 - 0x8000;
	const INT nNewRoomsBankId = 0x08;
	const INT nNewRoomsBankOffset = 0x4000 * nNewRoomsBankId + 0x10 - 0x8000;

	INT nBankBlocksId = (*m_pFile)[0x1C9BB];
	INT nBankBlocksOffset = 0x4000 * nBankBlocksId + 0x10 - 0x8000;
	const INT nNewBlocksBankId = 0x0A;
	const INT nNewBlocksBankOffset = 0x4000 * nNewBlocksBankId + 0x10 - 0x8000;
	const INT nNewBlocksDelta = 0x8D3A - 0x8000;		// we're moving blocks to begin of 0x0A bank

	// leave space 0x30 bytes amount for editor specific data
	INT nBankSprTilesId = (*m_pFile)[0x1F53B];
	const INT nBankSprTilesOffset = m_pFile->Word(0x1F540);	// WARNING! RAM ptr used!
	const INT nNewBankSprTilesId = (*m_pFile)[nfGfxPtrsBank];
	const INT nNewBankSprTilesOffset = m_pFile->Word(nfGfxPtrs);

	m_pFile->Byte(NES_OBJECTS_BANKID_PTR, nNewBankId);
	m_pFile->Word(NES_OBJECTS_PTR_PTR, nNewRamPtr);
	m_pFile->Word(NES_OBJECTS_PTR_PTR + 0x03, nNewRamPtr + 1);

	// fix bank id's in code for rooms
	m_pFile->Byte(0x1C936, nNewRoomsBankId);
	m_pFile->Byte(0x1C985, nNewRoomsBankId);
	m_pFile->Byte(0x1CA86, nNewRoomsBankId);
	m_pFile->Byte(0x1CADE, nNewRoomsBankId);
	m_pFile->Byte(0x1CAF3, nNewRoomsBankId + 1);
	m_pFile->Byte(0x1EAD0, nNewRoomsBankId);
	
	// fix bank id's in code for blocks
	m_pFile->Byte(0x1C9BB, nNewBlocksBankId);
	m_pFile->Byte(0x1CB12, nNewBlocksBankId);

	// fix offset before rooms data (After layouts)
	m_pFile->ByteEx(0x20010, 0xF0);

	// fix bank id in code for sprites tiles
	// m_pFile->Byte(0x1F53B, 0x0B);
	// (*m_pFile)[nfGfxPtrsBank] = nNewBankSprTilesId; - space after pointers will freed, so use it for duplicated array
	// (*m_pFile)[nfGfxBank] = nNewBankSprTilesId; - dont move gfx data, because bank_id in $cfdd proc used in many places!

	
	// because, level uses only first 0x10 bytes in array,
	// array chooser proc uses ORA instruction with ( level << 8 ) value.
	// now we're will use more than 0x10 offset,
	// so replace ORA instruction with ADC
	m_pFile->Byte(0x1F537, 0x65);	// 0x65 - code of ADC instruction. Was 0x05 - ORA

	// fix offset for sprites tiles ptrs array
	// m_pFile->Word(0x1F540, nNewBankSprTilesOffset);
	// m_pFile->Word(0x1F545, nNewBankSprTilesOffset + 1);

	// fix bank switcher
	m_pFile->Word(0x1D3A9, 0xFEF0);
	for(BYTE bank = 0; bank < 16; ++bank)
		m_pFile->Byte(0x1FF00 + INT(bank), bank);


	std::map<INT, INT> eraser_rooms;
	std::map<INT, INT> eraser_obj;
	std::map<INT, INT> eraser_blocks;
	std::map<INT, INT> eraser_sprptr;
	std::map<INT, BOOL> levels_saved;

	for(INT level = 0; level < 7; ++level)
	{
		// STEP #1: Move objects
		for(INT line = 0; line < 6; ++line)
		{
			INT data_ptr = m_pFile->Word(nOffsetPtr + 2 * ( 6 * level + line ));
			m_pFile->WordEx(nNewOffset + 2 * ( 6 * level + line ), data_ptr - nOffsetDelta);

			INT copy_from = data_ptr + nBankOffset;
			INT copy_to = data_ptr - nOffsetDelta + nNewBankOffset;
			
			INT nChar = 1;
			BYTE bChar = 0;
			m_pFile->ByteEx(copy_to, 0xFF);
			while(0xFF != (bChar = m_pFile->ByteEx(copy_from + nChar)) )
			{
				m_pFile->ByteEx(copy_to + nChar, bChar);
				nChar++;
			}
			eraser_obj[copy_from] = nChar;

			m_pFile->ByteEx(copy_to + nChar, bChar);
		}

		// STEP #2: Move rooms
		NES_LEVEL_HEADER lh = { 0 };
		NES_LEVEL_BLOCKS lb = { 0 };
		BYTE lo[48] = { 0 };
		BYTE level_offset = (*m_pFile)[NES_LEVELS_LIST + level];
		m_pFile->GetData(NES_LEVELS_PTRS + level_offset, &lh, sizeof(lh));
		m_pFile->GetData(0x8010 + lh.ptrBlocks, &lb, sizeof(lb));
		
		if ( levels_saved.find(lh.ptrBlocks) == levels_saved.end() )
		{
			levels_saved[lh.ptrBlocks] = TRUE;

			m_pFile->GetData(nBankRoomsOffset + lb.ptrLayout, &lo, sizeof(lo));
			eraser_rooms[0x8010 + lb.ptrLayout] = sizeof(lo);


			lb.ptrLayout -= ( NES_ROOMS_DEFAULT_PTR - NES_ROOMS_UOROM_PTR );
			m_pFile->PutData(0x8010 + lh.ptrBlocks, &lb, sizeof(lb));
			m_pFile->PutDataEx(lb.ptrLayout + nNewRoomsBankOffset, &lo, sizeof(lo));
			
			for(INT room = 0; room < 48; ++room)
			{
				if ( lo[room] )
				{
					BYTE roomdata[64] = { 0 };
					m_pFile->GetData( NES_ROOMS_DEFAULT_PTR + 6 * 48 + 64 * (lo[room] - 1) + nBankRoomsOffset,
						roomdata, sizeof(roomdata));

					m_pFile->PutDataEx( NES_ROOMS_UOROM_PTR + 6 * 48 + 64 * (lo[room] - 1) + nNewRoomsBankOffset,
						roomdata, sizeof(roomdata));

					eraser_rooms[NES_ROOMS_DEFAULT_PTR + 6 * 48 + 64 * (lo[room] - 1) + nBankRoomsOffset] = sizeof(roomdata);
				}
			}

			// STEP #3: Move blocks
			std::vector<BYTE> bl[4];
			size_t size = lb.ptrMajorBlocks2 - lb.ptrMajorBlocks1;
			bl[0].insert(bl[0].begin(), size, 0);
			bl[1].insert(bl[1].begin(), size, 0);
			bl[2].insert(bl[2].begin(), size, 0);
			bl[3].insert(bl[3].begin(), size, 0);

			m_pFile->GetDataEx(lb.ptrMajorBlocks1 + nBankBlocksOffset, &bl[0][0], size);
			m_pFile->GetDataEx(lb.ptrMajorBlocks2 + nBankBlocksOffset, &bl[1][0], size);
			m_pFile->GetDataEx(lb.ptrMajorBlocks3 + nBankBlocksOffset, &bl[2][0], size);
			m_pFile->GetDataEx(lb.ptrMajorBlocks4 + nBankBlocksOffset, &bl[3][0], size);
			eraser_blocks[lb.ptrMajorBlocks1 + nBankBlocksOffset] = size;
			eraser_blocks[lb.ptrMajorBlocks2 + nBankBlocksOffset] = size;
			eraser_blocks[lb.ptrMajorBlocks3 + nBankBlocksOffset] = size;
			eraser_blocks[lb.ptrMajorBlocks4 + nBankBlocksOffset] = size;

			lb.ptrMajorBlocks1 -= LOWORD(nNewBlocksDelta);
			lb.ptrMajorBlocks2 -= LOWORD(nNewBlocksDelta);
			lb.ptrMajorBlocks3 -= LOWORD(nNewBlocksDelta);
			lb.ptrMajorBlocks4 -= LOWORD(nNewBlocksDelta);

			m_pFile->PutDataEx(lb.ptrMajorBlocks1 + nNewBlocksBankOffset, &bl[0][0], size);
			m_pFile->PutDataEx(lb.ptrMajorBlocks2 + nNewBlocksBankOffset, &bl[1][0], size);
			m_pFile->PutDataEx(lb.ptrMajorBlocks3 + nNewBlocksBankOffset, &bl[2][0], size);
			m_pFile->PutDataEx(lb.ptrMajorBlocks4 + nNewBlocksBankOffset, &bl[3][0], size);

			m_pFile->PutData(0x8010 + lh.ptrBlocks, &lb, sizeof(lb));
		}

		// STEP #4: Move sprites tiles pointers arrays
		// CNesGraphics will write valid pointers here
		WORD wptrs[8] = { 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000 };
		m_pFile->PutDataEx(BANK_OFFSET(nNewBankSprTilesId) + nNewBankSprTilesOffset
				+ sizeof(wptrs) * level, &wptrs, sizeof(wptrs));

		// eraser_sprptr[BANK_OFFSET(nBankSprTilesId) + nBankSprTilesOffset
		//		+ sizeof(wptrs) * level] = sizeof(wptrs);

	}

	Erase(eraser_rooms);
	Erase(eraser_obj);
	Erase(eraser_blocks);
	Erase(eraser_sprptr);

	MakeBridgePatch(*m_pFile);
	MakeNoScrollPatch(*m_pFile);

	MakeFasterSpritesPatch(*m_pFile);

	return MakeSlotsPatch(*m_pFile);
}

BOOL CNesEditor::MakeBridgePatch(CNesFile & file)
{
	if ( !file.IsExtended() )
		return FALSE;

	// small hack for bridge
	BYTE bBridgeCode[] = { 
		0xA9, 0x00,				// lda #00
		0xC5, 0x1B,				// cmp $1b
		0x4C, 0xE5, 0x9F		// jmp $9fe5 - original code
		};

	WORD & pOrigCode = m_nBank.nObjHandlers[0];
	pOrigCode = 0x9BA2;	// for level #01

	for(int i = 1; i < NES_COUNT_LEVELS; ++i)
		if ( m_nBank.nObjHandlers[i] == pOrigCode )
			m_nBank.nObjHandlers[i] = pOrigCode;

	file.PutDataEx(BANK_OFFSET(6) + pOrigCode, bBridgeCode, sizeof(bBridgeCode));

	return TRUE;
}


BOOL CNesEditor::MakeSlotsPatch(CNesFile & file)
{
	if ( !file.IsExtended() )
		return FALSE;

	file.PutPatch(nfHiddensSlotsPatch);
	file.PutPatch(nfHiddensSlotsPatch2);
	file.PutPatch(nfHiddensSlotsPatch3);

	const BYTE nSlotsBank = file.Bank(nfHiddensSlotsPtrsEx);
	const INT ptrSteps = file.Word(nfHiddensSlotsPtrsArray);
	const WORD ptrStepsNewPtr = file.Ptr(nfHiddensSlotsPtrsEx);

	WORD wptrs[NES_COUNT_LEVELS][8] = { 0 }, wptr = ptrStepsNewPtr;
	
	for(int i = 0; i < 7; ++i)
	{
		wptrs[0][i] = wptr;
		wptr += m_nGame.nSteps[0][i].size() * sizeof(NES_POINT) + 2;
	}

	for(int i = 1; i < NES_COUNT_LEVELS; ++i)
	{
		CopyMemory(wptrs[i], wptrs[0], sizeof(wptrs[0]));
		
		for(int n = 0; n < 7; ++n)
			m_nGame.nSteps[i][n] = m_nGame.nSteps[0][n];
	}

	file.PutDataEx(BANK_OFFSET(nSlotsBank) + INT(ptrSteps), wptrs, sizeof(wptrs));

	return TRUE;
}

BOOL CNesEditor::MakeSlotsPatchEx(CNesFile & file)
{
	if ( !file.IsExtended() )
		return FALSE;
	
	const WORD ptrRPC				= 0xFF10;
	const BYTE nOldSlotsBank		= 0x06;
	const WORD nOldSlotsPtrs		= 0xA8C2;
	const WORD nOldSlotsProcPtr		= 0xA889;
	const BYTE nNewSlotsBank		= 0x0B;
	const WORD nNewSlotsProcPtr		= 0x8130;
	const WORD nNewSlotsPtr			= 0x8190;
	
	// get pointers
	WORD wptrs[7] = { 0 }, wFirstPtr = 0xFFFF, wptrsnew[7] = { 0 };
	file.GetDataEx(BANK_OFFSET(nOldSlotsBank) + INT(nOldSlotsPtrs), wptrs, sizeof(wptrs));
	for(int i = 0; i < sizeof(wptrs) / sizeof(wptrs[0]); ++i)
		wFirstPtr = min(wFirstPtr, wptrs[i]);

	wFirstPtr -= nNewSlotsPtr;
	for(int i = 0; i < sizeof(wptrsnew) / sizeof(wptrsnew[0]); ++i)
		wptrsnew[i] = wptrs[i] - wFirstPtr;

	// RPC patch
	BYTE bRPC[] = {	0xA5, 0x12, 0x48, 0xA9, nNewSlotsBank, 0x20, 0x91, 0xD3,
					0x20, LOBYTE(nNewSlotsProcPtr), HIBYTE(nNewSlotsProcPtr), 
					0x68, 0x20, 0x91, 0xD3, 0x60 };

	file.PutData(0x10010 + INT(ptrRPC), bRPC, sizeof(bRPC));

	BYTE bOrigProcPatch[] = {
		0x4C, LOBYTE(ptrRPC), HIBYTE(ptrRPC), 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 
		0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 
		0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 
		0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA };

	file.PutDataEx(BANK_OFFSET(nOldSlotsBank) + INT(nOldSlotsProcPtr), bOrigProcPatch, sizeof(bOrigProcPatch));

	// slots opener procedure
	BYTE bOrigProc[] = { 
		0xA0, 0x00, 0xB1, 0xB2, 0x8D, 0x0A, 0x00, 0xC8, 0xB1, 0xB2, 0x8D, 0x0B, 0x00, 0x0D, 0x0A, 0x00, 
		0xF0, 0x34, 0x38, 0xAD, 0x0A, 0x00, 0xED, 0x17, 0x00, 0xAD, 0x0B, 0x00, 0xED, 0x18, 0x00, 0xD0, 
		0x25, 0xC8, 0xB1, 0xB2, 0x8D, 0x0C, 0x00, 0xC8, 0xB1, 0xB2, 0x85, 0x0D, 0xA0, 0x89, 0x20, 0x48, 
		0xF3, 0xB0, 0x13, 0x18, 0xAD, 0xB2, 0x00, 0x69, 0x04, 0x8D, 0xB2, 0x00, 0xAD, 0xB3, 0x00, 0x69, 
		0x00, 0x8D, 0xB3, 0x00, 0x09, 0xFF, 0x60 };

	file.PutDataEx(BANK_OFFSET(nNewSlotsBank) + 0x8130, bOrigProc, sizeof(bOrigProc));

	for(int i = 0; i < sizeof(wptrs) / sizeof(wptrs[0]); ++i)
	{
		WORD ptr = wptrs[i], newptr = wptrsnew[i];

		while(0 != file.WordEx(BANK_OFFSET(nOldSlotsBank) + INT(ptr)))
		{
			NES_POINT pt = { 0 };
			file.GetDataEx(BANK_OFFSET(nOldSlotsBank) + INT(ptr), &pt, sizeof(pt));
			file.PutDataEx(BANK_OFFSET(nNewSlotsBank) + INT(newptr), &pt, sizeof(pt));
			ptr += sizeof(pt);
			newptr += sizeof(pt);
		}

		file.WordEx(BANK_OFFSET(nNewSlotsBank) + INT(newptr), 0);
	}

	file.PutDataEx(BANK_OFFSET(nOldSlotsBank) + INT(nOldSlotsPtrs), wptrsnew, sizeof(wptrsnew));

	// DONE

	return TRUE;
}

BOOL CNesEditor::MakeNoScrollPatch(CNesFile & file)
{
	if ( !file.IsExtended() )
		return FALSE;

	const WORD ptrCheckProcAddr = 0xFF10;
	WORD ptrFadeScreenProcAddr = 0;
	WORD ptrFadeoutProcAddr = 0;

	BYTE bCheckProc[] = {
		0xA5, 0x1C, 0xD0, 0x4A, 0xA5, 0x18, 0x29, 0x01, 0xF0, 0x44, 0x60
	};
	file.PutData(0x10010 + INT(ptrCheckProcAddr), bCheckProc, sizeof(bCheckProc));

	ptrFadeScreenProcAddr = ptrCheckProcAddr + sizeof(bCheckProc);
	BYTE bFadeScreenProc[] = {
		0x20, LOBYTE(ptrCheckProcAddr), HIBYTE(ptrCheckProcAddr), 0xD0, 0x3E, 0xA5, 0x10, 0x29, 0x7F, 
		0x85, 0x11, 0x8D, 0x00, 0x20, 0xA9, 0x00, 0x8D, 0x01, 0x20, 0xA5, 0x17, 0x85, 0x0A, 0xA5, 0x18, 
		0x85, 0x0B, 0xA9, 0x00, 0x85, 0x0C, 0xA5, 0x1B, 0x85, 0x0D, 0x20, 0xE4, 0xCD, 0xA0, 0x1F, 0xB9, 
		0xA0, 0x01, 0x99, 0x80, 0x01, 0x88, 0x10, 0xF7, 0xA5, 0x10, 0x09, 0x80, 0x85, 0x10, 0x8D, 0x00, 
		0x20, 0xA5, 0x11, 0x09, 0x1E, 0x85, 0x11, 0x8D, 0x01, 0x20, 0xA9, 0x00, 0x60
	};
	file.PutData(0x10010 + INT(ptrFadeScreenProcAddr), bFadeScreenProc, sizeof(bFadeScreenProc));

	ptrFadeoutProcAddr = ptrFadeScreenProcAddr + sizeof(bFadeScreenProc);
	BYTE bFadeoutProc[] = {
		0x20, LOBYTE(ptrCheckProcAddr), HIBYTE(ptrCheckProcAddr), 0xF0, 0x03, 0x4C, 0x48, 
		0xCC, 0xA0, 0x1F, 0xA9, 0x0F, 0x99, 0x80, 0x01, 0x88, 0x10, 0xFA, 0x60
	};
	file.PutData(0x10010 + INT(ptrFadeoutProcAddr), bFadeoutProc, sizeof(bFadeoutProc));
	
	file.Word(0x10010 + 0xED63, ptrFadeoutProcAddr);
	file.Byte(0x10010 + 0xEDCA, 0x4C);	// JSR -> JMP
	file.Word(0x10010 + 0xEDCB, ptrFadeScreenProcAddr);
	file.Byte(0x10010 + 0xED6F, 0x70);	// jmp to RTS

	return TRUE;
}

BOOL CNesEditor::MakeFasterSpritesPatch(CNesFile & file)
{
	BOOL fPatched = 
		file.PutPatch(nfFastSpritesProc1);

	fPatched &= file.PutPatch(nfFastSpritesProc2);
	fPatched &= file.PutPatch(nfFastSpritesProc3);
	fPatched &= file.PutPatch(nfFastSpritesProc4);
	fPatched &= file.PutPatch(nfFastSpritesProc5);
	fPatched &= file.PutPatch(nfFastSpritesProc6);
	fPatched &= file.PutPatch(nfFastSpritesProc7);
	fPatched &= file.PutPatch(nfFastSpritesProc8);
	fPatched &= file.PutPatch(nfFastSpritesProc9);
	fPatched &= file.PutPatch(nfFastSpritesProc10);
	return fPatched;
}

VOID CNesEditor::Erase(std::map<INT, INT> & eraser)
{
	for(std::map<INT, INT>::iterator e = eraser.begin(); e != eraser.end(); e++)
	{
		std::vector<BYTE> nl;
		nl.insert(nl.begin(), e->second, 0);
		m_pFile->PutDataEx( e->first, &nl[0], e->second);
	}
}
