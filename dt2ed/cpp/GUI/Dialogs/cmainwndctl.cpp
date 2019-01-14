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
#include <shellapi.h>

void CMainWnd::OnButton(USHORT uId)
{
	switch(uId)
	{
	case ID_FILE_OPEN_TEXT:					Ctl_OnOpenFile();			break;
	case ID_FILE_SAVEAS:
	case ID_FILE_SAVE_TEXT:					Ctl_OnSaveFile(ID_FILE_SAVEAS == uId);			break;
	case ID_EDIT_BLOCKSEDITOR:				Ctl_OnEditBlocksEditor();	break;
	case ID_EDIT_CONVERTTOUOROM:			Ctl_ConvertToUOROM();		break;
	case ID_EDIT_FIND_TEXT:					Ctl_OnSelectLevel();		break;
	case ID_EDIT_REPLACE_TEXT:				Ctl_OnShowTileset();		break;
	case ID_EDIT_CREATENEWLEVEL:			Ctl_OnCreateLevel();		break;
	case ID_EDIT_TEXTEDITOR:				Ctl_OnTextEditor();			break;
	case ID_HELP_ABOUT:						Ctl_OnAbout();				break;
	case ID_FILE_QUIT:						Ctl_OnQuit();				break;
	case ID_EDIT_TILESEDITOR:				Ctl_OnEditTilesEditor();	break;
	case ID_BLOCKSANDTILES_TILESBANK:		Ctl_OnChangeTilesBank();	break;
	case ID_SHOWDIALOG:						Ctl_OnShowDialog();			break;
	case ID_EDIT_GAMESETTINGS:				Ctl_OnGameSettings();		break;
	case ID_TOOLS_FLUSHOBJECTS:				Ctl_OnFlushObjects();		break;
	case ID_EDIT_LEVELSETTINGS:				Ctl_OnLevelSettings();		break;
	case ID_TOOLS_GRAPHICSEDITOR:			Ctl_OnGraphicsEditor();		break;
	case ID_RUN:							Ctl_OnRun();				break;
	case ID_REMOVEALLSLOTS:					Ctl_OnRemoveLevelSlots();	break;

	case ID_CANVAS_SHOWROOMSBORDERS:
	case ID_CANVAS_SHOWOUTSIDEOBJECTS:
	case ID_CANVAS_SHOWSPECIALOBJECTS:
	case ID_CANVAS_SHOWTILESMASK:			Ctl_OnCanvasMask(uId);			break;
					
	case ID_ACTIVELAYOUT_TILESLAYOUT:
	case ID_ACTIVELAYOUT_ROOMSLAYOUT:
	case ID_ACTIVELAYOUT_OBJECTSLAYOUT:
	case ID_ACTIVELAYER_SCROOGELAYER:
	case ID_ACTIVELAYER_CHECKPOINTLAYER:
	case ID_ACTIVELAYER_HIDDENSLOTSLAYER:
	case ID_ACTIVELAYERBRIDGE:
	case ID_ACTIVELAYER_BOSSRESPAWNLAYER:
	case ID_ACTIVELAYER_MUSECRETLAYER:		Ctl_OnActivateLayer(uId);	break;
	}
}

// -------- CONTROLS -------- 

