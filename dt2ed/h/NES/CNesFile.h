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

#include "cnesdata.h"
#include "CNesPatchEnum.h"

#define						NES_FILE_SIZE			131088
#define						NES_FILE_SIZE_UOROM		262160
#define						NES_FILE_SIGNATURE		0x1A53454E

#define						NES_LOADER_ERROR_ACCESS		ERROR_ACCESS_DENIED
#define						NES_LOADER_ERROR_CHECKSUM	ERROR_CRC

#define						NES_FILE_EXTENDED_OFFSET(v, offset)	\
							( (v).size() == NES_FILE_SIZE_UOROM && (offset) >= 0x1C010 ? 0x20000 : 0 )

/*
bridge chars:
	BANK #06
	Left Screen: #A00B
	Right Screen: #A017
	Left Screen Offset (in pixels): #A011
	Right Screen Offset (in pixels): #A01D
	Y of bridge in screen #00 (in pixels): #A027
	Top screen: #00 - can be fixed only by code patch ( see code at #9FE3, where $001B - is a Y screen)
*/


#define			NES_COUNT_LEVELS				7
#define			NES_BGTILES_PTR					0x1F0C4
#define			NES_EGYPT_CODE_PTR				0x1A55A		// 10 bytes
#define			NES_GYRO_TOOLS_PTR				0x1D3B4		// 1 - pogo jump, 2 - aside break, 8 - move power

#define			BANK_OFFSET(bank)			( 0x4000 * int( bank ) + 0x10 - 0x8000 )


#define		NES_LEVELS_LIST				0x02E10
#define		NES_LEVELS_PTRS				0x02E17
#define		NES_OBJECTS_PTRS			0x19434

#define		NES_OBJECTS_BANKID_PTR		0x1F443
#define		NES_OBJECTS_PTR_PTR			0x1F454

#pragma pack(push, 1)
typedef struct _tagNES_POINT
{
	BYTE bPosX;
	BYTE bScrX;
	BYTE bPosY;
	BYTE bScrY;
} NES_POINT, *PNES_POINT;

typedef struct _tagNES_DOT
{
	BYTE bPos;
	BYTE bScr;
} NES_DOT, *PNES_DOT;

typedef struct _tagNES_LEVEL_HEADER
{
	USHORT ptrTiles;
	USHORT ptrPalette;
	NES_POINT start;
	NES_POINT restart;
	NES_POINT bossrestart;
	WORD ptrUnused;
	WORD ptrPPUCorrector;
	WORD ptrPPUSpritesCorrector;
	WORD ptrUnknown3;
	WORD ptrBlocks;
	BYTE bgSound;
} NES_LEVEL_HEADER, *PNES_LEVEL_HEADER;

typedef struct _tagNES_LEVEL_BLOCKS
{
	// this array has size of 48 bytes (8x6)
	WORD ptrLayout;
	
	// we have no information about sizes of below 4 arrays
	// so calculate max block #id from level data and use maximum #id as size of arrays
	WORD ptrMajorBlocks1; 
	WORD ptrMajorBlocks2;
	WORD ptrMajorBlocks3;
	WORD ptrMajorBlocks4;

	// below arrays has 256 bytes amount
	WORD ptrMinorBlocks1;
	WORD ptrMinorBlocks2;
	WORD ptrMinorBlocks3;
	WORD ptrMinorBlocks4;
	WORD ptrMinorBlocksPalettes;
} NES_LEVEL_BLOCKS, *PNES_LEVEL_BLOCKS;

typedef struct _tagNES_OBJECT
{
	union
	{
		BYTE		offset;
		struct
		{
			BYTE			objectX:5;
			BYTE			screen:3;
		};
	};

	BYTE			objectY;
	BYTE			id;
} NES_OBJECT, *PNES_OBJECT;
#pragma pack(pop)

typedef struct _tagNES_BLOCK
{
	BYTE			bUpLeft;
	BYTE			bUpRight;
	BYTE			bBtLeft;
	BYTE			bBtRight;
} NES_BLOCK, *PNES_BLOCK;

