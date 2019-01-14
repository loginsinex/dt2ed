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

VOID CNesDataFile::Let_Data(std::vector<NES_DATA> & vData)
{
	m_vData = vData;
}

VOID CNesDataFile::Get_Data(std::vector<NES_DATA> & vData)
{
	vData = m_vData;
}

BOOL CNesDataFile::SaveTo(LPCTSTR pszFile)
{
	std::vector<BYTE> vStream;
	if ( !Data2Stream(vStream) )
		return FALSE;

	BOOL res = 0;

	HANDLE hFile = CreateFile(pszFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);

	if ( INVALID_HANDLE_VALUE != hFile )
	{
		DWORD w = 0;
		::WriteFile(hFile, &vStream[0], vStream.size(), &w, NULL);
		res = ( w == vStream.size() );
		CloseHandle(hFile);
	}

	return res;
}

BOOL CNesDataFile::OpenFrom(LPCTSTR pszFile)
{
	std::vector<BYTE> vStream;

	BOOL res = 0;

	HANDLE hFile = CreateFile(pszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if ( INVALID_HANDLE_VALUE != hFile )
	{
		LARGE_INTEGER li = { 0 };

		if ( GetFileSizeEx(hFile, &li) && li.QuadPart <= (1 << 20) )	// 1MB
		{
			vStream.insert(vStream.begin(), li.LowPart, 0);

			DWORD w = 0;
			::ReadFile(hFile, &vStream[0], vStream.size(), &w, NULL);
			res = ( w == vStream.size() );
		}
		CloseHandle(hFile);
	}

	if ( res )
		return Stream2Data(vStream);

	return FALSE;
}

BOOL CNesDataFile::OpenFrom(HINSTANCE hInstance, LPCTSTR pszType, LPCTSTR pszName)
{
	HRSRC hRes = FindResource(hInstance, pszName, pszType);
	if ( hRes )
	{
		HGLOBAL hRsrc = LoadResource(hInstance, hRes);
		if ( hRsrc )
		{
			PVOID pData = LockResource(hRsrc);
			if ( pData )
			{
				std::vector<BYTE> vStream;
				vStream.insert(vStream.begin(), SizeofResource(hInstance, hRes), 0);
				CopyMemory(&vStream[0], pData, vStream.size());

				return Stream2Data(vStream);
			}
		}
	}

	return FALSE;
}

BOOL CNesDataFile::Data2Stream(std::vector<BYTE> & vStream)
{
	BYTE w[] = { 'U', 'R', 'P', '>' };

	std::vector<BYTE> istream;
	size_t count = m_vData.size();
	istream.push_back(LOBYTE(m_vData.size()));
	istream.push_back(HIBYTE(m_vData.size()));
	for(size_t n = 0; n < count; ++n)
	{
		NES_DATA & nd = m_vData[n];
				
		istream.push_back(nd.nd.nBank);
		istream.push_back(LOBYTE(nd.nd.dType));
		istream.push_back(LOBYTE(nd.nd.ptr));
		istream.push_back(HIBYTE(nd.nd.ptr));
		istream.push_back(LOBYTE(nd.nd.vData.size()));
		istream.push_back(HIBYTE(nd.nd.vData.size()));
		istream.insert(istream.end(), nd.nd.vData.begin(), nd.nd.vData.end());

		istream.push_back(nd.ndExtended.nBank);
		istream.push_back(LOBYTE(nd.ndExtended.dType));
		istream.push_back(LOBYTE(nd.ndExtended.ptr));
		istream.push_back(HIBYTE(nd.ndExtended.ptr));
		istream.push_back(LOBYTE(nd.ndExtended.vData.size()));
		istream.push_back(HIBYTE(nd.ndExtended.vData.size()));
		istream.insert(istream.end(), nd.ndExtended.vData.begin(), nd.ndExtended.vData.end());

		int len = nd.sName.GetLength();
		for(int i = 0; i < len; ++i)
		{			
			istream.push_back(HIBYTE(nd.sName[i]));
			istream.push_back(LOBYTE(nd.sName[i]));
		}
		istream.push_back(0);
		istream.push_back(0);
	}

	std::vector<BYTE> md5;
	md5.insert(md5.begin(), 16, 0);

	MD5_CTX context;
	MD5Init(&context);
	MD5Update(&context, &istream[0], istream.size());
	MD5Final(&context);

	CopyMemory(&md5[0], context.digest, md5.size());
	istream.insert(istream.begin(), md5.begin(), md5.end());

	std::vector<BYTE> o;
	CLzw lz(FALSE);
	lz.deflate(&istream[0], istream.size(), o);
	
	vStream.insert(vStream.begin(), sizeof(w), 0);
	CopyMemory(&vStream[0], w, sizeof(w));

	vStream.insert(vStream.end(), o.begin(), o.end());

	return TRUE;
}

BOOL CNesDataFile::Stream2Data(std::vector<BYTE> & vStream)
{
	BYTE w[] = { 'U', 'R', 'P', '>' };
	if ( vStream.size() <= sizeof(w) )
		return FALSE;

	if ( memcmp(&vStream[0], w, sizeof(w)) )
		return FALSE;

	std::vector<BYTE> istream;

	CLzw lz;
	lz.inflate(&vStream[sizeof(w)], vStream.size() - sizeof(w), istream);
	if ( istream.size() <= 0x10 )
		return FALSE;


	MD5_CTX context;
	MD5Init(&context);
	MD5Update(&context, &istream[sizeof(context.digest)], istream.size() - sizeof(context.digest));
	MD5Final(&context);

	if ( memcmp(context.digest, &istream[0], sizeof(context.digest)) )
		return FALSE;

	m_vData.clear();

	size_t count = istream.size();
	WORD datasize = 0xffff;

	for(size_t n = sizeof(context.digest); n < count && m_vData.size() < size_t(datasize); ++n)
	{
		if ( datasize == 0xffff )
		{
			datasize = MAKEWORD(istream[n], istream[n + 1]);
			n++;
			continue;
		}

		NES_DATA nd;
		nd.local = 0;
		
		nd.nd.nBank = istream[n++];
		nd.nd.dType = (NES_DATA_TYPE) istream[n++];
		nd.nd.ptr = MAKEWORD(istream[n], istream[n+1]);
		n += sizeof(nd.nd.ptr);

		WORD size = MAKEWORD(istream[n], istream[n+1]);
		n += 2;
		for(WORD i = 0; i < size; ++i)
			nd.nd.vData.push_back(istream[n++]);

		nd.ndExtended.nBank = istream[n++];
		nd.ndExtended.dType = (NES_DATA_TYPE) istream[n++];
		nd.ndExtended.ptr = MAKEWORD(istream[n], istream[n+1]);
		n += sizeof(nd.ndExtended.ptr);

		size = MAKEWORD(istream[n], istream[n+1]);
		n += 2;
		for(WORD i = 0; i < size; ++i)
			nd.ndExtended.vData.push_back(istream[n++]);

		std::wstring sname;
		wchar_t chr = 0;
		while(0 != ( chr = MAKEWORD(istream[n+1], istream[n]) ) )
		{
			sname.push_back(chr);
			n += 2;
		}

		nd.sName = sname.c_str();
		m_vData.push_back(nd);

		n++;
	}

	return TRUE;
}
