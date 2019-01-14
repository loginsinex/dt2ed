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

#define ANCHOR_LEFT_TOP		0
#define ANCHOR_LEFT_BOTTOM	1
#define ANCHOR_RIGHT_TOP	2
#define ANCHOR_RIGHT_BOTTOM	3
#define ANCHOR_LEFT			4
#define ANCHOR_RIGHT		5
#define ANCHOR_TOP			6
#define ANCHOR_BOTTOM		7
#define ANCHOR_FULL			8

typedef enum
{
	AnchorNull,
	AnchorHardLinkLeftTop,
	AnchorHardLinkRightBottom,
	AnchorSoftLink
} AnchorType;

typedef struct _tagANCHOR_CORNER
{
	AnchorType		type;
	union
	{
		struct { UINT topleft, bottomright; } hard;
		struct { UINT up, down; } soft;
	} corner;
} ANCHOR_CORNER, *LPANCHOR_CORNER;

typedef struct _tagANCHOR_DATA
{
	ANCHOR_CORNER			left;
	ANCHOR_CORNER			top;
	ANCHOR_CORNER			right;
	ANCHOR_CORNER			bottom;
} ANCHOR_DATA, *LPANCHOR_DATA;

class CSizeAnchor
{
	HWND		m_hParent;
	UINT		m_uId;
	ANCHOR_DATA	m_ad;
	VOID		InternalResize(LPRECT lprc);

public:
	CSizeAnchor(UINT uId, AnchorType left = AnchorNull, AnchorType top = AnchorNull, AnchorType right = AnchorNull, AnchorType bottom = AnchorNull);
	VOID			Initialize(HWND hParent);
	VOID			ResizeByDefer(HDWP hDwp);
	USHORT			ControlId();
};