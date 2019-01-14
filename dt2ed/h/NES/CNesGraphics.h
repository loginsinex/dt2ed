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

typedef struct _tagNES_SPRITE_TILE
{
	BYTE			bPPUAddr;
	union
	{
		BYTE			bAttr;
		struct
		{
			BYTE		bAttrVal:6;
			BYTE		fMirror:2;
		};
	};
} NES_SPRITE_TILE, *PNES_SPRITE_TILE;

typedef struct _tagNES_SPRITE
{
	union
	{
		BYTE								bCounter;
		struct
		{
			BYTE								bCount:7;
			BYTE								fMirror:1;
		};
	};
	char								X;
	char								Y;
	std::vector<NES_SPRITE_TILE>		vTiles;
} NES_SPRITE, *PNES_SPRITE;

typedef struct _tagNES_GFX_OBJECT
{
	BYTE	bPPUAddr;
	BYTE	bSetId;
} NES_GFX_OBJECT, *PNES_GFX_OBJECT;

typedef struct _tagNES_SPRITE_INFO
{
	BYTE	bPPUAddr;
	BYTE	nLength;
	BYTE	nStream;
} NES_SPRITE_INFO, *PNES_SPRITE_INFO;

class CNesComparator
{
	DWORD				m_dwPPUMask[8];
	VOID				FillArea(BYTE bStart, BYTE bLength);

public:
	CNesComparator(BYTE bStart, BYTE bLength);
	CNesComparator(std::vector<NES_GFX_OBJECT> & vCfg, std::vector<std::vector<BYTE>> vTileStream);

	BOOL				And(CNesComparator & nc);
};

class CNesGraphics
{
	BOOL									m_fLoaded;

	std::vector<DWORD>						m_vSprList[3];
	INT										m_vGfx[NES_COUNT_LEVELS][8];
	std::vector<std::vector<BYTE>>			m_vTileStream;
	std::map<INT, std::vector<DWORD>>		m_mCfgExt;

	struct
	{
		std::vector<std::vector<NES_GFX_OBJECT>>		vGfx;
		std::vector<std::vector<NES_SPRITE>>			vSprite;
	} m_nBank;

	BOOL				LoadSprites(CNesFile & file);
	BOOL				LoadGfx(CNesFile & file);
	BOOL				LoadTileSets(CNesFile & file);
	BOOL				StorGfx(CNesFile & file);
	BOOL				StorSprites(CNesFile & file);

	BOOL				MakeTiles(std::vector<NES_GFX_OBJECT> & vGfx, std::vector<NES_TILE> & vTiles, std::vector<NES_SPRITE_INFO> & vSprInfo);
	VOID				DrawTile(HDC hdc, int hx, int hy, NES_TILE tile, BOOL fMirrorTile, PBYTE pbPal);
	BOOL				IsGfxLinked(INT level, INT set);

public:
	CNesGraphics();
	BOOL				LoadGraphics(CNesFile & file);
	BOOL				StorGraphics(CNesFile & file);

	BOOL				Get_SprList(std::vector<std::vector<DWORD>> & vSprites);
	VOID				Get_Sprite(DWORD id, std::vector<NES_SPRITE> & vSprite);

	BOOL				Get_Gfx(INT level, INT id, 
								std::vector<NES_SPRITE_INFO> & vSprInfo,
								std::vector<NES_TILE> & vTiles, BOOL & fLink);

	BOOL				Get_Tiles(INT setid, std::vector<NES_TILE> & vTiles);
	BOOL				Get_SetObjects(INT setid, std::vector<DWORD> & vBestCfgs);

	BOOL				GetBestCfgForObject(INT level, INT set, INT obj, std::vector<DWORD> & vBestCfgs);
	BOOL				CreateGfx(INT level, INT set);
	BOOL				MergeWith(INT level, INT set, INT linklevel, INT linkset);
	BOOL				GfxAddObject(INT level, INT set, INT setid);
	BOOL				GfxDelObject(INT level, INT set, INT objid);
	BOOL				GetAvailableSets(INT level, INT set, std::vector<BYTE> & vSets);

	BOOL				DrawSprite(HDC hdc, 
							int x, int y, 				
							std::vector<NES_TILE> & vTiles,
							DWORD sprid,
							PBYTE pbPal);
};