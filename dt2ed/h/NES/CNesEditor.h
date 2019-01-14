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

extern const COLORREF g_crNesColor[];

#define		NEDF_SHOWTILEMASK			0x01
#define		NEDF_SHOWSPECOBJ			0x02
#define		NEDF_SHOWROOMBORDERS		0x04
#define		NEDF_SHOWOUTSIDEOBJECTS		0x08

class CNesEditor
{
	BOOL					m_fLoaded;
	INT						m_nCurrentLevel;
	CNesRooms				m_nRooms;
	CNesFile *				m_pFile;
	CNesTiles				m_nTiles[NES_COUNT_LEVELS];
	CBitmap *				m_vpbmObj[257];
	RECT					m_rcObj[257];
	CBitmap *				m_vpbmScr[4];

	struct
	{
		BYTE				nHeaderPtr;
		BYTE				nBlocksPtr;

		struct
		{
			BYTE			nMinorBlocks[4];
			BYTE			nMinorBlocksPalettes;
		} nMinor;

		struct
		{
			BYTE			nMajorBlock[4];
		} nMajor;

		BYTE				nLayout;

		std::map<BYTE, BYTE>	nObject;
		BYTE					nPalette;
	} m_nLvlBanks[NES_COUNT_LEVELS];


	struct Banks
	{
		struct
		{
			std::vector<NES_LEVEL_HEADER>		nHeader;
			std::vector<NES_LEVEL_BLOCKS>		nBlocks;
		} nHeader;

		struct
		{
			std::vector<std::vector<BYTE>>		nMinorBlocks[4];
			std::vector<std::vector<BYTE>>		nMinorBlocksPalettes;
		} nMinor;

		struct
		{
			std::vector<std::vector<BYTE>>		nMajorBlocks[4];
		} nMajor;

		std::vector<NES_LAYOUT>					nLayout;
		std::vector<std::vector<BYTE>>			nPalette;
		std::vector<NES_OBJECTS_FLOOR>			nObjects[NES_COUNT_LEVELS];
		
		WORD									nPalLoaders[NES_COUNT_LEVELS];
		WORD									nObjHandlers[NES_COUNT_LEVELS];
		NES_BRIDGE								nBridgeData;
	} m_nBank;

	struct
	{
		BYTE									bPalette[16];
		BYTE									bPalTiles[64];
		BYTE									bLayout[48];
		std::vector<NES_TILE>					nTiles;
		std::vector<NES_BLOCK>					nMinorBlocks;
		std::vector<NES_BLOCK>					nMajorBlocks;
		std::vector<std::vector<NES_OBJECT>>	nObjects;
		INT										pObj;

		CBitmap *								pbRooms[6][8];
		CBitmap									bmMask[16];

		std::vector<std::vector<BYTE>>			nBlocks;
		std::vector<BYTE>						vTilesUsed;
	} m_nLevel;

	struct
	{
		BYTE									bDrawMask;
		BOOL									fExtended;
		BYTE									bBgTiles[0x0C];
		BYTE									sLevelName[NES_COUNT_LEVELS][12];
		TCHAR									sDecodedLevelName[NES_COUNT_LEVELS][13];
		BYTE									bEgyptCode[10];
		NES_MU_SECRET							nMuSecret;
		std::vector<NES_POINT>					nSteps[NES_COUNT_LEVELS][7];
		BYTE									nMoney[3][5];
		BYTE									nBossLives[NES_COUNT_LEVELS];
		BYTE									nBossTypes[NES_COUNT_LEVELS];
		WORD									wDrops[17];
	} m_nGame;

	BYTE					GetTileMask(BYTE tileId);
	BOOL					WriteTile(std::vector<BYTE> & ppMajorBlock, INT tx, INT ty, INT tile, BYTE nPal);

	BOOL					FlushLevel();
	BOOL					CacheLevel();
	BOOL					FreeLevel();

	BOOL					FreeRoomsImages();
	BOOL					CacheBlocks();
	BOOL					CacheRoomsImages();
	BOOL					WriteImage(CBitmap * pbmp, INT x, INT y, INT block, BYTE npal = 0);

