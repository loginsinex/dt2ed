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

typedef enum _tagNES_PATCH_ENUM
{
	nfRoomsBank		= 0,
	nfRoomsOffset		= 1,
	nfBgTiles		= 2,
	nfLevelsNames		= 3,
	nfBridgeLeftScrY		= 4,
	nfBridgeLeftY		= 5,
	nfBridgeLeftScrX		= 6,
	nfBridgeLeftX		= 7,
	nfBridgeRightScrX		= 8,
	nfBridgeRightX		= 9,
	nfBridgeBgTile		= 10,
	nfBridgeObjectId		= 11,
	nfBridgeLeftScrY2		= 12,
	nfEgyptCode		= 13,
	nfMuSecretObjectId		= 14,
	nfMuSecretPosY		= 15,
	nfMuSecretPosType		= 16,
	nfMuSecretCounter		= 17,
	nfMuSecretDataPtr		= 18,
	nfMuSecretCounter2		= 19,
	nfMuSecretObjectId2		= 20,
	nfMuSecretPosY2		= 21,
	nfMuSecretDataPtrVal		= 22,
	nfMuSecretDataReaderPtr1		= 23,
	nfMuSecretDataReaderPtr2		= 24,
	nfMuSecretDataReaderPtr3		= 25,
	nfMuSecretDataReader2Ptr1		= 26,
	nfMuSecretDataReader2Ptr2		= 27,
	nfMuSecretPosType2		= 28,
	nfMuSecretPosType3		= 29,
	nfMuSecretPos2Type1		= 30,
	nfMuSecretPos2Type2		= 31,
	nfMuSecretPos2Type3		= 32,
	nfMuSecretMask		= 33,
	nfTextTiles		= 34,
	nfTextTiles2		= 35,
	nfTextBank		= 36,
	nfTextArrayPtr		= 37,
	nfTextArray2Ptr		= 38,
	nfTextBank1		= 39,
	nfTextArrayPtr1		= 40,
	nfTextArray2Ptr1		= 41,
	nfTextBank2		= 42,
	nfTextArrayPtr2		= 43,
	nfTextArray2Ptr2		= 44,
	nfTextBank3		= 45,
	nfTextArrayPtr3		= 46,
	nfTextArray2Ptr3		= 47,
	nfTextBank4		= 48,
	nfTextArrayPtr4		= 49,
	nfTextArray2Ptr4		= 50,
	nfTextBank5		= 51,
	nfTextArrayPtr5		= 52,
	nfTextArray2Ptr5		= 53,
	nfTextWordsArrayPtr		= 54,
	nfTextWordsArrayPtr2		= 55,
	nfTextOtherDialog1Lo		= 56,
	nfTextOtherDialog1Hi		= 57,
	nfTextOtherDialog2Lo		= 58,
	nfTextOtherDialog2Hi		= 59,
	nfTextOtherDialog3Lo		= 60,
	nfTextOtherDialog3Hi		= 61,
	nfTextOtherDialog4Lo		= 62,
	nfTextOtherDialog4Hi		= 63,
	nfTextOtherDialog5Lo		= 64,
	nfTextOtherDialog5Hi		= 65,
	nfTextOtherDialog6Lo		= 66,
	nfTextOtherDialog6Hi		= 67,
	nfTextOtherDialog7Lo		= 68,
	nfTextOtherDialog7Hi		= 69,
	nfTextOtherDialog8Lo		= 70,
	nfTextOtherDialog8Hi		= 71,
	nfTextPtrEx		= 72,
	nfTextWordsBank		= 73,
	nfHiddensSlotsPtrsArray		= 74,
	nfHiddensSlotsPtrsEx		= 75,
	nfFastSpritesProc1		= 76,
	nfFastSpritesProc2		= 77,
	nfFastSpritesProc3		= 78,
	nfFastSpritesProc4		= 79,
	nfFastSpritesProc5		= 80,
	nfFastSpritesProc6		= 81,
	nfFastSpritesProc7		= 82,
	nfFastSpritesProc8		= 83,
	nfFastSpritesProc9		= 84,
	nfFastSpritesProc10		= 85,
	nfSpriteListBank		= 86,
	nfSpriteList1Lo		= 87,
	nfSpriteList1Hi		= 88,
	nfSpriteList2Lo		= 89,
	nfSpriteList2Hi		= 90,
	nfSpriteList3Lo		= 91,
	nfSpriteList3Hi		= 92,
	nfTilesArchBank		= 93,
	nfTilesArchPtr		= 94,
	nfGfxPtrsBank		= 95,
	nfGfxPtrs		= 96,
	nfGfxBank		= 97,
	nfFastPatchSpriteListBank		= 98,
	nfHiddensSlotsPtrs2Array		= 99,
	nfHiddensSlotsPatch		= 100,
	nfHiddensSlotsPatch2		= 101,
	nfHiddensSlotsPatch3		= 102,
	nfMoney50k		= 103,
	nfMoney1000k		= 104,
	nfMoney10k		= 105,
	nfBossLives		= 106,
	nfBossTypes		= 107,
	nfTextOtherDialog9Lo		= 108,
	nfTextOtherDialog9Hi		= 109,
	nfTextOtherDialog10Lo		= 110,
	nfTextOtherDialog10Hi		= 111,
	nfBoss2XOffset		= 112,
	nfBoss4XOffset		= 113,
	nfDropsPtr		= 114 
} NES_PATCH_ENUM, *PNES_PATCH_ENUM;
