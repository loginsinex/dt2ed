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

CNesFileLoader::CNesFileLoader()
{

}

CNesFileLoader::operator LPTSTR()
{
	return (LPTSTR) Filename().GetString();
}

CNesFileLoader::operator LPCTSTR()
{
	return m_sNesFile.GetString();
}

CNesFileLoader::operator CString()
{
	return m_sNesFile;
}


CString CNesFileLoader::Filename()
{
	return m_sNesFile;
}

INT CNesFileLoader::LoadFile(CString sFilename)
{
	if ( !lstrlen(sFilename) )
		return NES_LOADER_ERROR_ACCESS;

	std::vector<BYTE> vData;

	INT ret = 0;

	HANDLE hFile = CreateFile(sFilename, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, NULL, NULL);

	if ( INVALID_HANDLE_VALUE != hFile )
	{
		LARGE_INTEGER li = { 0 };
		if ( GetFileSizeEx( hFile, &li ) && ( li.QuadPart == NES_FILE_SIZE || li.QuadPart == NES_FILE_SIZE_UOROM ) )
		{
			DWORD w = 0;
			vData.insert(vData.begin(), li.LowPart, 0);

			if ( !::ReadFile(hFile, &vData[0], li.LowPart, &w, NULL) || w != li.LowPart )
				ret = NES_LOADER_ERROR_ACCESS;
		}
		else
			ret = NES_LOADER_ERROR_CHECKSUM;

		CloseHandle(hFile);
	}
	else
		ret = NES_LOADER_ERROR_ACCESS;

	if ( !ret )
	{
		if ( ! (ret = ValidateFile(vData)) )
			m_sNesFile = sFilename;
	}

	return ret;
}

INT CNesFileLoader::SaveAsFile(CString sFilename)
{
	if ( !IsFileLoaded() )
		return NES_LOADER_ERROR_ACCESS;

	m_sNesFile = sFilename;
	return SaveFile();
}

INT CNesFileLoader::SaveFile(LPCTSTR pszFilename)
{
	if ( !IsFileLoaded() )
		return NES_LOADER_ERROR_ACCESS;

	if ( !pszFilename )
		pszFilename = m_sNesFile.GetString();

	INT ret = 0;

	HANDLE hFile = CreateFile(pszFilename, GENERIC_WRITE, FILE_SHARE_READ,
		NULL, OPEN_ALWAYS, NULL, NULL);

	if ( INVALID_HANDLE_VALUE != hFile )
	{
		DWORD w = 0;
		
		if ( !::WriteFile(hFile, &m_vData[0], m_vData.size(), &w, NULL) || w != m_vData.size() )
			ret = NES_LOADER_ERROR_ACCESS;

		CloseHandle(hFile);
	}
	else
		ret = NES_LOADER_ERROR_ACCESS;

	if ( !ret )
		m_sNesFile = pszFilename;

	return ret;
}

INT CNesFileLoader::ValidateFile(std::vector<BYTE> & vData)
{
	INT ret = 0;

	if ( vData.size() != NES_FILE_SIZE && vData.size() != NES_FILE_SIZE_UOROM )
		return NES_LOADER_ERROR_CHECKSUM;

	if ( *((PDWORD) &vData[0]) != NES_FILE_SIGNATURE )
		return NES_LOADER_ERROR_CHECKSUM;

	if ( !ret )
		m_vData = vData;

	return ret;
}

INT CNesFileLoader::FreeFile()
{
	if ( !IsFileLoaded() )
		return NES_LOADER_ERROR_ACCESS;

	m_vData.clear();

	return 0;
}

BOOL CNesFileLoader::IsFileLoaded()
{
	return ( m_vData.size() > 0 );
}

BYTE & CNesFileLoader::operator[](int offset)
{
	offset += NES_FILE_EXTENDED_OFFSET(m_vData, offset);
	return m_vData[offset];		// std::array throws exception
}

