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

const BYTE g_pbLocalesHashes[][16] = {
	{ 0xB3, 0x10, 0x75, 0x0C, 0xA3, 0xBB, 0x33, 0x24, 0xD7, 0x7D, 0xBE, 0x2E, 0x49, 0xC5, 0x33, 0x16 }
};


const TCHAR l_sDecodedChars[] =
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


CNesTextEditor::CNesTextEditor()
	: m_wLocale( 0xFFFF )
{

}

CNesTextEditor CNesTextEditor::operator=(CNesTextEditor & ed)
{
	if ( &ed == this )
		return ed;

	this->m_tiles = ed.m_tiles;
	this->m_vDialogList = ed.m_vDialogList;
	this->m_vDialogs = ed.m_vDialogs;
	this->m_vWordList = ed.m_vWordList;

	return *this;
}

BOOL CNesTextEditor::LoadFile(CNesFile & file)
{
	if ( !file.IsFileLoaded() )
		return FALSE;

	m_tiles.FreeTiles();
	m_tiles.PatchTiles(file, file.Bank(nfTextTiles), file.Ptr(nfTextTiles));
	m_tiles.PatchTiles(file, file.Bank(nfTextTiles2), file.Ptr(nfTextTiles2));
		
	BYTE b[0x10] = { 0 };

	if ( m_tiles.Get_Hash(b) )
	{
		for(int i = 0; i < sizeof(g_pbLocalesHashes) / sizeof(g_pbLocalesHashes[0]); ++i)
			if ( !memcmp(g_pbLocalesHashes[i], b, sizeof(b)) )
			{
				m_wLocale = i;
				break;
			}
	}

	if ( 0xFFFF == m_wLocale )
		return FALSE;

	m_vWordList.clear();
	m_vDialogList.clear();
	m_vDialogs.clear();

	INT nBankText = file[nfTextBank];
	INT nBanksPtrsArrayPtr = file.Word(nfTextArrayPtr);

	m_vBankOffsets.push_back(file.Ptr(nfTextBank));
	m_mOffsets[file.Ptr(nfTextArrayPtr)] = file.Word(nfTextArrayPtr) - LOWORD(nBanksPtrsArrayPtr);
	m_mOffsets[file.Ptr(nfTextArray2Ptr)] = file.Word(nfTextArray2Ptr) - LOWORD(nBanksPtrsArrayPtr);

	m_vBankOffsets.push_back(file.Ptr(nfTextBank1));
	m_mOffsets[file.Ptr(nfTextArrayPtr1)] = file.Word(nfTextArrayPtr1) - LOWORD(nBanksPtrsArrayPtr);
	m_mOffsets[file.Ptr(nfTextArray2Ptr1)] = file.Word(nfTextArray2Ptr1) - LOWORD(nBanksPtrsArrayPtr);

	m_vBankOffsets.push_back(file.Ptr(nfTextBank2));
	m_mOffsets[file.Ptr(nfTextArrayPtr2)] = file.Word(nfTextArrayPtr2) - LOWORD(nBanksPtrsArrayPtr);
	m_mOffsets[file.Ptr(nfTextArray2Ptr2)] = file.Word(nfTextArray2Ptr2) - LOWORD(nBanksPtrsArrayPtr);

	m_vBankOffsets.push_back(file.Ptr(nfTextBank3));
	m_mOffsets[file.Ptr(nfTextArrayPtr3)] = file.Word(nfTextArrayPtr3) - LOWORD(nBanksPtrsArrayPtr);
	m_mOffsets[file.Ptr(nfTextArray2Ptr3)] = file.Word(nfTextArray2Ptr3) - LOWORD(nBanksPtrsArrayPtr);

	m_vBankOffsets.push_back(file.Ptr(nfTextBank4));
	m_mOffsets[file.Ptr(nfTextArrayPtr4)] = file.Word(nfTextArrayPtr4) - LOWORD(nBanksPtrsArrayPtr);
	m_mOffsets[file.Ptr(nfTextArray2Ptr4)] = file.Word(nfTextArray2Ptr4) - LOWORD(nBanksPtrsArrayPtr);

	m_vBankOffsets.push_back(file.Ptr(nfTextBank5));
	m_mOffsets[file.Ptr(nfTextArrayPtr5)] = file.Word(nfTextArrayPtr5) - LOWORD(nBanksPtrsArrayPtr);
	m_mOffsets[file.Ptr(nfTextArray2Ptr5)] = file.Word(nfTextArray2Ptr5) - LOWORD(nBanksPtrsArrayPtr);

	// load custom dialogs
	NES_OTHER_DIALOG nod;

	nod.bLoOffset = file.Ptr(nfTextOtherDialog1Lo); nod.bHiOffset = file.Ptr(nfTextOtherDialog1Hi); m_vOtherDialogs.push_back(nod);
	nod.bLoOffset = file.Ptr(nfTextOtherDialog2Lo); nod.bHiOffset = file.Ptr(nfTextOtherDialog2Hi); m_vOtherDialogs.push_back(nod);
	nod.bLoOffset = file.Ptr(nfTextOtherDialog3Lo); nod.bHiOffset = file.Ptr(nfTextOtherDialog3Hi); m_vOtherDialogs.push_back(nod);
	nod.bLoOffset = file.Ptr(nfTextOtherDialog4Lo); nod.bHiOffset = file.Ptr(nfTextOtherDialog4Hi); m_vOtherDialogs.push_back(nod);
	nod.bLoOffset = file.Ptr(nfTextOtherDialog5Lo); nod.bHiOffset = file.Ptr(nfTextOtherDialog5Hi); m_vOtherDialogs.push_back(nod);
	nod.bLoOffset = file.Ptr(nfTextOtherDialog6Lo); nod.bHiOffset = file.Ptr(nfTextOtherDialog6Hi); m_vOtherDialogs.push_back(nod);
	nod.bLoOffset = file.Ptr(nfTextOtherDialog7Lo); nod.bHiOffset = file.Ptr(nfTextOtherDialog7Hi); m_vOtherDialogs.push_back(nod);
	nod.bLoOffset = file.Ptr(nfTextOtherDialog8Lo); nod.bHiOffset = file.Ptr(nfTextOtherDialog8Hi); m_vOtherDialogs.push_back(nod);
	nod.bLoOffset = file.Ptr(nfTextOtherDialog9Lo); nod.bHiOffset = file.Ptr(nfTextOtherDialog9Hi); m_vOtherDialogs.push_back(nod);
	nod.bLoOffset = file.Ptr(nfTextOtherDialog10Lo); nod.bHiOffset = file.Ptr(nfTextOtherDialog10Hi); m_vOtherDialogs.push_back(nod);
	
	INT nBanksPtrsWordsArrayPtr = file.Word(nfTextWordsArrayPtr);

	m_vBankOffsets.push_back(file.Ptr(nfTextWordsBank));
	m_mWordsOffsets[file.Ptr(nfTextWordsArrayPtr)] = file.Word(nfTextWordsArrayPtr) - LOWORD(nBanksPtrsWordsArrayPtr);
	m_mWordsOffsets[file.Ptr(nfTextWordsArrayPtr2)] = file.Word(nfTextWordsArrayPtr2) - LOWORD(nBanksPtrsWordsArrayPtr);

	WORD wDialogList[84] = { 0 };
	WORD wWordsList[123] = { 0 };
	file.GetDataEx(BANK_OFFSET(nBankText) + nBanksPtrsArrayPtr, wDialogList, sizeof(wDialogList));
	file.GetDataEx(BANK_OFFSET(nBankText) + nBanksPtrsWordsArrayPtr, wWordsList, sizeof(wWordsList));
	
	std::map<INT, size_t> mDialogs;

	for(int i = 0; i < sizeof(wWordsList) / sizeof(wWordsList[0]); ++i)
	{
		if ( !i )
			continue;

		std::vector<BYTE> vWord;
		int ptr = wWordsList[i];
		BYTE bChar = 0;

		while( 0xFE > ( bChar = file.ByteEx(BANK_OFFSET(nBankText) + (ptr++)) ) && vWord.size() < 0x100 )
			vWord.push_back(bChar);

		m_vWordList.push_back(vWord);
	}

	for(int i = 0; i < sizeof(wDialogList) / sizeof(wDialogList[0]); ++i)
	{
		NES_DIALOG vDialog;

		int ptr = wDialogList[i];

		if ( mDialogs.find(ptr) != mDialogs.end() )
		{
			m_vDialogs.push_back(mDialogs[ptr]);
			continue;
		}

		m_vDialogs.push_back(m_vDialogList.size());

		mDialogs[ptr] = m_vDialogList.size();

		if ( !ptr )
		{
			vDialog.fNullPtr = TRUE;
			m_vDialogList.push_back(vDialog);
			continue;
		}

		vDialog.fNullPtr = FALSE;

		BYTE bChar = 0;
		while( 0xFF != ( bChar = file.ByteEx(BANK_OFFSET(nBankText) + (ptr++)) ) && vDialog.nti.size() < 0x100 )
		{
			NES_TEXT_ITEM nt;
			nt.val = bChar;
			if ( nt.val < 0x80 )
				nt.nt = ntChar;
			else if ( nt.val > 0x80 && nt.val < 0xFD )
				nt.nt = ntWord;
			else if ( nt.val == 0xFE )				
				nt.nt = ntNewLine;
			else if ( nt.val == 0xFD )
				nt.nt = ntEndOfLine;

			vDialog.nti.push_back(nt);

		}

		m_vDialogList.push_back(vDialog);
	}

	size_t od_count = m_vOtherDialogs.size();
	for(size_t n = 0; n < od_count; ++n)
	{
		NES_OTHER_DIALOG & nod = m_vOtherDialogs[n];
		WORD addr = MAKEWORD( file.Byte(0x10010 + nod.bLoOffset),
								file.Byte(0x10010 + nod.bHiOffset) );

		BYTE bChar = 0;
		while(0xFF != ( bChar = file.ByteEx(BANK_OFFSET(nBankText) + int(addr++)) ) && nod.vData.size() < 0x100 )
			nod.vData.push_back(bChar);

		_tprintf(TEXT("Dialog #%d: Length: %d\n"), n + 1, nod.vData.size());
	}

	return TRUE;
}

