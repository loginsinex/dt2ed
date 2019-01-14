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

BOOL CDataReader::IsLoaded()
{
	return ( m_vData.size() > 0 );
}

VOID CDataReader::LoadData(std::vector<BYTE> & vData)
{
	m_ptr = 0;
	m_vData = vData;
}

BYTE CDataReader::PopByte()
{
	if ( !eof() )
		return m_vData[m_ptr++];

	return 0;
}

WORD CDataReader::PopWord()
{
	if ( m_ptr + sizeof(WORD) <= m_vData.size() )
	{
		WORD ret = 0;
		CopyMemory(&ret, &m_vData[m_ptr], sizeof(ret));
		m_ptr += sizeof(WORD);
		return ret;
	}

	return 0;
}

DWORD CDataReader::PopDword()
{
	if ( m_ptr + sizeof(DWORD) <= m_vData.size() )
	{
		DWORD ret = 0;
		CopyMemory(&ret, &m_vData[m_ptr], sizeof(ret));
		m_ptr += sizeof(DWORD);
		return ret;
	}

	return 0;
}

size_t CDataReader::PopData(std::vector<BYTE> & data, size_t size)
{
	if ( m_ptr + size <= m_vData.size() )
	{
		data.clear();
		data.insert(data.begin(), m_vData.begin() + m_ptr, m_vData.begin() + m_ptr + size);
		m_ptr += size;
		return data.size();
	}
	else if ( !eof() )
	{
		data.clear();
		data.insert(data.begin(), m_vData.begin() + m_ptr, m_vData.end());
		m_ptr += data.size();
		return data.size();
	}

	return 0;
}

BOOL CDataReader::eof()
{
	return ( m_ptr >= m_vData.size() );
}

BOOL CDataReader::flush()
{
	m_ptr = 0;
	return ( m_vData.size() > 0 );
}

CTilesCreator::CTilesCreator(HINSTANCE hInstance, LPCTSTR pszResType, LPCTSTR pszRes)
{
	HRSRC hRsrc = FindResource(hInstance, pszRes, pszResType);

	if ( hRsrc )
	{
		std::vector<BYTE> vData;

		vData.insert(vData.begin(), SizeofResource(hInstance, hRsrc), 0);

		HGLOBAL hRes = LoadResource(hInstance, hRsrc);
		if ( hRes )
		{
			PVOID pData = LockResource(hRes);
			if ( pData )
			{
				CopyMemory(&vData[0], pData, vData.size());
			}
			else
				vData.clear();
		}
		else
			vData.clear();

		if ( vData.size() > 0 )
			DecompressData(vData);
	}
}

CTilesCreator::CTilesCreator(LPCTSTR pszFilename)
{
	HANDLE hFile = CreateFile(pszFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if ( INVALID_HANDLE_VALUE != hFile )
	{
		DWORD w = 0;
		std::vector<BYTE> vData;

		LARGE_INTEGER li = { 0 };
		if ( GetFileSizeEx(hFile, &li) && li.QuadPart <= ( 2 << 30 ) )	// 1MB
		{
			vData.insert(vData.begin(), li.LowPart, 0);
			if ( !::ReadFile(hFile, &vData[0], vData.size(), &w, NULL) )
				vData.clear();
		}
		CloseHandle(hFile);

		if ( vData.size() > 0 )
			DecompressData(vData);
	}
}

VOID CTilesCreator::DecompressData(std::vector<BYTE> & in)
{
	std::vector<BYTE> out;

	CLzw lz;
	lz.inflate(&in[0], in.size(), out);

	if ( out.size() > 0 )
	{
		m_vData.LoadData(out);
	}
}

BOOL CTilesCreator::LoadTiles(COLORREF clrTransparent, CBitmap ** pbmp)
{
	if ( !m_vData.IsLoaded() )
		return FALSE;

	int count = 0;
	clrTransparent = XCHGCOLORREF( clrTransparent );

	while( !m_vData.eof() )
	{
		BYTE id = m_vData.PopByte();

		WORD cx = m_vData.PopWord(), cy = m_vData.PopWord();
		BYTE nColors = m_vData.PopByte();
		std::vector<COLORREF> cols;

		for(int c = 0; c < INT(nColors) && !m_vData.eof(); ++c)
			cols.push_back( m_vData.PopDword() );
		
		if ( size_t(nColors) != cols.size() )
			continue;

		size_t bmp_size = size_t(cx) * size_t(cy);
		std::vector<BYTE> bitmap;
		if ( bmp_size == m_vData.PopData(bitmap, bmp_size) )
		{
			count++;
			pbmp[id] = new CBitmap;
			CBitmap & bm = *pbmp[id];

			bm.Create(cx, cy);
			COLORREF * pclr = (COLORREF*) bm.LockRawBits();
			for(size_t n = 0; n < bmp_size; ++n)
			{
				if ( bitmap[n] && bitmap[n] <= cols.size() )
				{
					pclr[n] = cols[bitmap[n] - 1];
					((WRGBQUAD*) &pclr[n])->rgbAlpha = 255;
				}
				else
				{
					((WRGBQUAD*) &pclr[n])->rgbAlpha = 0;
					pclr[n] = clrTransparent;
				}
			}
			/*
			CPngLoader png(bm);
			CString s;
			s.Format(TEXT("c:\\Users\\AGATHA\\Documents\\STUFF\\src\\dt2ed\\Release\\spr\\%02x.png"), id);
			png.SavePng(s);
			*/

		}
		else
			_tprintf(TEXT("WARNING: Bitmap%02X was not loaded. Found %d bytes, expected %d\n"),
					id, bitmap.size(), bmp_size);
	}

	m_vData.flush();

	return ( count > 0 );
}

CTilesImage::CTilesImage(CBitmap & bmImage)
	: m_bmImage( bmImage )
{

}

RECT CTilesImage::GetObjectRect()
{
	COLORREF clrTransparent = *((COLORREF*) m_bmImage.LockRawBits());

	INT cx = m_bmImage.Width(), cy = m_bmImage.Height(), k = 0;
	RECT rc = { cx, cy, -1, -1 };
	COLORREF * pclr = ((COLORREF*) m_bmImage.LockRawBits());

	for(int y = 0; y < cy; ++y)
	{
		k = y * cx;
		for(int x = 0; x < cx; ++x)
		{
			if ( pclr[k] != clrTransparent && x < rc.left )
				rc.left = x;

			if (  pclr[k] != clrTransparent && y < rc.top )
				rc.top = y;

			k++;
		}
	}

	for(int y = cy - 1; y >= 0; --y)
	{
		k = y * cx + cx - 1;
		for(int x = cx - 1; x >= 0; --x)
		{
			if ( pclr[k] != clrTransparent && x > rc.right )
				rc.right = x;

			if ( pclr[k] != clrTransparent && y > rc.bottom )
				rc.bottom = y;

			k--;
		}
	}

	return rc;
}
