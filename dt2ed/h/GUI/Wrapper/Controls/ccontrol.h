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

class CControl
{
	HWND			m_hWnd;

protected:
	CControl();
	CControl(HWND hWnd);

public:
	void			AssignWindow(HWND hWnd);
	void			AssignControl(HWND hWnd, USHORT uCtlId);
	INT_PTR			cSendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = NULL);
	HWND			GetWindowHandle();
	void			Enable(BOOL bEnabled);
	BOOL			Enable();
	BOOL			Visible();
	void			Visible(BOOL bVisible);
	operator		HWND();
};