BOOL CNesTextEditor::StorFile(CNesFile & file)
{
	if ( !file.IsExtended() )
		return FALSE;

	const BYTE nNewBank = file.Bank(nfTextPtrEx);
	const WORD nNewDlgPtrs = file.Ptr(nfTextPtrEx);

	size_t banks_count = m_vBankOffsets.size();
	for(size_t n = 0; n < banks_count; ++n)
		file.Byte(0x10010 + int(m_vBankOffsets[n]), nNewBank);

	for(std::map<WORD, INT>::iterator v = m_mOffsets.begin(); v != m_mOffsets.end(); v++)
		file.Word(0x10010 + v->first, int(nNewDlgPtrs) + v->second);

	std::map<INT, WORD> mPtrs;
	size_t dlg_count = m_vDialogs.size();
	int begin_ptr = int(nNewDlgPtrs) + sizeof(WORD) * dlg_count;

	for(size_t n = 0; n < dlg_count; ++n)
	{
		if ( mPtrs.find( m_vDialogs[n] ) != mPtrs.end() )
		{
			file.WordEx(BANK_OFFSET(nNewBank) + int(nNewDlgPtrs) + sizeof(WORD) * n, mPtrs[ m_vDialogs[n] ]);
		}
		else
		{
			NES_DIALOG & nd = m_vDialogList[ m_vDialogs[n] ];
			if ( nd.fNullPtr )
			{
				mPtrs[m_vDialogs[n]] = 0;
				file.WordEx(BANK_OFFSET(nNewBank) + int(nNewDlgPtrs) + sizeof(WORD) * n, 0);
			}
			else
			{
				mPtrs[m_vDialogs[n]] = begin_ptr;
				file.WordEx(BANK_OFFSET(nNewBank) + int(nNewDlgPtrs) + sizeof(WORD) * n, begin_ptr);
				size_t bcount = nd.nti.size();
				for(size_t b = 0; b < bcount; ++b)
				{
					NES_TEXT_ITEM & nt = m_vDialogList[ m_vDialogs[n] ].nti[b];

					switch(nt.nt)
					{
					case ntChar:
						{
							file.ByteEx(BANK_OFFSET(nNewBank) + begin_ptr, nt.val);
							begin_ptr++;
							break;
						}
					case ntWord:
						{
							file.ByteEx(BANK_OFFSET(nNewBank) + begin_ptr, nt.val);
							begin_ptr++;
							break;
						}
					case ntNewLine:
						{
							file.ByteEx(BANK_OFFSET(nNewBank) + begin_ptr, 0xFE);
							begin_ptr++;
							break;
						}
					case ntEndOfLine:
						{
							file.ByteEx(BANK_OFFSET(nNewBank) + begin_ptr, 0xFD);
							begin_ptr++;
							break;
						}
					}
				}
				file.ByteEx(BANK_OFFSET(nNewBank) + begin_ptr, 0xFF);
				begin_ptr++;
			}
		}
	}
	file.WordEx(BANK_OFFSET(nNewBank) + begin_ptr, 0xFFFF);
	begin_ptr += 2;


	for(std::map<WORD, INT>::iterator v = m_mWordsOffsets.begin(); v != m_mWordsOffsets.end(); v++)
		file.Word(0x10010 + int(v->first), LOWORD(v->second + begin_ptr));

	int arrptr = begin_ptr;
	file.WordEx(BANK_OFFSET(nNewBank) + arrptr, 0x00DF);	// first word is null?
	arrptr += 2;

	size_t wrds_count = m_vWordList.size();
	begin_ptr = arrptr + sizeof(WORD) * wrds_count;

	for(size_t n = 0; n < wrds_count; ++n)
	{
		file.WordEx(BANK_OFFSET(nNewBank) + arrptr + sizeof(WORD) * n, LOWORD(begin_ptr));

		file.PutDataEx(BANK_OFFSET(nNewBank) + begin_ptr, &m_vWordList[n][0], m_vWordList[n].size());
		begin_ptr += m_vWordList[n].size();
		file.ByteEx(BANK_OFFSET(nNewBank) + (begin_ptr++), 0xFF);
	}

	arrptr = begin_ptr;
	size_t od_count = m_vOtherDialogs.size();
	for(size_t n = 0; n < od_count; ++n)
	{
		NES_OTHER_DIALOG & nod = m_vOtherDialogs[n];
		WORD addr = LOWORD(arrptr);
		
		file.Byte(0x10010 + nod.bLoOffset, LOBYTE(addr));
		file.Byte(0x10010 + nod.bHiOffset, HIBYTE(addr));
		file.PutDataEx(BANK_OFFSET(nNewBank) + arrptr, &nod.vData[0], nod.vData.size());
		arrptr += nod.vData.size();
		file.ByteEx(BANK_OFFSET(nNewBank) + (arrptr++), 0xFF);
	}

	begin_ptr = arrptr;

	for(int fre = begin_ptr; fre <= 0xBFFF; ++fre)
		file.ByteEx(BANK_OFFSET(nNewBank) + fre, 0xFF);

	return TRUE;
}

