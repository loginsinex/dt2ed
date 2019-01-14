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

const BYTE m_mTileBases[] = {
						0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB4, 0xB0, 0x9D, 0xE6, 0x00, 0x00, 0xD0, 0xE6, 0xD0,
						0x00, 0xC0, 0xD0, 0xD8, 0xC0, 0xA6, 0xC0, 0xDB, 0xE4, 0xF0, 0xC0, 0xD0,0xC0, 0xE0, 0xB8, 0xC0,
						0xC0, 0xD2, 0xE0, 0xBA, 0xF0, 0x00, 0xC0, 0xC0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD0, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0xA3, 0xA4, 0xA0, 0xAC, 0xA0,
						0xA3, 0xA4, 0xAC, 0xA0, 0xAB, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00,
						0x00, 0x00 };

VOID l_FillArray(std::map<INT, std::vector<DWORD>> & m, int setid, ...)
{
	std::vector<DWORD> & vArr = m[setid];

	va_list argptr;
	va_start(argptr, setid);
	INT * idList = (INT*) argptr;

	for(INT i = 0; 0xffff != idList[i]; ++i)
		vArr.push_back(idList[i]);

	va_end(argptr);
}

CNesComparator::CNesComparator(BYTE bStart, BYTE bLength)
{
	ZeroMemory(m_dwPPUMask, sizeof(m_dwPPUMask));

	int start = bStart, end = min(start + int(bLength), 256);
	for(int i = start; i < end; ++i)
		m_dwPPUMask[i / 32] |= ( 1 << ( i % 32 ) );
}

CNesComparator::CNesComparator(std::vector<NES_GFX_OBJECT> & vgfx, std::vector<std::vector<BYTE>> vTileStream)
{
	ZeroMemory(m_dwPPUMask, sizeof(m_dwPPUMask));

	size_t count = vgfx.size();
	for(size_t n = 0; n < count; ++n)
	{
		NES_GFX_OBJECT & spr = vgfx[n];
		FillArea(spr.bPPUAddr, vTileStream[spr.bSetId / 2].size() / 0x10);
	}
}

VOID CNesComparator::FillArea(BYTE bStart, BYTE bLength)
{
	int start = bStart, end = min(start + int(bLength), 256);
	for(int i = start; i < end; ++i)
		m_dwPPUMask[i / 32] |= ( 1 << ( i % 32 ) );
}

BOOL CNesComparator::And(CNesComparator & nc)
{
	for(int i = 0; i < sizeof(m_dwPPUMask) / sizeof(m_dwPPUMask[0]); ++i)
	{
		if ( ( m_dwPPUMask[i] & nc.m_dwPPUMask[i] ) != 0 )
			return TRUE;
	}

	return FALSE;
}

// ------------------------