VOID CMainWnd::OnOpenFile(CString s)
{
	if ( m_File.LoadFile(s) )
		ShowErrorF(TEXT("Unable to open file '%s'\nIt is a not valid NES file or file access error"),
			PathFindFileName(s));
	else
	{
		// try to load patcher
		CNesDataFile ndf;
		if ( ndf.OpenFrom(GetInstance(), TEXT("LDR"), MAKEINTRESOURCE(IDR_LDR1)) )
		{
			std::vector<NES_DATA> vData;
			ndf.Get_Data(vData);
			m_File.Let_Patcher(vData);
		}
		else
		{
			ShowError(TEXT("Unable to load suitable loader!"));
			m_File.FreeFile();
			return;
		}

		if ( !m_nTextEditor.LoadFile(m_File) )
		{
			ShowError(TEXT("Unable to load ROM. Possible unsupported locale.\nCurrently supported only [U] ROM."));
			m_File.FreeFile();
		}
		else
		{
			m_nGraphics.LoadGraphics(m_File);

			m_pMenu->EnableItems(TRUE, ID_EDIT_FIND_TEXT, ID_FILE_SAVE_TEXT, ID_FILE_SAVEAS, ID_EDIT_REPLACE_TEXT, ID_EDIT_BLOCKSEDITOR, ID_EDIT_TILESEDITOR,
				ID_BLOCKSANDTILES_TILESBANK, ID_EDIT_TEXTEDITOR, ID_EDIT_GAMESETTINGS, ID_EDIT_LEVELSETTINGS, ID_TOOLS_FLUSHOBJECTS, ID_TOOLS_GRAPHICSEDITOR,
				ID_RUN,
				0);

			m_tb.EnableAll(TRUE, ID_EDIT_FIND_TEXT, ID_FILE_SAVE_TEXT, ID_FILE_SAVEAS, ID_EDIT_REPLACE_TEXT, ID_EDIT_BLOCKSEDITOR, ID_EDIT_TILESEDITOR,
				ID_BLOCKSANDTILES_TILESBANK, ID_EDIT_TEXTEDITOR, ID_EDIT_GAMESETTINGS, ID_EDIT_LEVELSETTINGS, ID_TOOLS_FLUSHOBJECTS, ID_TOOLS_GRAPHICSEDITOR,
				ID_RUN,
				0);

			
			m_dlgLeftPane.EnableLayers(TRUE);
			m_dlgLeftPane.EnableItem(TRUE, ID_FILE_SAVE_TEXT);

			m_pMenu->EnableItems(!m_File.IsExtended(), ID_EDIT_CONVERTTOUOROM, 0);
			m_tb.EnableAll(!m_File.IsExtended(), ID_EDIT_CONVERTTOUOROM, 0);

			m_nCurrentLevel = 0;
			m_Canvas.SetCursorSet(0, 0xFFFFFF, 32, 32, 32, 32);
			m_Canvas.SetCursorSet(1, 0xFFFFFF, 256, 256, 256, 256);
			m_Canvas.SetCursorSet(2, 0x00FF00, 32, 32, 8, 1, TRUE);
			m_Canvas.SetCursorSet(3, 0xF2A072, 32, 32, 128, 1);
			m_Canvas.SetCursorSet(4, 0xF2A072, 32, 32, 128, 1);
			m_Canvas.SetCursorSet(5, 0xA0F230, 16, 16, 1, 1);
			m_Canvas.SetCursorSet(6, 0xF2F230, 16, 16, 16, 16);
			m_Canvas.SetCursorSet(7, 0x529133, 16, 16, 1, 1);
			m_Canvas.SetCursorSet(8, 0xF2A072, 32, 32, 128, 1);

			if ( m_nEditor.LoadFile(m_File, m_Tiles, m_ScroogeTiles) )
			{
				m_nEditor.SetCurrentLevel( m_nCurrentLevel );

				DrawCurrentLevel();
				OnButton(ID_ACTIVELAYOUT_TILESLAYOUT);

				ShowStat();
			}
			else
			{
				ShowError(TEXT("Unable to load this file."));
				m_File.FreeFile();
			}
		}
	}
}

VOID CMainWnd::Ctl_OnOpenFile()
{
	if ( m_File.IsFileLoaded() )
	{
		ShowWarning(TEXT("File is already opened.\nClose the editor and open again to open new file."));
		return;
	}

	CString s = OpenFile(TEXT("Duck Tales 2.nes"), TEXT("NES Files (*.nes)"), TEXT("*.nes"), 0);

	if ( s != TEXT("") )
	{
		OnOpenFile(s);
	}
}