BYTE & CNesFileLoader::GetByteEx(INT offset)
{
	return m_vData[offset];		// std::array throws exception
}

VOID CNesFileLoader::Byte(INT offset, BYTE w)
{
	offset += NES_FILE_EXTENDED_OFFSET(m_vData, offset);
	if ( offset >= 0 && offset < INT(m_vData.size()) )
		m_vData[offset] = w;
}

VOID CNesFileLoader::Word(INT offset, USHORT w)
{
	offset += NES_FILE_EXTENDED_OFFSET(m_vData, offset);
	if ( offset >= 0 && offset + sizeof(w) - 1 < INT(m_vData.size()) )
		*((PUSHORT) &m_vData[offset]) = w;
}

VOID CNesFileLoader::Long(INT offset, DWORD w)
{
	offset += NES_FILE_EXTENDED_OFFSET(m_vData, offset);
	if ( offset >= 0 && offset + sizeof(w) - 1 < INT(m_vData.size()) )
		*((PDWORD) &m_vData[offset]) = w;
}

BYTE CNesFileLoader::Byte(INT offset)
{
	offset += NES_FILE_EXTENDED_OFFSET(m_vData, offset);
	if ( offset >= 0 && offset < INT(m_vData.size()) )
		return m_vData[offset];

	return 0;
}

USHORT CNesFileLoader::Word(INT offset)
{
	offset += NES_FILE_EXTENDED_OFFSET(m_vData, offset);
	if ( offset >= 0 && offset + sizeof(USHORT) < INT(m_vData.size()) )
		return *((PUSHORT) &m_vData[offset]);

	return 0;
}

DWORD CNesFileLoader::Long(INT offset)
{
	offset += NES_FILE_EXTENDED_OFFSET(m_vData, offset);
	if ( offset >= 0 && offset + sizeof(DWORD) < INT(m_vData.size()) )
		return *((PDWORD) &m_vData[offset]);

	return 0;
}

std::vector<BYTE> & CNesFileLoader::Data(INT offset, std::vector<BYTE> & v, INT size)
{
	v.clear();
	offset += NES_FILE_EXTENDED_OFFSET(m_vData, offset);

	if ( offset >= 0 && offset + size - 1 < INT(m_vData.size()) )
		v.insert(v.begin(), m_vData.begin() + offset, m_vData.begin() + offset + size - 1);

	return v;
}

VOID CNesFileLoader::Data(INT offset, std::vector<BYTE> & v)
{
	offset += NES_FILE_EXTENDED_OFFSET(m_vData, offset);
	if ( offset >= 0 && offset + v.size() - 1 < m_vData.size() )
		CopyMemory(&m_vData[offset], &v[0], v.size());
}

VOID CNesFileLoader::GetData(INT offset, PVOID pData, INT size)
{
	offset += NES_FILE_EXTENDED_OFFSET(m_vData, offset);
	if ( offset >= 0 && offset + size - 1 < INT(m_vData.size()) && !IsBadWritePtr(pData, size))
		CopyMemory(pData, &m_vData[offset], size);
}

VOID CNesFileLoader::PutData(INT offset, PVOID pData, INT size)
{
	offset += NES_FILE_EXTENDED_OFFSET(m_vData, offset);
	if ( offset >= 0 && offset + size - 1 < INT(m_vData.size()) && !IsBadReadPtr(pData, size))
		CopyMemory(&m_vData[offset], pData, size);
}

VOID CNesFileLoader::ByteEx(INT offset, BYTE w)
{
	if ( offset >= 0 && offset < INT(m_vData.size()) )
		m_vData[offset] = w;
}

VOID CNesFileLoader::WordEx(INT offset, USHORT w)
{
	if ( offset >= 0 && offset + sizeof(w) - 1 < INT(m_vData.size()) )
		*((PUSHORT) &m_vData[offset]) = w;
}