CNesGraphics::CNesGraphics()
	: m_fLoaded( FALSE )
{
    l_FillArray(m_mCfgExt, 0x00,    0xffff);
	l_FillArray(m_mCfgExt, 0x01,    0x0155, 0x0156, 0x0157, 0xffff);

    l_FillArray(m_mCfgExt, 0x02,    0xffff);
    l_FillArray(m_mCfgExt, 0x03,    0xffff);
    l_FillArray(m_mCfgExt, 0x04,    0xffff);
    l_FillArray(m_mCfgExt, 0x05,    0xffff);
    l_FillArray(m_mCfgExt, 0x06,    0xffff);
	
	l_FillArray(m_mCfgExt, 0x07,    0x025d, 0x025e, 0x025f, 0x0260,
									0x0261, 0x0262, 0x0263, 0x0264,
									0x0265, 0x0266, 0x0267, 0x0268,
									0xffff);

    l_FillArray(m_mCfgExt, 0x08,    0x0132, 0x0133, 0x0134, 0x0135,
									0x0136, 0x0137, 0x0138, 0x0139,
									0x013a, 0x013b, 0x013c, 0x013d,
									0x013e, 0x013f, 0x0140, 0x0141,
									0x0142, 0x0143, 0x0144, 0x0145,
									0x0146, 0x0147, 0x0148, 0x0149,
									0x014a, 0x014b, 0x014c, 0x014d,
									0x014e, 0x014f, 0x0150, 0x0151,
									0x0152, 0x0153, 0x0154, 0xffff);

    l_FillArray(m_mCfgExt, 0x09,    0x0112, 0x0256, 0x0257, 0x0258, 
									0x0271,	0x0272, 0xffff);

    l_FillArray(m_mCfgExt, 0x0a,    0x025a, 0x025b, 0x025c, 0x0271,
				                    0x0272, 0xffff);

    l_FillArray(m_mCfgExt, 0x0b,    0xffff);
    l_FillArray(m_mCfgExt, 0x0c,    0xffff);

    l_FillArray(m_mCfgExt, 0x0d,    0x0209, 0x020a, 0x020b, 0xffff);
    l_FillArray(m_mCfgExt, 0x0e,    0x020c, 0x020d, 0x020e, 0x021a,
				                    0xffff);

    l_FillArray(m_mCfgExt, 0x0f,    0x0203, 0x0204, 0x0205, 0x0206,
				                    0x0207, 0x0208, 0x0234, 0xffff);

    l_FillArray(m_mCfgExt, 0x10,    0xffff);
	
	l_FillArray(m_mCfgExt, 0x11,    0x0221, 0x0222, 0x0223, 0xffff);
    l_FillArray(m_mCfgExt, 0x12,    0x021b, 0x021c, 0xffff);
    l_FillArray(m_mCfgExt, 0x13,    0x020f, 0x0210, 0x0211, 0x0212,
									0x0213, 0x0214, 0x0215, 0x0216,
									0x0217, 0x0218, 0x0219, 0xffff);

    l_FillArray(m_mCfgExt, 0x14,    0x021d, 0x021e, 0x021f, 0x0220,
				                    0xffff);

    l_FillArray(m_mCfgExt, 0x15,    0x0152, 0x0153, 0xffff);
    l_FillArray(m_mCfgExt, 0x16,    0x0231, 0x0232, 0x0233, 0x0235,
				                    0xffff);

    l_FillArray(m_mCfgExt, 0x17,    0x0268, 0x0269, 0x026a,
				                    0x026b, 0xffff);

    l_FillArray(m_mCfgExt, 0x18,    0x022e, 0x022f, 0x0230, 0xffff);
    l_FillArray(m_mCfgExt, 0x19,    0x022c, 0x022d, 0xffff);
    l_FillArray(m_mCfgExt, 0x1a,    0x0242, 0xffff);
    l_FillArray(m_mCfgExt, 0x1b,    0x023d, 0x023e, 0x023f, 0x0240,
									0x0241, 0xffff);

    l_FillArray(m_mCfgExt, 0x1c,    0x0250, 0x0251, 0x0252, 0x0253,
									0x0254, 0x0255, 0xffff);

    l_FillArray(m_mCfgExt, 0x1d,    0x024c, 0x024d, 0x024e, 0x024f,
									0xffff);

    l_FillArray(m_mCfgExt, 0x1e,    0x0243, 0x0244, 0x0245, 0x0246,
									0x0247, 0x0248, 0x0249, 0x024a,
									0x024b, 0xffff);

    l_FillArray(m_mCfgExt, 0x1f,    0x0236, 0x0237, 0x0238, 0x0239,
									0x023a, 0x023b, 0x023c, 0xffff);

    l_FillArray(m_mCfgExt, 0x20,    0x0059, 0x0062, 0x0055, 0x0056,
									0x0057, 0x0058, 0x0063, 0x0064,
									0x0077, 0x0078, 0x0079, 0x007a,
									0x006c, 0x005c, 0xffff);

    l_FillArray(m_mCfgExt, 0x21,    0x0065, 0x0066, 0x0067, 0x0068,
									0x0069, 0x011a, 0x0053, 0x0070,
									0xffff);

    l_FillArray(m_mCfgExt, 0x22,    0x005a, 0x005b, 0x005d, 0x005e,
									0x005f, 0x0060, 0x0061, 0x005c,
									0x0053, 0x0055, 0x0056, 0x0057,
									0x0058, 0x0059, 0x0063, 0x0064,
									0x006c, 0x0070, 0x0077, 0x0078,
									0x0079, 0x007a, 0xffff);

    l_FillArray(m_mCfgExt, 0x23,    0x0224, 0x0225, 0x0226, 0x0227,
									0x0228, 0x0229, 0x022a, 0x022b,
									0xffff);

    l_FillArray(m_mCfgExt, 0x24,    0x004a, 0x004b, 0x004c, 0x004d,
									0xffff);

    l_FillArray(m_mCfgExt, 0x25,    0xffff);

	l_FillArray(m_mCfgExt, 0x26,    0x0032, 0x0033, 0x0034, 0xffff);
    l_FillArray(m_mCfgExt, 0x27,    0x0035, 0xffff);
    l_FillArray(m_mCfgExt, 0x28,    0x0036, 0xffff);

    l_FillArray(m_mCfgExt, 0x29,    0xffff);
    l_FillArray(m_mCfgExt, 0x2a,    0xffff);
    l_FillArray(m_mCfgExt, 0x2b,    0xffff);
    l_FillArray(m_mCfgExt, 0x2c,    0xffff);
    l_FillArray(m_mCfgExt, 0x2d,    0xffff);
	
	l_FillArray(m_mCfgExt, 0x2e,    0x0117, 0x0118, 0x0119, 0x011a,
									0xffff);

    l_FillArray(m_mCfgExt, 0x30,    0xffff);
    l_FillArray(m_mCfgExt, 0x31,    0xffff);
    l_FillArray(m_mCfgExt, 0x32,    0xffff);
    l_FillArray(m_mCfgExt, 0x33,    0xffff);
    l_FillArray(m_mCfgExt, 0x34,    0xffff);
    l_FillArray(m_mCfgExt, 0x35,    0xffff);

    l_FillArray(m_mCfgExt, 0x36,    0x0025, 0x0026, 0xffff);

    l_FillArray(m_mCfgExt, 0x37,    0xffff);
    l_FillArray(m_mCfgExt, 0x38,    0xffff);
    l_FillArray(m_mCfgExt, 0x39,    0xffff);
    l_FillArray(m_mCfgExt, 0x3a,    0xffff);
    l_FillArray(m_mCfgExt, 0x3b,    0xffff);
    l_FillArray(m_mCfgExt, 0x3c,    0xffff);
    l_FillArray(m_mCfgExt, 0x3d,    0xffff);
    l_FillArray(m_mCfgExt, 0x3e,    0xffff);
    l_FillArray(m_mCfgExt, 0x3f,    0xffff);
    l_FillArray(m_mCfgExt, 0x40,    0xffff);
    l_FillArray(m_mCfgExt, 0x41,    0xffff);
    l_FillArray(m_mCfgExt, 0x42,    0xffff);
    l_FillArray(m_mCfgExt, 0x43,    0xffff);
    l_FillArray(m_mCfgExt, 0x44,    0xffff);
    l_FillArray(m_mCfgExt, 0x45,    0xffff);
    l_FillArray(m_mCfgExt, 0x46,    0xffff);
    l_FillArray(m_mCfgExt, 0x47,    0xffff);
    l_FillArray(m_mCfgExt, 0x48,    0xffff);
    l_FillArray(m_mCfgExt, 0x49,    0xffff);
	
	l_FillArray(m_mCfgExt, 0x4a,    0x0104, 0x0105, 0x0106, 0xffff);
    l_FillArray(m_mCfgExt, 0x4b,    0x0114, 0x0115, 0xffff);
    l_FillArray(m_mCfgExt, 0x4c,    0x011b, /* 0x011c, */ 0x011e, 0x011f,
									/* 0x012c, */ 0xffff);

    l_FillArray(m_mCfgExt, 0x4d,    0x012c, 0x012d, 0x012e, 0x012f,
									0x0130, 0x0131, 0x015c, 0x015d,
									0x015e, 0x015f, 0x0160, 0xffff);

    l_FillArray(m_mCfgExt, 0x4e,    0x010d, 0x0161, 0x012f, 0x0130,
									0x0131, 0xffff);

    l_FillArray(m_mCfgExt, 0x4f,    0x0100, 0xffff);
    l_FillArray(m_mCfgExt, 0x50,    0x0101, 0xffff);
    l_FillArray(m_mCfgExt, 0x51,    0x0116, 0x0101, 0x012a, 0x012b,
									0xffff);

    l_FillArray(m_mCfgExt, 0x52,    0x012a, 0x012b, 0xffff);
    l_FillArray(m_mCfgExt, 0x53,    0x0103, 0x0107, 0x0108, 0x0109,
									0x010e, 0x010f, 0x0110, 0x0112,
									0x0113, 0xffff);

    l_FillArray(m_mCfgExt, 0x54,    0x010b, 0x010c, 0xffff);
    l_FillArray(m_mCfgExt, 0x55,    0x0159, 0xffff);

    l_FillArray(m_mCfgExt, 0x56,    0xffff);
    l_FillArray(m_mCfgExt, 0x57,    0xffff);
    l_FillArray(m_mCfgExt, 0x58,    0xffff);
    l_FillArray(m_mCfgExt, 0x59,    0xffff);
    l_FillArray(m_mCfgExt, 0x5a,    0xffff);
    l_FillArray(m_mCfgExt, 0x5b,    0xffff);
    l_FillArray(m_mCfgExt, 0x5c,    0xffff);
    l_FillArray(m_mCfgExt, 0x5d,    0xffff);
    l_FillArray(m_mCfgExt, 0x5e,    0xffff);
    l_FillArray(m_mCfgExt, 0x5f,    0xffff);
    l_FillArray(m_mCfgExt, 0x60,    0xffff);
    l_FillArray(m_mCfgExt, 0x61,    0xffff);
    l_FillArray(m_mCfgExt, 0x62,    0xffff);

}