VOID CMainWnd::Ctl_OnSaveFile(BOOL fReqFilename)
{
	if ( !m_File.IsFileLoaded() )
		return;

	m_nEditor.StorFile(m_File);
	m_nTextEditor.StorFile(m_File);
	m_nGraphics.StorGraphics(m_File);

	int fSaveFile = FALSE;
	if ( !fReqFilename )
		fSaveFile = m_File.SaveFile();
	else
	{
		CString sFilename = SaveFile(m_File.Filename(), TEXT("NES files (*.nes)"), TEXT("*.nes"), 0);
		if ( sFilename.GetLength() > 0 )
			fSaveFile = m_File.SaveFile(sFilename);
		else
			return;
	}

	ShowStat();

	if ( fSaveFile )
		ShowErrorF(TEXT("Unable to save file '%s'\nPossible access error."),
			PathFindFileName(m_File));
}

VOID CMainWnd::Ctl_OnEditBlocksEditor()
{
	if ( !m_File.IsFileLoaded() )
		return;
			
	std::vector<NES_BLOCK> vmn, vmj;
	std::vector<NES_TILE> vt;
	std::vector<BYTE> vUsedBlocks;
	BYTE bPal[16] = { 0 }, bPalTiles[256] = { 0 }, bBgTiles[0x0C] = { 0 };
	m_nEditor.Get_LevelBlocks(bPal, bPalTiles, vt, vmn, vmj);
	m_nEditor.Get_BgTiles(bBgTiles);
	m_nEditor.Get_BlocksUsing(vUsedBlocks);

	CBlockEditorDlg dlg(GetInstance(), vUsedBlocks, m_File.IsExtended());
	dlg.Let_Blocks(bPal, bPalTiles, vt, vmn, vmj, bBgTiles);
			
	if ( dlg.Show(this) )
	{
		dlg.Get_Blocks(bPal, bPalTiles, vt, vmn, vmj, bBgTiles);
				
		m_nEditor.Let_LevelBlocks(bPal, bPalTiles, vt, vmn, vmj);
		m_nEditor.Let_BgTiles(bBgTiles);

		DrawCurrentLevel();
	}
}

VOID CMainWnd::Ctl_ConvertToUOROM()
{
	if ( !m_File.IsFileLoaded() )
		return;

	if ( Confirm(TEXT("Are you sure to convert this ROM to UOROM mapper?")) )
	{
		if ( !m_nEditor.SetExtendedNes() )
			ShowError(TEXT("Failed to convert ROM. Its already converted?"));
		else
		{
			m_pMenu->EnableItems(!m_File.IsExtended(), ID_EDIT_CONVERTTOUOROM, 0);
			m_tb.EnableAll(!m_File.IsExtended(), ID_EDIT_CONVERTTOUOROM, 0);
			DrawCurrentLevel();

			if ( m_Canvas.GetActiveCursor() == 5 )
				m_dlgLeftPane.EnableItem(TRUE, ID_REMOVEALLSLOTS);
		}
	}
}

VOID CMainWnd::Ctl_OnSelectLevel()
{
	if ( m_File.IsFileLoaded() )
	{
		std::vector<CString> vlNames;
		for(size_t n = 0; n < NES_COUNT_LEVELS; ++n)
		{
			CString str;
			m_nEditor.Get_LevelName(n, str);
			vlNames.push_back(str);
		}

		CChooseLevelDlg dlg(GetInstance());
		dlg.Let_LevelNames(vlNames);

		if ( dlg.Show(this, m_nCurrentLevel) )
		{
			m_nCurrentLevel = dlg.GetLevel();
			m_nEditor.SetCurrentLevel(m_nCurrentLevel);
			DrawCurrentLevel();

			BOOL fCanBeExtended = m_nEditor.Get_ExtendedLevel();

			m_dlgLeftPane.EnableItem(ID_EDIT_CREATENEWLEVEL, fCanBeExtended);
			m_pMenu->EnableItems(fCanBeExtended, ID_EDIT_CREATENEWLEVEL, 0);
			m_tb.EnableAll(fCanBeExtended, ID_EDIT_CREATENEWLEVEL, 0);

			NES_BRIDGE nb = { 0 };
			BOOL fActiveBridge = m_nEditor.Get_LevelBridge(nb);
			m_pMenu->EnableItems(fActiveBridge, ID_ACTIVELAYERBRIDGE);
			m_tb.EnableAll(fActiveBridge, ID_ACTIVELAYERBRIDGE);
			
			if ( !fActiveBridge && m_Canvas.GetActiveCursor() == 6 )
				OnButton(ID_ACTIVELAYOUT_TILESLAYOUT);

			m_wCatchedItem = 0xFFFF;

			ShowStat();
		}
	}
}