	VOID					Erase(std::map<INT, INT> & eraser);
	VOID					Get_Layouts(std::map<INT, PNES_LAYOUT> & lo);
	VOID					Let_Layouts(std::map<INT, PNES_LAYOUT> & lo);
	VOID					Let_Objects();
	size_t					Get_LastRoom();
	BOOL					CheckObjArgs(BYTE nRoomX, BYTE nRoomY, BYTE bObjX, BYTE bObjY);
	INT						FindObject(BYTE nRoomX, BYTE & nRoomY, BYTE bObjX, BYTE bObjY);
	INT						FindObjectExact(BYTE nRoomX, BYTE nRoomY, BYTE bObjX, BYTE bObjY);

	BOOL					MakeSlotsPatch(CNesFile & file);
	BOOL					MakeSlotsPatchEx(CNesFile & file);
	BOOL					MakeNoScrollPatch(CNesFile & file);
	BOOL					MakeBridgePatch(CNesFile & file);
	BOOL					MakeFasterSpritesPatch(CNesFile & file);
	BOOL					LoadBridgeData(CNesFile & file);
	BOOL					StorBridgeData(CNesFile & file);
	BOOL					LoadEgyptSecretData(CNesFile & file);
	BOOL					StorEgyptSecretData(CNesFile & file);
	BOOL					LoadMuSecretData(CNesFile & file);
	BOOL					StorMuSecretData(CNesFile & file);

	// Extended methods

	BOOL					LoadBanks(CNesFile & file);
	BOOL					SaveBanks(CNesFile & file);
	BOOL					SaveObjects(CNesFile & file);
	BOOL					RemoveUnusedLinks();
	BOOL					RemoveUnusedRooms();
	BOOL					LoadSprites(CTilesCreator & tiles, CTilesCreator & tilesScrooge);

public:
	CNesEditor();
	~CNesEditor();

	INT			LoadFile(CNesFile & file, CTilesCreator & tiles, CTilesCreator & tilesScrooge);
	INT			StorFile(CNesFile & file);
	BOOL		SetCurrentLevel(INT nLevel);
	BOOL		SetExtendedNes();

	CNesTiles &	GetTileSet();

	BOOL		DrawRoom(HDC hDC, INT x, INT y, INT nRoomX, INT nRoomY);
	BOOL		DrawRoomObjects(HDC hDC, INT x, INT y, INT nRoomX, INT nRoomY, BYTE fDrawSteps = 0);
	BOOL		DrawBridge(HDC hDC, INT x, INT y, INT nRoomX, INT nRoomY);
	BOOL		DrawMuSecret(HDC hDC, INT x, INT y, INT nRoomX, INT nRoomY);
	BOOL		IsValidMuSecretRoom(BYTE nRoomX, BYTE nRoomY);
	INT			GetMuObjectId(BYTE nRoomX, BYTE nRoomY, BYTE nPosX, BYTE nPosY, NES_POINT & pt);
	BOOL		DeleteMuObject(int id);
	BOOL		InsertMuObject(BYTE nRoomX, BYTE nRoomY, BYTE nPosX, BYTE nPosY);
	BOOL		SetMuObject(BYTE nRoomX, BYTE nRoomY, BYTE nPosX, BYTE nPosY, int id);
	BOOL		ValidateMuObjectPos(POINT & pt);

	BOOL		GetRoomPoint(BYTE room, std::vector<POINT> & rc);
	BOOL		GetBlockFromPoint(INT x, INT y, BYTE & room, BYTE & block);
	BOOL		SetBlockFromPoint(INT x, INT y, BYTE room, BYTE block);

	BOOL		Let_LevelBlocks(PBYTE pbPalette, PBYTE pbPalTiles, std::vector<NES_TILE> & vTiles, std::vector<NES_BLOCK> & vMinor, std::vector<NES_BLOCK> & vMajor);
	BOOL		Get_LevelBlocks(PBYTE pbPalette, PBYTE pbPalTiles, std::vector<NES_TILE> & vTiles, std::vector<NES_BLOCK> & vMinor, std::vector<NES_BLOCK> & vMajor);
	BOOL		Get_BlocksUsing(std::vector<BYTE> & bUsedBlocks);
	BOOL		Get_TilesUsing(std::vector<BYTE> & bUsedBlocks);
	BOOL		Let_LevelTiles(std::vector<NES_BLOCK> & vMinor, PBYTE pbPalTiles);


	BOOL		Let_BgTiles(PBYTE pbBgTiles);
	BOOL		Get_BgTiles(PBYTE pbBgTiles);

	BOOL		InsertRoom(BYTE nRoomX, BYTE nRoomY);
	BOOL		SetRoom(BYTE nRoomX, BYTE nRoomY, BYTE bRoomId);
	BOOL		DeleteRoom(BYTE nRoomX, BYTE nRoomY);
	INT			GetCountRoomsLinks(BYTE bRoomId);