BOOL CNesGraphics::LoadSprites(CNesFile & file)
{
	BYTE nSpritesBank = file[nfSpriteListBank];
	if ( nSpritesBank > 0x06 && !file.IsExtended() || nSpritesBank > 0x0e && file.IsExtended() )
	{
		// file can be patched with fast sprites patch
		nSpritesBank = file[nfFastPatchSpriteListBank];
		if ( nSpritesBank > 0x06 && !file.IsExtended() || nSpritesBank > 0x0e && file.IsExtended() )
			return FALSE;
	}

	WORD wPtrLists[] = 
	{
		MAKEWORD(file[nfSpriteList1Lo], file[nfSpriteList1Hi]),
		MAKEWORD(file[nfSpriteList2Lo], file[nfSpriteList2Hi]),
		MAKEWORD(file[nfSpriteList3Lo], file[nfSpriteList3Hi])
	};

	std::map<WORD, size_t> mSpr;

	for(int i = 0; i < sizeof(wPtrLists) / sizeof(wPtrLists[0]); ++i)
	{
		WORD wMinPtr = 0xFFFF, wCurPtr = 0, c = 2;
		m_vSprList[i].clear();

		wCurPtr = file.WordEx(BANK_OFFSET(nSpritesBank) + wPtrLists[i]);

		while(wCurPtr < wMinPtr || wCurPtr >= 0x8000 && wCurPtr <= 0xBFFF)
		{
			if ( mSpr.find(wCurPtr) == mSpr.end() )
			{
				mSpr[wCurPtr] = m_nBank.vSprite.size();

				if ( wCurPtr )
				{
					wMinPtr = min(wCurPtr, wMinPtr);
					WORD ptr = wCurPtr;
					std::vector<NES_SPRITE> vSprites;
					while( 0 != file.ByteEx(BANK_OFFSET(nSpritesBank) + int(ptr)) )
					{
						NES_SPRITE nSprite;
					
						nSprite.bCounter = file.ByteEx(BANK_OFFSET(nSpritesBank) + int(ptr++));
						nSprite.X = file.ByteEx(BANK_OFFSET(nSpritesBank) + int(ptr++));
						nSprite.Y = file.ByteEx(BANK_OFFSET(nSpritesBank) + int(ptr++));

						for(int i = 0; i < int(nSprite.bCount); ++i)
						{
							NES_SPRITE_TILE nt = { 0 };
							file.GetDataEx(BANK_OFFSET(nSpritesBank) + int(ptr), &nt, sizeof(nt));
							nSprite.vTiles.push_back(nt);
							ptr += sizeof(nt);
						}
						vSprites.push_back(nSprite);
					}
					m_nBank.vSprite.push_back(vSprites);
				}
				else
				{
					std::vector<NES_SPRITE> vSprites; // add empty spr list
					m_nBank.vSprite.push_back(vSprites);
					_tprintf(TEXT("Added empty sprite as 0x%02x%02x\n"), i, m_vSprList[i].size() - 1);
				}
			}

			m_vSprList[i].push_back(mSpr[wCurPtr]);

			wCurPtr = file.WordEx(BANK_OFFSET(nSpritesBank) + wPtrLists[i] + c);
			c += sizeof(wCurPtr);
		} 
	}
	return TRUE;
}