VOID CMainWnd::Ctl_OnShowTileset()
{
	if ( m_File.IsFileLoaded() )
	{
		std::vector<NES_BLOCK> vmn, vmj;
		std::vector<NES_TILE> vt;
		BYTE bPal[16] = { 0 }, bPalTiles[64] = { 0 }, bBgTiles[0x0C] = { 0 };
		m_nEditor.Get_LevelBlocks(bPal, bPalTiles, vt, vmn, vmj);

		CTileSetDlg dlg(GetInstance(), m_nEditor.GetTileSet(), vmn, bPalTiles);
		dlg.Show(this);
	}
}

VOID CMainWnd::Ctl_OnCreateLevel()
{
	if ( m_nEditor.Get_ExtendedLevel() )
	{
		if ( Confirm(
			TEXT("WARNING!\n")
			TEXT("All objects and all data from the level will be removed!\n")
			TEXT("This operation cannot be undone.\nDo you want to continue?\n")
			TEXT("\nIf you're agree with this, then select level in the following dialog after this message\n")
			TEXT("which new level will based on.")
				)
			)
		{
			std::vector<CString> vlNames;
			for(size_t n = 0; n < NES_COUNT_LEVELS; ++n)
			{
				if ( n == m_nCurrentLevel )
					continue;

				CString str;
				m_nEditor.Get_LevelName(n, str);
				vlNames.push_back(str);
			}

			CChooseLevelDlg dlg(GetInstance());
			dlg.Let_LevelNames(vlNames);

			if ( dlg.Show(this, 0) )
			{
				BYTE bLevel = 0;
						
				if ( m_nEditor.Let_ExtendedLevel( dlg.GetLevel() ) )
				{
					m_pMenu->EnableItems(FALSE, ID_EDIT_CREATENEWLEVEL, 0);
					m_tb.EnableAll(FALSE, ID_EDIT_CREATENEWLEVEL);
					m_dlgLeftPane.EnableItem(FALSE, ID_EDIT_CREATENEWLEVEL);

					DrawCurrentLevel();
				}
				else
					ShowError(TEXT("Unable to create new level"));
			}
		}
	}
}

VOID CMainWnd::Ctl_OnTextEditor()
{
	if ( !m_File.IsFileLoaded() )
		return;

	CNesTextEditorDlg dlg(GetInstance(), m_File.IsExtended());
	dlg.Let_Editor(m_nTextEditor);

	if ( dlg.Show(this) )
		dlg.Get_Editor(m_nTextEditor);
}

VOID CMainWnd::Ctl_OnAbout()
{
	CAboutDlg dlg(GetInstance());
	dlg.Show(this);
}

VOID CMainWnd::Ctl_OnQuit()
{
	Close(0);
}