VOID CNesTextEditor::Get_Tiles(CNesTiles & tiles)
{
	tiles = m_tiles;
}

BOOL CNesTextEditor::Get_WordsList(std::vector<std::vector<BYTE>> & vWords)
{
	vWords = m_vWordList;
	return TRUE;
}

BOOL CNesTextEditor::Let_WordsList(std::vector<std::vector<BYTE>> & vWords)
{
	m_vWordList = vWords;
	return TRUE;
}

BOOL CNesTextEditor::Get_Dialog(INT id, NES_DIALOG & vDialog, BOOL & fStandAlone)
{
	if ( id >= 0 && id < (INT) m_vDialogs.size() )
	{
		fStandAlone = FALSE;
		vDialog = m_vDialogList[m_vDialogs[id]];
		int real_id = m_vDialogs[id], counter = 0;
		for(size_t n = 0; n < m_vDialogs.size(); ++n)
			if ( real_id == m_vDialogs[n] )
				counter++;

		if ( counter <= 1 )
			fStandAlone = TRUE;

		return TRUE;
	}
	return FALSE;
}

BOOL CNesTextEditor::Let_Dialog(INT id, NES_DIALOG & vDialog, BOOL fStandAlone)
{
	if ( id >= 0 && id < (INT) m_vDialogs.size() )
	{
		if ( fStandAlone )
		{
			int real_id = m_vDialogs[id], counter = 0;
			for(size_t n = 0; n < m_vDialogs.size(); ++n)
				if ( real_id == m_vDialogs[n] )
					counter++;

			if ( counter <= 1 )
				fStandAlone = FALSE;
		}

		if ( fStandAlone )
		{
			m_vDialogs[id] = m_vDialogList.size();
			m_vDialogList.push_back(vDialog);
		}
		else
		{
			m_vDialogList[m_vDialogs[id]] = vDialog;
		}
		return TRUE;
	}
	return FALSE;
}

size_t CNesTextEditor::GetDialogsCount()
{
	return m_vDialogs.size();
}

WORD CNesTextEditor::FindChar(TCHAR bChar)
{
	for(int i = 0; i < sizeof(l_sDecodedChars) / sizeof(l_sDecodedChars[0]); ++i)
	{
		if ( bChar == l_sDecodedChars[i] )
			return i;
	}

	return 0xffff;
}