BOOL CNesGraphics::LoadGfx(CNesFile & file)
{
	const BYTE nGfxPtrsBank = file[nfGfxPtrsBank],
		nGfxBank = file[nfGfxBank];
	WORD wGfxPtrs = file.Word(nfGfxPtrs);

	if ( nGfxBank > 0x06 && !file.IsExtended() || nGfxBank > 0x0e && file.IsExtended() )
		return FALSE;

//	BYTE mgfx[0x62] = { 0 };

	std::map<WORD, size_t>	 mGfx;
	m_nBank.vGfx.clear();

	for(int level = 0; level < NES_COUNT_LEVELS; ++level)
	{
		for(int i = 0; i < 8; ++i)
		{
			WORD wCurPtr = file.WordEx(BANK_OFFSET(nGfxPtrsBank) + int(wGfxPtrs) + 2 * ( 8 * level + i ));
			m_vGfx[level][i] = 0;

			if ( mGfx.find(wCurPtr) != mGfx.end() )
			{
				m_vGfx[level][i] = mGfx[wCurPtr];
				continue;
			}

			m_vGfx[level][i] = mGfx[wCurPtr] = m_nBank.vGfx.size();

			std::vector<NES_GFX_OBJECT> vGfx;
			while(0xFF != file.ByteEx(BANK_OFFSET(nGfxBank) + int(wCurPtr)))
			{
				NES_GFX_OBJECT ngfx = { 0 };
				ngfx.bPPUAddr = file.ByteEx(BANK_OFFSET(nGfxBank) + int(wCurPtr++));
				ngfx.bSetId = file.ByteEx(BANK_OFFSET(nGfxBank) + int(wCurPtr++));
/*
				if ( !mgfx[ngfx.bSetId / 2] )
					mgfx[ngfx.bSetId / 2] = ngfx.bPPUAddr;
				else if ( mgfx[ngfx.bSetId / 2] != ngfx.bPPUAddr )
					_tprintf(TEXT("WARNING! Tileset 0x%02X loaded into multiple PPU addresses!\n"), ngfx.bSetId);
*/
				vGfx.push_back(ngfx);
			}
			m_nBank.vGfx.push_back(vGfx);
		}
	}

/*
	for(int i = 0; i < sizeof(mgfx) / sizeof(mgfx[0]); i++ )
	{
		_tprintf(TEXT("0x%02X, "), mgfx[i]);
		if ( (i + 1) % 16 == 0 )
			_tprintf(TEXT("\n\t"));
	}
*/

	return TRUE;
}