VOID CNesFileLoader::LongEx(INT offset, DWORD w)
{
	if ( offset >= 0 && offset + sizeof(w) - 1 < INT(m_vData.size()) )
		*((PDWORD) &m_vData[offset]) = w;
}

BYTE CNesFileLoader::ByteEx(INT offset)
{
	if ( offset >= 0 && offset < INT(m_vData.size()) )
		return m_vData[offset];

	return 0;
}

USHORT CNesFileLoader::WordEx(INT offset)
{
	if ( offset >= 0 && offset + sizeof(USHORT) < INT(m_vData.size()) )
		return *((PUSHORT) &m_vData[offset]);

	return 0;
}

DWORD CNesFileLoader::LongEx(INT offset)
{
	if ( offset >= 0 && offset + sizeof(DWORD) < INT(m_vData.size()) )
		return *((PDWORD) &m_vData[offset]);

	return 0;
}

VOID CNesFileLoader::GetDataEx(INT offset, PVOID pData, INT size)
{
	if ( offset >= 0 && offset + size - 1 < INT(m_vData.size()) && !IsBadWritePtr(pData, size))
		CopyMemory(pData, &m_vData[offset], size);
}

VOID CNesFileLoader::PutDataEx(INT offset, PVOID pData, INT size)
{
	if ( offset >= 0 && offset + size - 1 < INT(m_vData.size()) && !IsBadReadPtr(pData, size))
		CopyMemory(&m_vData[offset], pData, size);
}

BOOL CNesFileLoader::SetFileExtended()
{
	if ( !IsFileLoaded() || NES_FILE_SIZE_UOROM == m_vData.size() )
		return FALSE;

	std::vector<BYTE> vData;
	vData.insert(vData.begin(), NES_FILE_SIZE_UOROM, 0);
	CopyMemory(&vData[0], &m_vData[0], 0x10 + 0x4000 * 7);
	CopyMemory(&vData[0x3C010], &m_vData[0x1C010], 0x4000);
	vData[4] = 0x10;
	
	m_vData = vData;

	return TRUE;
}

BOOL CNesFileLoader::IsExtended()
{
	return ( m_vData.size() == NES_FILE_SIZE_UOROM );
}

BOOL CNesFile::IsValidRequest(NES_PATCH_ENUM pe, NES_DATA_PATCH & pData)
{
	if ( !IsFileLoaded() || size_t(pe) >= m_vPatchData.size() )
		return FALSE;

	NES_DATA & nd = m_vPatchData[pe];

	pData = ( IsExtended() && nd.ndExtended.dType != dtNull
					? 
					nd.ndExtended : nd.nd
			);

	return TRUE;
}

BYTE & CNesFile::operator[](NES_PATCH_ENUM pe)
{
	NES_DATA_PATCH pd;
	if ( IsValidRequest(pe, pd) )
	{
#ifdef DEBUG
		if ( pd.dType != dtByte )
			_tprintf(TEXT("WARNING: Type mismatch. [w] Requested %d, but expected %d type\n"), dtByte, pd.dType);
#endif
		if ( 0x0F == pd.nBank )
			return CNesFileLoader::GetByteEx( (IsExtended() ? 0x30010 : 0x10010 ) + int(pd.ptr));
		else
			return CNesFileLoader::GetByteEx(BANK_OFFSET(pd.nBank) + int(pd.ptr));
	}
#ifdef DEBUG
	else
		_tprintf(TEXT("Unable to find data %s with id #%d\n"), __FUNCTION__ , pe);
#endif
	return m_bNullByte;
}