VOID CMainWnd::Ctl_OnEditTilesEditor()
{
	if ( !m_File.IsFileLoaded() )
		return;
			
	std::vector<NES_BLOCK> vmn, vmj;
	std::vector<NES_TILE> vt_unused;
	std::vector<BYTE> vTilesUsed;
	CNesTiles & vTiles = m_nEditor.GetTileSet();
	BYTE bPal[16] = { 0 }, bPalTiles[256] = { 0 }, bBgTiles[0x0C] = { 0 };
	m_nEditor.Get_LevelBlocks(bPal, bPalTiles, vt_unused, vmn, vmj);
	m_nEditor.Get_BgTiles(bBgTiles);
	m_nEditor.Get_TilesUsing(vTilesUsed);

	CTilesEditDlg dlg(GetInstance(), vTilesUsed);
	dlg.Let_Blocks(bPal, bPalTiles, vTiles, vmn, vmj, bBgTiles);
			
	if ( dlg.Show(this) )
	{
		dlg.Get_Blocks(bPal, bPalTiles, vTiles, vmn, vmj, bBgTiles);
		m_nEditor.Let_LevelTiles(vmn, bPalTiles);

		DrawCurrentLevel();
	}		
}

VOID CMainWnd::Ctl_OnChangeTilesBank()
{
	if ( !m_File.IsFileLoaded() )
		return;

	if ( !m_File.IsExtended() )
	{
		ShowError(TEXT("This option is unavailable in UNROM mode."));
		return;
	}

	BYTE bTilesBanks[NES_COUNT_LEVELS] = { 0 };
	m_nEditor.Get_TileBankInfo(bTilesBanks);

	CTilesBanksDlg dlg(GetInstance());
	dlg.Let_Info(bTilesBanks);

	if ( dlg.Show(this) )
	{
		BYTE nBank; BOOL f;
		dlg.Get_Info(nBank, f);

		if ( m_nEditor.Let_TileBankInfo(nBank, f) )
			DrawCurrentLevel();
	}
}