BOOL CNesGraphics::LoadTileSets(CNesFile & file)
{
	const WORD nSetsCount = 0xc4;
	const BYTE nTilesBank = file[nfTilesArchBank];
	WORD wArchTilesPtr = file.Word(nfTilesArchPtr);
	m_vTileStream.clear();

	if ( nTilesBank > 0x06 && !file.IsExtended() || nTilesBank > 0x0e && file.IsExtended() )
		return FALSE;

	WORD wCurPtr = file.WordEx(BANK_OFFSET(nTilesBank) + int(wArchTilesPtr)), c = 2;

	while(c < nSetsCount)
	{
		const BYTE nSetBank = (HIBYTE(wCurPtr) >> 6) + 1;
		wCurPtr = (wCurPtr & 0x3FFF) | 0x8000;

		std::vector<BYTE> vTileStream;

		BYTE bRepeatByte = file.ByteEx(BANK_OFFSET(nSetBank) + int(wCurPtr++));
		do
		{
			BYTE b = file.ByteEx(BANK_OFFSET(nSetBank) + int(wCurPtr++));
			if ( b == bRepeatByte )
			{
				BYTE bRepeat = file.ByteEx(BANK_OFFSET(nSetBank) + int(wCurPtr++));
				if ( !bRepeat )
					break;

				b = file.ByteEx(BANK_OFFSET(nSetBank) + int(wCurPtr++));
				vTileStream.insert(vTileStream.end(), bRepeat, b);
			}
			else
				vTileStream.push_back(b);
		} while(wCurPtr >= 0x8000);

		m_vTileStream.push_back(vTileStream);

		wCurPtr = file.WordEx(BANK_OFFSET(nTilesBank) + int(wArchTilesPtr) + c);
		c += sizeof(wCurPtr);
	}
	return TRUE;
}


BOOL CNesGraphics::LoadGraphics(CNesFile & file)
{
	if ( !file.IsFileLoaded() )
		return FALSE;

	m_fLoaded = LoadSprites(file) && LoadGfx(file) && LoadTileSets(file);
	return m_fLoaded;
}

BOOL CNesGraphics::Get_SprList(std::vector<std::vector<DWORD>> & vSprites)
{
	if ( !m_fLoaded )
		return FALSE;

	vSprites.push_back(m_vSprList[0]);
	vSprites.push_back(m_vSprList[1]);
	vSprites.push_back(m_vSprList[2]);
	return TRUE;
}

VOID CNesGraphics::Get_Sprite(DWORD id, std::vector<NES_SPRITE> & vSprite)
{
	if ( id >= DWORD(m_nBank.vSprite.size()) )
		return;

	vSprite = m_nBank.vSprite[id];
}

BOOL CNesGraphics::IsGfxLinked(INT level, INT id)
{
	if ( !m_fLoaded || level < 0 || level >= NES_COUNT_LEVELS || id < 0 || id >= 8 )
		return FALSE;

	BOOL fLink = FALSE;

	for(int lvl = 0; lvl < NES_COUNT_LEVELS; ++lvl)
	{
		for(int set = 0; set < 8; ++set)
		{
			if ( set != id && level != lvl && m_vGfx[level][id] == m_vGfx[lvl][set] )
			{
				fLink = TRUE;
				break;
			}
		}

		if ( fLink )
			break;
	}

	return fLink;
}


BOOL CNesGraphics::Get_Gfx(INT level, INT id, 
												std::vector<NES_SPRITE_INFO> & vSprInfo,
												std::vector<NES_TILE> & vTiles,
												BOOL & fLink
												)
{
	if ( !m_fLoaded || level < 0 || level >= NES_COUNT_LEVELS || id < 0 || id >= 8 )
		return FALSE;

	fLink = IsGfxLinked(level, id);

	return MakeTiles(m_nBank.vGfx[m_vGfx[level][id]], vTiles, vSprInfo);
}

BOOL CNesGraphics::Get_Tiles(INT setid, std::vector<NES_TILE> & vTiles)
{
	if ( !m_fLoaded || setid < 0 || setid >= int(m_vTileStream.size()) )
		return FALSE;

	if ( !m_mTileBases[setid] )
		return FALSE;


	NES_TILE tile = { 0 };
	vTiles.clear();
	vTiles.insert(vTiles.end(), 256, tile);

	std::vector<BYTE> & vTileStream = m_vTileStream[setid];

	if ( 0 != ( vTileStream.size() % 16 ) )
		return FALSE;

	size_t tcount = vTileStream.size() / 16;
	int base = m_mTileBases[setid];

	for(size_t tc = 0; tc < tcount; ++tc)
	{
		NES_TILE & tile = vTiles[base + tc];
		if ( base + tc > 0xff )
			break;

		int offset = 16 * tc;

		for(int y = 0; y < 8; ++y)
		{
			for(int x = 0; x < 8; ++x)
			{
				tile.pix[x][y].clr =
					( 1 & ( vTileStream[offset + y] >> ( 7 - x ) ) | ( 1 & ( vTileStream[offset + 8 + y] >> ( 7 - x ) ) ) << 1 );
			}
		}
	}
	return TRUE;
}