VOID CNesFile::Byte(NES_PATCH_ENUM pe, BYTE w)
{
	NES_DATA_PATCH pd;
	if ( IsValidRequest(pe, pd) )
	{
#ifdef DEBUG
		if ( pd.dType != dtByte )
			_tprintf(TEXT("WARNING: Type mismatch. [w] Requested %d, but expected %d type\n"), dtByte, pd.dType);
#endif
		if ( 0x0F == pd.nBank )
			CNesFileLoader::Byte(0x10010 + int(pd.ptr), w);
		else
			ByteEx(BANK_OFFSET(pd.nBank) + int(pd.ptr), w);
	}
#ifdef DEBUG
	else
		_tprintf(TEXT("Unable to find data %s with id #%d\n"), __FUNCTION__ , pe);
#endif
}

VOID CNesFile::Word(NES_PATCH_ENUM pe, USHORT w)
{
	NES_DATA_PATCH pd;
	if ( IsValidRequest(pe, pd) )
	{
#ifdef DEBUG
		if ( pd.dType != dtWord )
			_tprintf(TEXT("WARNING: Type mismatch. [w] Requested %d, but expected %d type\n"), dtWord, pd.dType);
#endif
		if ( 0x0F == pd.nBank )
			CNesFileLoader::Word(0x10010 + int(pd.ptr), w);
		else
			WordEx(BANK_OFFSET(pd.nBank) + int(pd.ptr), w);
	}
#ifdef DEBUG
	else
		_tprintf(TEXT("Unable to find data %s with id #%d\n"), __FUNCTION__ , pe);
#endif
}

VOID CNesFile::Long(NES_PATCH_ENUM pe, DWORD w)
{
	NES_DATA_PATCH pd;
	if ( IsValidRequest(pe, pd) )
	{
#ifdef DEBUG
		if ( pd.dType != dtLong )
			_tprintf(TEXT("WARNING: Type mismatch. [w] Requested %d, but expected %d type\n"), dtLong, pd.dType);
#endif
		if ( 0x0F == pd.nBank )
			CNesFileLoader::Long(0x10010 + int(pd.ptr), w);
		else
			LongEx(BANK_OFFSET(pd.nBank) + int(pd.ptr), w);
	}
#ifdef DEBUG
	else
		_tprintf(TEXT("Unable to find data %s with id #%d\n"), __FUNCTION__ , pe);
#endif
}

BYTE CNesFile::Byte(NES_PATCH_ENUM pe)
{
	NES_DATA_PATCH pd;
	if ( IsValidRequest(pe, pd) )
	{
#ifdef DEBUG
		if ( pd.dType != dtByte )
			_tprintf(TEXT("WARNING: Type mismatch. [r] Requested %d, but expected %d type\n"), dtByte, pd.dType);
#endif
		if ( 0x0F == pd.nBank )
			return CNesFileLoader::Byte(0x10010 + int(pd.ptr));
		else
			return ByteEx(BANK_OFFSET(pd.nBank) + int(pd.ptr));
	}
#ifdef DEBUG
	else
		_tprintf(TEXT("Unable to find data %s with id #%d\n"), __FUNCTION__ , pe);
#endif

	return 0;
}

USHORT CNesFile::Word(NES_PATCH_ENUM pe)
{
	NES_DATA_PATCH pd;
	if ( IsValidRequest(pe, pd) )
	{
#ifdef DEBUG
		if ( pd.dType != dtWord )
			_tprintf(TEXT("WARNING: Type mismatch. [r] Requested %d, but expected %d type\n"), dtWord, pd.dType);
#endif
		if ( 0x0F == pd.nBank )
			return CNesFileLoader::Word(0x10010 + int(pd.ptr));
		else
			return WordEx(BANK_OFFSET(pd.nBank) + int(pd.ptr));
	}
#ifdef DEBUG
	else
		_tprintf(TEXT("Unable to find data %s with id #%d\n"), __FUNCTION__ , pe);
#endif

	return 0;
}