VOID CMainWnd::Ctl_OnActivateLayer(USHORT uId)
{
	NES_BRIDGE nb = { 0 };
	BOOL fActiveBridge = m_nEditor.Get_LevelBridge(nb);

	m_pMenu->EnableItems(TRUE, ID_ACTIVELAYOUT_TILESLAYOUT, ID_ACTIVELAYOUT_ROOMSLAYOUT, ID_ACTIVELAYOUT_OBJECTSLAYOUT, 
		ID_ACTIVELAYER_SCROOGELAYER, ID_ACTIVELAYER_CHECKPOINTLAYER, 
		ID_ACTIVELAYER_HIDDENSLOTSLAYER, ID_ACTIVELAYER_MUSECRETLAYER, ID_ACTIVELAYER_BOSSRESPAWNLAYER, 0);

	m_tb.EnableAll(TRUE, ID_ACTIVELAYOUT_TILESLAYOUT, ID_ACTIVELAYOUT_ROOMSLAYOUT, ID_ACTIVELAYOUT_OBJECTSLAYOUT, 
		ID_ACTIVELAYER_SCROOGELAYER, ID_ACTIVELAYER_CHECKPOINTLAYER, 
		ID_ACTIVELAYER_HIDDENSLOTSLAYER, ID_ACTIVELAYER_MUSECRETLAYER, ID_ACTIVELAYER_BOSSRESPAWNLAYER, 0);

	// bridge commands
	m_pMenu->EnableItems(fActiveBridge, ID_ACTIVELAYERBRIDGE, 0);
	m_tb.EnableAll(fActiveBridge, ID_ACTIVELAYERBRIDGE, 0);
	
	if ( !fActiveBridge && ID_ACTIVELAYERBRIDGE == uId )
		return;

	m_pMenu->EnableItems(FALSE, uId, 0);
	m_tb.EnableAll(FALSE, uId, 0);
			
	m_dlgLeftPane.SelectLayer(uId);

	m_dlgLeftPane.EnableItem(FALSE, ID_REMOVEALLSLOTS);

	m_wCatchedItem = -1;

	INT oldLayerId = m_Canvas.GetActiveCursor();
	INT oldLayerCtlId = uId;

	switch(uId)
	{
	case ID_ACTIVELAYOUT_TILESLAYOUT: m_Canvas.SetActiveCursor(0); break;
	case ID_ACTIVELAYOUT_ROOMSLAYOUT: m_Canvas.SetActiveCursor(1); break;
	case ID_ACTIVELAYOUT_OBJECTSLAYOUT: m_Canvas.SetActiveCursor(2); break;
	case ID_ACTIVELAYER_SCROOGELAYER: m_Canvas.SetActiveCursor(3); break;
	case ID_ACTIVELAYER_CHECKPOINTLAYER: m_Canvas.SetActiveCursor(4); break;
	case ID_ACTIVELAYERBRIDGE: m_Canvas.SetActiveCursor(6); break;
	case ID_ACTIVELAYER_MUSECRETLAYER: m_Canvas.SetActiveCursor(7); break;
	case ID_ACTIVELAYER_BOSSRESPAWNLAYER: m_Canvas.SetActiveCursor(8); break;
	case ID_ACTIVELAYER_HIDDENSLOTSLAYER:
		{
			m_Canvas.SetActiveCursor(5);

			CSelSlotDlg dlg(GetInstance());
			dlg.Let_Selected(m_wCatchedSlot);
					
			if ( dlg.Show(this) )
			{
				dlg.Get_Selected(m_wCatchedSlot);
				if ( m_File.IsExtended() )
					m_dlgLeftPane.EnableItem(TRUE, ID_REMOVEALLSLOTS);
			}
			else if ( oldLayerId != 5 )
			{
				switch(oldLayerId)
				{
				case 0: OnButton(ID_ACTIVELAYOUT_TILESLAYOUT); break;
				case 1: OnButton(ID_ACTIVELAYOUT_ROOMSLAYOUT); break;
				case 2: OnButton(ID_ACTIVELAYOUT_OBJECTSLAYOUT); break;
				case 3: OnButton(ID_ACTIVELAYER_SCROOGELAYER); break;
				case 4: OnButton(ID_ACTIVELAYER_CHECKPOINTLAYER); break;
				case 6: OnButton(ID_ACTIVELAYERBRIDGE); break;
				case 7: OnButton(ID_ACTIVELAYER_MUSECRETLAYER); break;
				case 8: OnButton(ID_ACTIVELAYER_BOSSRESPAWNLAYER); break;
				}
			}

			break;
		}
	}

	INT newLayerId = m_Canvas.GetActiveCursor();

	if ( oldLayerId == 5 && newLayerId != 5 || newLayerId == 5 || 
		newLayerId == 6 || oldLayerId == 6 && newLayerId != 6 ||
		newLayerId == 7 || oldLayerId == 7 && newLayerId != 7 )
		DrawCurrentLevel(0xFF);

	ShowStat();
}

VOID CMainWnd::Ctl_OnShowDialog()
{
	if ( !m_File.IsFileLoaded() || m_Canvas.GetActiveCursor() != 2 )
		return;

	POINT pt = m_Canvas.GetActiveCursorPos();

	BYTE nRoomY = LOBYTE(pt.y / ( 32 * 8 )), nRoomX = LOBYTE( pt.x / 32 );
	POINT opt = { pt.x - INT(nRoomX) * 32, pt.y - INT(nRoomY) * 256 };

	WORD id = m_nEditor.GetObjectFromPos(nRoomX, nRoomY, LOBYTE(opt.x), LOBYTE(opt.y), opt, TRUE);

	WORD char_id = 0;
	switch(id)
	{
	case 0x3c: char_id = 0x10; break;
	case 0x3d: char_id = 0x12; break;
	case 0x3e: char_id = 0x14; break;
	case 0x3f: char_id = 0x18; break;
	case 0x41: char_id = 0x1a; break;
	case 0x42: char_id = 0x1c; break;
	case 0x40: char_id = 0x1e; break;
	}

	if ( !char_id )
		return;


	CNesTextEditorDlg dlg(GetInstance(), m_File.IsExtended());
	dlg.Let_Editor(m_nTextEditor);
	dlg.Let_Selection(LOBYTE(m_nCurrentLevel), LOBYTE(char_id));

	if ( dlg.Show(this) )
		dlg.Get_Editor(m_nTextEditor);

}

