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

typedef enum _tagNES_TEXT_ITEM_TYPE
{
	ntChar,
	ntWord,
	ntNewLine,
	ntEndOfLine
} NES_TEXT_ITEM_TYPE, *PNES_TEXT_ITEM_TYPE;

typedef struct _tagNES_OTHER_DIALOG
{
	WORD				bLoOffset;
	WORD				bHiOffset;
	std::vector<BYTE>	vData;
} NES_OTHER_DIALOG, *PNES_OTHER_DIALOG;

typedef struct _tagNES_TEXT_ITEM
{
	NES_TEXT_ITEM_TYPE			nt;
	BYTE						val;
} NES_TEXT_ITEM, *PNES_TEXT_ITEM;

typedef struct _tagNES_DIALOG
{
	BOOL						fNullPtr;
	std::vector<NES_TEXT_ITEM>	nti;
} NES_DIALOG, *PNES_DIALOG;

class CNesTextEditor
{
	std::vector<std::vector<BYTE>>					m_vWordList;
	std::vector<NES_DIALOG>							m_vDialogList;
	std::vector<WORD>								m_vDialogs;
	CNesTiles										m_tiles;
	std::vector<WORD>								m_vBankOffsets;
	std::map<WORD, INT>								m_mOffsets;
	std::map<WORD, INT>								m_mWordsOffsets;
	std::vector<NES_OTHER_DIALOG>					m_vOtherDialogs;
	WORD											m_wLocale;

public:
	CNesTextEditor();
	CNesTextEditor operator=(CNesTextEditor & ed);

	BOOL						LoadFile(CNesFile & file);
	BOOL						StorFile(CNesFile & file);
	BOOL						Get_WordsList(std::vector<std::vector<BYTE>> & vWords);
	BOOL						Let_WordsList(std::vector<std::vector<BYTE>> & vWords);

	BOOL						Get_Dialog(INT id, NES_DIALOG & vDialog, BOOL & fStandAlone);
	BOOL						Let_Dialog(INT id, NES_DIALOG & vDialog, BOOL fStandAlone);

	VOID						Get_Tiles(CNesTiles & tiles);
	size_t						GetDialogsCount();
	WORD						FindChar(TCHAR bChar);
	WORD						GetLocale();
};