typedef struct _tagNES_LAYOUT
{
	BYTE		bRoom[48];
} NES_LAYOUT, *PNES_LAYOUT;

typedef struct _tagNES_OBJECTS_FLOOR
{
	BYTE						bFloor;
	std::vector<NES_OBJECT>		nObj;
	BOOL						fLastFloor;
} NES_OBJECTS_FLOOR, *PNES_OBJECTS_FLOOR;

typedef struct _tagNES_BRIDGE
{
	NES_POINT					ptLeft;
	struct
	{
		BYTE					bScrX;
		BYTE					bPosX;
	} ptRight;
	BYTE						bObjectId;
	BYTE						bBgTile;
} NES_BRIDGE, *PNES_BRIDGE;

typedef struct _tagNES_MU_SECRET
{
	BOOL						fVertical;
	BYTE						bObjId;
	BYTE						bPosY;
	std::vector<NES_DOT>		vobjX;
} NES_MU_SECRET, *PNES_MU_SECRET;

/*
typedef enum _tagNES_PATCH_ENUM
{
	nfRoomsBank,
	nfRoomsOffset,
	nfBgTiles,
	nfLevelsNames,

	nfBridgeLeftScrY,
	nfBridgeLeftY,
	nfBridgeLeftScrX,
	nfBridgeLeftX,
	nfBridgeRightScrX,
	nfBridgeRightX,
	nfBridgeBgTile,
	nfBridgeObjectId,
	nfBridgeLeftScrY2,

	nfEgyptCode,

	nfMuSecretObjectId,
	nfMuSecretPosY,
	nfMuSecretPosType,
	nfMuSecretCounter,
	nfMuSecretDataPtr,
	nfMuSecretCounter2,
	nfMuSecretObjectId2,
	nfMuSecretPosY2,
	nfMuSecretDataPtrVal,
	nfMuSecretDataReaderPtr1,
	nfMuSecretDataReaderPtr2,
	nfMuSecretDataReaderPtr3,
	nfMuSecretDataReader2Ptr1,
	nfMuSecretDataReader2Ptr2,
	nfMuSecretPosType2,
	nfMuSecretPosType3,
	nfMuSecretPos2Type1,
	nfMuSecretPos2Type2,
	nfMuSecretPos2Type3,
	nfMuSecretMask,

	nfTextTiles,
	nfTextTiles2,
	nfTextBank,
	nfTextArrayPtr,
	nfTextArray2Ptr,
	nfTextBank1,
	nfTextArrayPtr1,
	nfTextArray2Ptr1,
	nfTextBank2,
	nfTextArrayPtr2,
	nfTextArray2Ptr2,
	nfTextBank3,
	nfTextArrayPtr3,
	nfTextArray2Ptr3,
	nfTextBank4,
	nfTextArrayPtr4,
	nfTextArray2Ptr4,
	nfTextBank5,
	nfTextArrayPtr5,
	nfTextArray2Ptr5,
	nfTextWordsArrayPtr,
	nfTextWordsArrayPtr2,
	nfTextOtherDialog1Lo,
	nfTextOtherDialog1Hi,
	nfTextOtherDialog2Lo,
	nfTextOtherDialog2Hi,
	nfTextOtherDialog3Lo,
	nfTextOtherDialog3Hi,
	nfTextOtherDialog4Lo,
	nfTextOtherDialog4Hi,
	nfTextOtherDialog5Lo,
	nfTextOtherDialog5Hi,
	nfTextOtherDialog6Lo,
	nfTextOtherDialog6Hi,
	nfTextOtherDialog7Lo,
	nfTextOtherDialog7Hi,
	nfTextOtherDialog8Lo,
	nfTextOtherDialog8Hi,
	nfTextPtrEx,
	nfTextWordsBank,

	nfHiddensSlotsPtrsArray,
	nfHiddensSlotsPtrsEx
} NES_PATCH_ENUM, *PNES_PATCH_ENUM;
*/