VOID CMainWnd::Ctl_OnCanvasMask(USHORT uId)
{
#define SETFLAG(b, f)	b |= (f)
#define UNSETFLAG(b, f) b &= ~(f)
#define ISETFLAG(b, f) ( (f) == ( b & (f) ) )

	std::map<USHORT, BYTE> mFlags;
	mFlags[ID_CANVAS_SHOWTILESMASK] = NEDF_SHOWTILEMASK;
	mFlags[ID_CANVAS_SHOWSPECIALOBJECTS] = NEDF_SHOWSPECOBJ;
	mFlags[ID_CANVAS_SHOWROOMSBORDERS] = NEDF_SHOWROOMBORDERS;
	mFlags[ID_CANVAS_SHOWOUTSIDEOBJECTS] = NEDF_SHOWOUTSIDEOBJECTS;

	switch(uId)
	{
	case ID_CANVAS_SHOWTILESMASK:
	case ID_CANVAS_SHOWSPECIALOBJECTS:
	case ID_CANVAS_SHOWROOMSBORDERS:
	case ID_CANVAS_SHOWOUTSIDEOBJECTS:
		{
			BYTE bFlag = mFlags[uId];
			if ( !ISETFLAG(m_bShowMask, bFlag) )
			{
				SETFLAG(m_bShowMask, bFlag);
			}
			else
			{
				UNSETFLAG(m_bShowMask, bFlag);
			}

			if ( m_nEditor.SetDrawMask(m_bShowMask) )
				DrawCurrentLevel();

			for(auto v = mFlags.begin(); v != mFlags.end(); v++)
			{
				if ( ( m_bShowMask & v->second ) == v->second )
					m_pMenu->CheckItems(TRUE, v->first, 0);
				else
					m_pMenu->CheckItems(FALSE, v->first, 0);
			}

			break;
		}
	}

#undef SETFLAG
#undef UNSETFLAG
#undef ISETFLAG
}

VOID CMainWnd::Ctl_OnGameSettings()
{
	if ( !m_File.IsFileLoaded() )
		return;

	CGameSettings sets(GetInstance());
	BYTE bEgyptCode[10] = { 0 };
	BYTE nCosts[3][5] = { 0 };
	WORD wDrops[17] = { 0 };
	NES_MU_SECRET nmu;

	m_nEditor.Get_EgyptSecret(bEgyptCode);
	m_nEditor.Get_MuSecret(nmu);
	m_nEditor.Get_Costs((PBYTE) nCosts);
	m_nEditor.Get_Drops(wDrops);

	sets.Let_EgyptCode(bEgyptCode);
	sets.Let_MuSecret(nmu);
	sets.Let_Costs((PBYTE) nCosts);
	sets.Let_Drops(wDrops);

	if ( sets.Show(this) )
	{
		sets.Get_EgyptCode(bEgyptCode);
		sets.Get_MuSecret(nmu);
		sets.Get_Costs((PBYTE) nCosts);
		sets.Get_Drops(wDrops);

		m_nEditor.Let_EgyptSecret(bEgyptCode);
		m_nEditor.Let_MuSecret(nmu);
		m_nEditor.Let_Costs((PBYTE) nCosts);
		m_nEditor.Let_Drops(wDrops);
	}

	m_nEditor.SetCurrentLevel(m_nCurrentLevel);
	DrawCurrentLevel();
}

VOID CMainWnd::Ctl_OnFlushObjects()
{
	if ( !m_File.IsFileLoaded() || !m_File.IsExtended() )
	{
		ShowError(TEXT("This option is unavailable in UNROM mode."));
		return;
	}


	if ( Confirm(TEXT("Are you sure that you want to delete ALL objects from the level?\n")
				TEXT("It is needed if you want create level with new objects.")) )

	{
		for(BYTE i = 0; i < 6; ++i)
			while(m_nEditor.DeleteObject(i, 0));

		m_nEditor.SetCurrentLevel(m_nCurrentLevel);
		DrawCurrentLevel();
	}
}