BOOL CNesGraphics::Get_SetObjects(INT setid, std::vector<DWORD> & vBestCfgs)
{
	if ( !m_fLoaded || setid < 0 || setid >= int(m_vTileStream.size()) )
		return FALSE;

	if ( !m_mTileBases[setid] )
		return FALSE;

	vBestCfgs = m_mCfgExt[setid];

	return TRUE;
}


BOOL CNesGraphics::MakeTiles(std::vector<NES_GFX_OBJECT> & vGfx, std::vector<NES_TILE> & vTiles, std::vector<NES_SPRITE_INFO> & vSprInfo)
{
	NES_TILE tile = { 0 };
	vTiles.clear();
	vTiles.insert(vTiles.end(), 256, tile);
	vSprInfo.clear();

	size_t count = vGfx.size();
	for(size_t n = 0; n < count; ++n)
	{
		NES_GFX_OBJECT & gfx = vGfx[n];
		int base = gfx.bPPUAddr;
		int spr = gfx.bSetId / 2;
		if ( spr >= int(m_vTileStream.size()) )
			continue;

		std::vector<BYTE> & vTileStream = m_vTileStream[spr];

		if ( 0 != ( vTileStream.size() % 16 ) )
			continue;

		NES_SPRITE_INFO sprinf = { 0 };
		size_t tcount = vTileStream.size() / 16;
		sprinf.bPPUAddr = gfx.bPPUAddr;
		sprinf.nLength = LOBYTE(tcount);
		sprinf.nStream = spr;
		vSprInfo.push_back(sprinf);

		for(size_t tc = 0; tc < tcount; ++tc)
		{
			NES_TILE & tile = vTiles[base + tc];
			if ( base + tc > 0xff )
				break;

			int offset = 16 * tc;

			for(int y = 0; y < 8; ++y)
			{
				for(int x = 0; x < 8; ++x)
				{
					tile.pix[x][y].clr =
						( 1 & ( vTileStream[offset + y] >> ( 7 - x ) ) | ( 1 & ( vTileStream[offset + 8 + y] >> ( 7 - x ) ) ) << 1 );
				}
			}
		}
	}

	return TRUE;
}

BOOL CNesGraphics::GetBestCfgForObject(INT level, INT set, INT obj, std::vector<DWORD> & vBestCfgs)
{
	if ( level < 0 || level >= NES_COUNT_LEVELS || set < 0 || set >= 8 || obj < 0 )
		return FALSE;

	std::vector<NES_GFX_OBJECT> & vGfx = m_nBank.vGfx[m_vGfx[level][set]];
	if ( obj >= int(vGfx.size()) )
		return FALSE;

	NES_GFX_OBJECT & gfx = vGfx[obj];
	
	int setid = gfx.bSetId / 2;

	if ( m_mCfgExt.find( setid ) == m_mCfgExt.end() )
		return FALSE;

	vBestCfgs = m_mCfgExt[setid];
	return TRUE;
}

BOOL CNesGraphics::DrawSprite(HDC hdc, 
							int center_x, int center_y, 				
							std::vector<NES_TILE> & vTiles,
							DWORD sprid,
							PBYTE pbPal)
{
	if ( sprid >= DWORD(m_nBank.vSprite.size()) )
		return FALSE;

	std::vector<NES_SPRITE> sprl = m_nBank.vSprite[sprid];
	size_t count = sprl.size();

	for(size_t n = 0; n < count; ++n)
	{
		NES_SPRITE & spr = sprl[n];
		POINT pt = { spr.X, spr.Y };

		for(int i = 0; i < int(spr.bCount); ++i)
		{
			int tile = spr.vTiles[i].bPPUAddr;
			if ( spr.fMirror )
				DrawTile(hdc, center_x + pt.x, center_y + pt.y + 8 * i, vTiles[tile], spr.vTiles[i].fMirror, &pbPal[4 * spr.vTiles[i].bAttrVal] );
			else
				DrawTile(hdc, center_x + pt.x + 8 * i, center_y + pt.y, vTiles[tile], spr.vTiles[i].fMirror, &pbPal[4 * spr.vTiles[i].bAttrVal] );
		}
	}

	return TRUE;
}