/*
typedef enum _tagNES_PATCH_DATA_TYPE
{
	dtNull,
	dtByte,
	dtWord,
	dtDword,
	dtStream
} NES_PATCH_DATA_TYPE, *PNES_PATCH_DATA_TYPE;

typedef struct _tagNES_PATCH_DATA
{
	BYTE					nBank;
	WORD					ptr;
	NES_PATCH_DATA_TYPE		dType;

	_tagNES_PATCH_DATA() : nBank( 0 ), ptr( 0 ), dType( dtNull ) { }
} NES_PATCH_DATA, *PNES_PATCH_DATA;


typedef struct _tagNES_DATA
{
	NES_PATCH_DATA			pData;
	NES_PATCH_DATA			pExtendedData;
} NES_DATA, *PNES_DATA;
*/

class CNesFileLoader
{
	CString					m_sNesFile;
	std::vector<BYTE>		m_vData;

	INT						ValidateFile(std::vector<BYTE> & vData);

public:
	CNesFileLoader();

	INT						LoadFile(CString sFilename);
	INT						SaveAsFile(CString sFilename);
	INT						SaveFile(LPCTSTR pszFilename = NULL);
	INT						FreeFile();

	BOOL					IsFileLoaded();
	BOOL					IsExtended();
	VOID					Byte(INT offset, BYTE w);
	VOID					Word(INT offset, USHORT w);
	VOID					Long(INT offset, DWORD w);
	BYTE					Byte(INT offset);
	USHORT					Word(INT offset);
	DWORD					Long(INT offset);

	std::vector<BYTE>	&	Data(INT offset, std::vector<BYTE> & v, INT size);
	VOID					Data(INT offset, std::vector<BYTE> & v);

	VOID					GetData(INT offset, PVOID pData, INT size);
	VOID					PutData(INT offset, PVOID pData, INT size);

	BYTE				&	GetByteEx(INT offset);
	VOID					ByteEx(INT offset, BYTE w);
	VOID					WordEx(INT offset, USHORT w);
	VOID					LongEx(INT offset, DWORD w);
	BYTE					ByteEx(INT offset);
	USHORT					WordEx(INT offset);
	DWORD					LongEx(INT offset);

	VOID					GetDataEx(INT offset, PVOID pData, INT size);
	VOID					PutDataEx(INT offset, PVOID pData, INT size);

	BYTE	&				operator[](int offset);

	CString					Filename();
	operator LPTSTR();
	operator LPCTSTR();
	operator CString();

	BOOL					SetFileExtended();
};

class CNesFile: public CNesFileLoader
{
	BYTE							m_bNullByte;
	std::vector<NES_DATA>			m_vPatchData;

	inline BOOL				IsValidRequest(NES_PATCH_ENUM pe, NES_DATA_PATCH & pData);

public:
	using CNesFileLoader::Byte;
	using CNesFileLoader::Word;
	using CNesFileLoader::Long;
	using CNesFileLoader::GetData;
	using CNesFileLoader::PutData;
	using CNesFileLoader::operator[];

	BOOL					LoadPatcher(HINSTANCE hInstance, LPCTSTR pszType, LPCTSTR pszName);

	BOOL					Let_Patcher(std::vector<NES_DATA> & vPatchData);
	BOOL					Get_Patcher(std::vector<NES_DATA> & vPatchData);


	BYTE	&				operator[](NES_PATCH_ENUM pe);
	VOID					Byte(NES_PATCH_ENUM pe, BYTE w);
	VOID					Word(NES_PATCH_ENUM pe, USHORT w);
	VOID					Long(NES_PATCH_ENUM pe, DWORD w);
	BYTE					Byte(NES_PATCH_ENUM pe);
	USHORT					Word(NES_PATCH_ENUM pe);
	DWORD					Long(NES_PATCH_ENUM pe);

	VOID					GetData(NES_PATCH_ENUM pe, PVOID pData, INT size);
	VOID					PutData(NES_PATCH_ENUM pe, PVOID pData, INT size);

	BOOL					PutPatch(NES_PATCH_ENUM pe);

	BYTE					Bank(NES_PATCH_ENUM pe);
	WORD					Ptr(NES_PATCH_ENUM pe);
};