	WORD		GetObjectFromPos(BYTE nRoomX, BYTE nRoomY, BYTE bObjX, BYTE bObjY, POINT & pt, BOOL fExact = FALSE);
	BOOL		InsertObject(BYTE nRoomX, BYTE nRoomY, BYTE bObjX, BYTE bObjY, BYTE id);
	BOOL		SetObject(BYTE nRoomX, BYTE nRoomY, BYTE bObjX, BYTE bObjY, BYTE id);
	BOOL		DeleteObject(BYTE nRoomX, BYTE nRoomY, BYTE bObjX, BYTE bObjY);
	BOOL		DeleteObject(BYTE nRoomY, BYTE id);
	
	BOOL		InsertNewSlot(INT idSet, NES_POINT & pt);
	BOOL		DeleteSlot(INT idSet, INT idSlot);
	INT			GetSlotFromPos(INT idSet, INT x, INT y);
	BOOL		IsValidSlot(INT idSet, INT idSlot);

	BOOL		Get_TileBankInfo(PBYTE pbTilesBanks);
	BOOL		Let_TileBankInfo(BYTE bTilesBank, BOOL fCreateNewBank = FALSE);
	BOOL		Get_ScroogePos(NES_POINT & pt, BYTE id);
	BOOL		Let_ScroogePos(NES_POINT & pt, BYTE id);
	BOOL		Get_LevelName(INT n, CString & sLevelName);
	BOOL		Let_LevelName(INT n, CString & sLevelName);
	BOOL		Get_ExtendedLevel();
	BOOL		Let_ExtendedLevel(BYTE nBasedOnLevel);

	BOOL		Get_Costs(PBYTE pbCosts);
	BOOL		Let_Costs(PBYTE pbCosts);

	BOOL		Get_Drops(PWORD pwDrops);
	BOOL		Let_Drops(PWORD pwDrops);

	BOOL		Get_LevelBridge(NES_BRIDGE & nb);
	BOOL		Let_LevelBridge(NES_BRIDGE & nb);

	BOOL		Get_EgyptSecret(PBYTE pbCode);
	BOOL		Let_EgyptSecret(PBYTE pbCode);

	BOOL		Get_MuSecret(NES_MU_SECRET & nmu);
	BOOL		Let_MuSecret(NES_MU_SECRET & nmu);

	BOOL		Get_RoomData(BYTE bRoom, NES_ROOM & nRoom);
	BOOL		Let_RoomData(BYTE bRoom, NES_ROOM & nRoom);

	BOOL		Get_LevelData(PBYTE pbPalette, BYTE & bSound, BYTE & bBossLife, BYTE & bBossType);
	BOOL		Let_LevelData(PBYTE pbPalette, BYTE bSound, BYTE bBossLife, BYTE bBossType);

	BOOL		Get_PaletteEx(PBYTE pbPalette);

	BOOL		SetDrawMask(BYTE bMask);
};

template<class T> class CNesGarbageRemoval
{
	std::vector<T> &			m_vArray;
	PBYTE						m_pbLinks;

public:
	CNesGarbageRemoval(std::vector<T> & vArray, PBYTE pbLinks);
	size_t						RemoveUnused();
};

template<class T> CNesGarbageRemoval<T>::CNesGarbageRemoval(std::vector<T> & vArray, PBYTE pbLinks)
	: m_vArray( vArray )
{
	m_pbLinks = pbLinks;
}

template<class T> size_t CNesGarbageRemoval<T>::RemoveUnused()
{
	std::map<INT, std::vector<BYTE>> vArr;
	for(BYTE i = 0; i < NES_COUNT_LEVELS; ++i)
		vArr[m_pbLinks[i]].push_back(i);

	if ( vArr.size() == m_vArray.size() )
		return 0;

	std::vector<T> l_vArray;
	for(std::map<INT, std::vector<BYTE>>::iterator v = vArr.begin();
		v != vArr.end(); v++)
	{
		size_t count = v->second.size();
		if ( count > 0 )
		{
			BYTE id = LOBYTE(l_vArray.size());
			for(size_t n = 0; n < count; ++n)
				m_pbLinks[ v->second[n] ] = id;

			l_vArray.push_back( m_vArray[ v->first ] );
		}
	}

	size_t counter = m_vArray.size() - l_vArray.size();
	m_vArray = l_vArray;
	return counter;
}