VOID CNesGraphics::DrawTile(HDC hdc, int hx, int hy, NES_TILE tile, BOOL fMirrorTile, PBYTE pbPal)
{
	COLORREF clr[4] = { 0, 0x808080, 0xA0A0A0, 0xFFFFFF };

	CBitmap bm;
	bm.Create(8, 8);
	BitBlt(bm.LockDC(), 0, 0, bm.Width(), bm.Height(), hdc, hx, hy, SRCCOPY);
	bm.FreeDC();

	if ( pbPal )
	{
		clr[0] = g_crNesColor[pbPal[0]];
		clr[1] = g_crNesColor[pbPal[1]];
		clr[2] = g_crNesColor[pbPal[2]];
		clr[3] = g_crNesColor[pbPal[3]];
	}

	COLORREF * pclr = (COLORREF*) bm.LockRawBits();

	if ( !fMirrorTile )
	{
		for(int y = 0; y < 8; ++y)
		{
			for(int x = 0; x < 8; ++x)
			{
				if ( !tile.pix[x][y].clr )
					continue;

				pclr[x + 8 * y] = clr[ tile.pix[x][y].clr ];
			}
		}
	}
	else
	{
		for(int y = 0; y < 8; ++y)
		{
			for(int x = 0; x < 8; ++x)
			{
				int crx = ( fMirrorTile & 1 ? 7 - x : x ),
					cry = ( fMirrorTile & 2 ? 7 - y : y );

				if ( !tile.pix[crx][cry].clr )
					continue;

				pclr[x + 8 * y] = clr[ tile.pix[crx][cry].clr ];
			}
		}	
	}

	BitBlt(hdc, hx, hy, bm.Width(), bm.Height(), bm.LockDC(), 0, 0, SRCCOPY);
	bm.FreeDC();
}

BOOL CNesGraphics::StorGfx(CNesFile & file)
{
	const BYTE nGfxPtrsBank = file[nfGfxPtrsBank],
		nGfxBank = file[nfGfxBank];
	WORD wGfxPtrs = file.Word(nfGfxPtrs);
	BOOL fExtended = file.IsExtended();

	if ( nGfxBank > 0x06 && !file.IsExtended() || nGfxBank > 0x0e && file.IsExtended() )
		return FALSE;

	// find mimimum ptr
	WORD wMinPtr = 0xffff;

	for(int level = 0; level < NES_COUNT_LEVELS; ++level)
	{
		for(int i = 0; i < 8; ++i)
		{
			WORD wCurPtr = file.WordEx(BANK_OFFSET(nGfxPtrsBank) + int(wGfxPtrs) + 2 * ( 8 * level + i ));
			if ( wCurPtr ) 
				wMinPtr = min(wMinPtr, wCurPtr);
		}
	}

	// store gfx
	WORD wPtr = wMinPtr;
	std::vector<WORD> vPtrList;
	size_t count = m_nBank.vGfx.size();

	for(size_t n = 0; n < count; ++n)
	{
		size_t scount = m_nBank.vGfx[n].size();
		vPtrList.push_back(wPtr);

		for(size_t sn = 0; sn < scount; ++sn)
		{
			NES_GFX_OBJECT & gfx = m_nBank.vGfx[n][sn];

			WORD w = MAKEWORD(gfx.bSetId, gfx.bPPUAddr);
			file.PutDataEx(BANK_OFFSET(nGfxBank) + int(wPtr), &gfx, sizeof(gfx));
			wPtr += sizeof(gfx);
		}
		file.ByteEx(BANK_OFFSET(nGfxBank) + int(wPtr++), 0xff);
	}

	// store ptrs
	for(int level = 0; level < NES_COUNT_LEVELS; ++level)
	{
		for(int i = 0; i < 8; ++i)
		{
			file.WordEx(BANK_OFFSET(nGfxPtrsBank) + int(wGfxPtrs) + 2 * ( 8 * level + i ), 
				vPtrList[ m_vGfx[level][i] ]);

			// duplicate array
			if ( fExtended )
				file.WordEx(BANK_OFFSET(nGfxPtrsBank) + int(wGfxPtrs) + 2 * ( 8 * level + i + ( 8 * NES_COUNT_LEVELS )), 
					vPtrList[ m_vGfx[level][i] ]);
		}
	}

	return TRUE;
}