VOID CMainWnd::Ctl_OnLevelSettings()
{
	if ( !m_File.IsFileLoaded() )
		return;

	CLevelSettingsDlg dlg(GetInstance());
	BYTE bPal[0x10] = { 0 }, bSound = 0, bBossLife = 0, bBossType = 0;
	CString sLevelName;
	m_nEditor.Get_LevelData(bPal, bSound, bBossLife, bBossType);
	m_nEditor.Get_LevelName(m_nCurrentLevel, sLevelName);
	
	sLevelName = sLevelName.Trim();

	dlg.Let_LevelName(sLevelName);
	dlg.Let_Palette(bPal);
	dlg.Let_Sound(bSound);
	dlg.Let_BossLife(bBossLife);

	if ( dlg.Show(this) )
	{	
		dlg.Get_LevelName(sLevelName);

		dlg.Get_Palette(bPal);
		dlg.Get_Sound(bSound);
		dlg.Get_BossLife(bBossLife);
		m_nEditor.Let_LevelName(m_nCurrentLevel, sLevelName);
		m_nEditor.Let_LevelData(bPal, bSound, bBossLife, bBossType);

		m_nEditor.SetCurrentLevel(m_nCurrentLevel);
		DrawCurrentLevel();
	}
}

VOID CMainWnd::Ctl_OnGraphicsEditor()
{
	if ( !m_File.IsFileLoaded() )
		return;

	
	int level = m_nCurrentLevel, set = 0;

	if ( 2 == m_Canvas.GetActiveCursor() )
	{
		POINT pt = m_Canvas.GetActiveCursorPos();
		BYTE nRoomY = LOBYTE(pt.y / ( 32 * 8 )), nRoomX = LOBYTE( pt.x / 32 );
		POINT opt = { pt.x - INT(nRoomX) * 32, pt.y - INT(nRoomY) * 256 };

		WORD id = m_nEditor.GetObjectFromPos(nRoomX, nRoomY, LOBYTE(opt.x), LOBYTE(opt.y), opt, TRUE);

		if ( id >= 0x9d )
		{
			id -= 0x9d;
			if ( id / 8 + m_nCurrentLevel >= NES_COUNT_LEVELS )
			{
				if ( !m_File.IsExtended() )
				{
					id = 0xffff;
				}
				else
				{
					id = id - 8 * ( NES_COUNT_LEVELS - m_nCurrentLevel );
					if ( id / 8 >= NES_COUNT_LEVELS )
						id = 0xffff;
					else
						id += 0x9d;
				}
			}
			else
				id += 0x9d + m_nCurrentLevel * 8;

			if ( id != 0xffff )
			{
				level = ( id - 0x9d ) / 8;
				set = ( id - 0x9d ) % 8;
			}
		}
	}

	BYTE bPal[16] = { 0 };
	m_nEditor.Get_PaletteEx(bPal);

	CGfxEditorDlg dlg(GetInstance());
	dlg.Let_Editor(level, set, m_nGraphics, bPal, m_File.IsExtended());

	if ( dlg.Show(this) )
	{
		dlg.Get_Editor( m_nGraphics );
	}
}

VOID CMainWnd::Ctl_OnRun()
{
	if ( m_File.IsFileLoaded() )
	{
		ShellExecute(*this, TEXT("open"), m_File, NULL, NULL, SW_SHOW);
	}
}

VOID CMainWnd::Ctl_OnRemoveLevelSlots()
{
	if ( m_File.IsExtended() && Confirm(TEXT("Delete selected slots from the level?")) )
	{
		int i = 0;
		while( m_nEditor.DeleteSlot( m_wCatchedSlot, i++ ) );
		DrawCurrentLevel();
	}
	
}