DWORD CNesFile::Long(NES_PATCH_ENUM pe)
{
	NES_DATA_PATCH pd;
	if ( IsValidRequest(pe, pd) )
	{
#ifdef DEBUG
		if ( pd.dType != dtLong )
			_tprintf(TEXT("WARNING: Type mismatch. [r] Requested %d, but expected %d type\n"), dtLong, pd.dType);
#endif
		if ( 0x0F == pd.nBank )
			return CNesFileLoader::Long(0x10010 + int(pd.ptr));
		else
			return LongEx(BANK_OFFSET(pd.nBank) + int(pd.ptr));
	}
#ifdef DEBUG
	else
		_tprintf(TEXT("Unable to find data %s with id #%d\n"), __FUNCTION__ , pe);
#endif

	return 0;
}


VOID CNesFile::GetData(NES_PATCH_ENUM pe, PVOID pData, INT size)
{
	NES_DATA_PATCH pd;
	if ( IsValidRequest(pe, pd) )
	{
#ifdef DEBUG
		if ( pd.dType != dtStream )
			_tprintf(TEXT("WARNING: Type mismatch. [r] Requested %d, but expected %d type\n"), dtStream, pd.dType);
#endif
		if ( 0x0F == pd.nBank )
			CNesFileLoader::GetData(0x10010 + int(pd.ptr), pData, size);
		else
			GetDataEx(BANK_OFFSET(pd.nBank) + int(pd.ptr), pData, size);
	}
#ifdef DEBUG
	else
		_tprintf(TEXT("Unable to find data %s with id #%d\n"), __FUNCTION__ , pe);
#endif
}

VOID CNesFile::PutData(NES_PATCH_ENUM pe, PVOID pData, INT size)
{
	NES_DATA_PATCH pd;
	if ( IsValidRequest(pe, pd) )
	{
#ifdef DEBUG
		if ( pd.dType != dtStream )
			_tprintf(TEXT("WARNING: Type mismatch. [w] Requested %d, but expected %d type\n"), dtStream, pd.dType);
#endif
		if ( 0x0F == pd.nBank )
			CNesFileLoader::PutData(0x10010 + int(pd.ptr), pData, size);
		else
			PutDataEx(BANK_OFFSET(pd.nBank) + int(pd.ptr), pData, size);
	}
#ifdef DEBUG
	else
		_tprintf(TEXT("Unable to find data %s with id #%d\n"), __FUNCTION__ , pe);
#endif
}

BOOL CNesFile::PutPatch(NES_PATCH_ENUM pe)
{
	NES_DATA_PATCH pd;
	if ( IsValidRequest(pe, pd) && pd.vData.size() > 0 )
	{
#ifdef DEBUG
		if ( pd.dType != dtStream )
			_tprintf(TEXT("WARNING: Type mismatch. [w] Requested %d, but expected %d type\n"), dtStream, pd.dType);
#endif
		if ( 0x0F == pd.nBank )
			CNesFileLoader::PutData(0x10010 + int(pd.ptr), &pd.vData[0], pd.vData.size());
		else
			PutDataEx(BANK_OFFSET(pd.nBank) + int(pd.ptr), &pd.vData[0], pd.vData.size());

		return TRUE;
	}
#ifdef DEBUG
	else
		_tprintf(TEXT("Unable to find data %s with id #%d\n"), __FUNCTION__ , pe);
#endif
	return FALSE;
}

BYTE CNesFile::Bank(NES_PATCH_ENUM pe)
{
	NES_DATA_PATCH pd;
	if ( IsValidRequest(pe, pd) )
		return pd.nBank;

	return 0xFF;
}

WORD CNesFile::Ptr(NES_PATCH_ENUM pe)
{
	NES_DATA_PATCH pd;
	if ( IsValidRequest(pe, pd) )
		return pd.ptr;

	return 0;
}

BOOL CNesFile::Let_Patcher(std::vector<NES_DATA> & vPatchData)
{
	m_vPatchData = vPatchData;
	return TRUE;
}

BOOL CNesFile::Get_Patcher(std::vector<NES_DATA> & vPatchData)
{
	vPatchData = m_vPatchData;
	return TRUE;
}