BOOL CNesGraphics::StorSprites(CNesFile & file)
{
	BYTE nSpritesBank = file[nfSpriteListBank];
	if ( nSpritesBank > 0x06 && !file.IsExtended() || nSpritesBank > 0x0e && file.IsExtended() )
	{
		// file can be patched with fast sprites patch
		nSpritesBank = file[nfFastPatchSpriteListBank];
		if ( nSpritesBank > 0x06 && !file.IsExtended() || nSpritesBank > 0x0e && file.IsExtended() )
			return FALSE;
	}

	WORD wPtrLists[] = 
	{
		MAKEWORD(file[nfSpriteList1Lo], file[nfSpriteList1Hi]),
		MAKEWORD(file[nfSpriteList2Lo], file[nfSpriteList2Hi]),
		MAKEWORD(file[nfSpriteList3Lo], file[nfSpriteList3Hi])
	};

	std::map<DWORD, BOOL> mSpr;

	for(int i = 0; i < sizeof(wPtrLists) / sizeof(wPtrLists[0]); ++i)
	{
		size_t lcount = m_vSprList[i].size();

		for(size_t n = 0; n < lcount; ++n)
		{
			if ( mSpr.find( m_vSprList[i][n] ) == mSpr.end() )
			{
				mSpr[m_vSprList[i][n]] = 1;

				std::vector<NES_SPRITE> & vSprites = m_nBank.vSprite[ m_vSprList[i][n] ];
				size_t scount = vSprites.size();

				if ( scount > 0 )
				{
					WORD ptr = file.WordEx(BANK_OFFSET(nSpritesBank) + wPtrLists[i] + n * sizeof(ptr));

					for(size_t sn = 0; sn < scount; ++sn)
					{
						NES_SPRITE & nSprite = vSprites[sn];

						file.ByteEx(BANK_OFFSET(nSpritesBank) + int(ptr++), nSprite.bCounter);

						file.ByteEx(BANK_OFFSET(nSpritesBank) + int(ptr++), nSprite.X);

						file.ByteEx(BANK_OFFSET(nSpritesBank) + int(ptr++), nSprite.Y);

						size_t ncount = nSprite.vTiles.size();

						for(size_t i = 0; i < ncount; ++i)
						{
							NES_SPRITE_TILE & nt = nSprite.vTiles[i];
							file.PutDataEx(BANK_OFFSET(nSpritesBank) + int(ptr), &nt, sizeof(nt));
							ptr += sizeof(nt);
						}
					}
				}
			}
		} 
	}
	return TRUE;
}

BOOL CNesGraphics::StorGraphics(CNesFile & file)
{
	if ( !m_fLoaded || !file.IsFileLoaded() )
		return FALSE;

	return ( StorGfx(file) & StorSprites(file) );
}

BOOL CNesGraphics::CreateGfx(INT level, INT set)
{
	if ( !IsGfxLinked(level, set) )
		return FALSE;

	std::vector<NES_GFX_OBJECT> vGfx = m_nBank.vGfx[ m_vGfx[level][set] ];	// copy to new
	m_vGfx[level][set] = m_nBank.vGfx.size();	
	m_nBank.vGfx.push_back(vGfx);
	return TRUE;
}

BOOL CNesGraphics::MergeWith(INT level, INT set, INT linklevel, INT linkset)
{
	if ( !m_fLoaded || level < 0 || level >= NES_COUNT_LEVELS || set < 0 || set >= 8 ||
		linklevel < 0 || linklevel >= 8 || linkset < 0 || linkset >= 8 )
		return FALSE;

	if ( IsGfxLinked(level, set) )
		return FALSE;

	m_vGfx[level][set] = m_vGfx[linklevel][linkset];
	return TRUE;
}

BOOL CNesGraphics::GfxAddObject(INT level, INT set, INT setid)
{
	if ( !m_fLoaded || level < 0 || level >= NES_COUNT_LEVELS || set < 0 || set >= 8 || 
		setid < 0 || setid >= sizeof(m_mTileBases) / sizeof(m_mTileBases[0]) )
		return FALSE;

	std::vector<BYTE> vAvSets;
	if ( !GetAvailableSets(level, set, vAvSets) )
		return FALSE;
	
	size_t count = vAvSets.size();
	BOOL fFoundSet = FALSE;
	for(size_t n = 0; n < count; ++n)
		if ( vAvSets[n] == LOBYTE(setid) )
		{
			fFoundSet = TRUE;
			break;
		}

	if ( !fFoundSet )
		return FALSE;

	NES_GFX_OBJECT gfx;
	gfx.bSetId = LOBYTE(setid) * 2;
	gfx.bPPUAddr = m_mTileBases[setid];

	m_nBank.vGfx[m_vGfx[level][set]].push_back(gfx);

	return TRUE;
}

BOOL CNesGraphics::GfxDelObject(INT level, INT set, INT objid)
{
	if ( !m_fLoaded || level < 0 || level >= NES_COUNT_LEVELS || set < 0 || set >= 8 || objid < 0 )
		return FALSE;

	if ( objid >= int(m_nBank.vGfx[ m_vGfx[level][set] ].size()) )
		return FALSE;

	std::vector<NES_GFX_OBJECT> & gfx = m_nBank.vGfx[m_vGfx[level][set]];
	gfx.erase(gfx.begin() + objid);

	return TRUE;
}

BOOL CNesGraphics::GetAvailableSets(INT level, INT set, std::vector<BYTE> & vSets)
{
	if ( !m_fLoaded || level < 0 || level >= NES_COUNT_LEVELS || set < 0 || set >= 8 )
		return FALSE;

	CNesComparator ncPPU( m_nBank.vGfx[ m_vGfx[level][set] ], m_vTileStream );

	for(int i = 0; i < sizeof(m_mTileBases) / sizeof(m_mTileBases[0]); ++i)
	{
		if ( m_mTileBases[i] )
		{
			CNesComparator nc(m_mTileBases[i], m_vTileStream[i].size() / 0x10);
			if ( !ncPPU.And(nc) )
				vSets.push_back(LOBYTE(i));
		}
	}

	return ( vSets.size() > 0 );
}
