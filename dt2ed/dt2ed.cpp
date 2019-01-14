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
#include <GdiPlus.h>

int DoMainLoop(HWND hWnd, HACCEL hAcc);
HACCEL GetAccelTable(HINSTANCE hInstance, LPTSTR pszResource);

int APIENTRY _tWinMain(HINSTANCE hInst,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	CreateNewDialogClass(hInst, TEXT("DuckTales2Editor"));
	InitBufView(hInst, TEXT("SysLevel32"));

	CMainWnd * wnd = new CMainWnd(hInst);

	HACCEL hAcc = GetAccelTable(hInst, MAKEINTRESOURCE(IDR_ACCEL));
	HWND hWnd = wnd->Create(NULL, (LPARAM) hAcc);

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	MSG msg = { 0 };
	int ret = DoMainLoop(*wnd, hAcc);

	DestroyAcceleratorTable(hAcc);
	DestroyWindow( *wnd );
	delete wnd;

	return ret;
}

int DoMainLoop(HWND hWindow, HACCEL hAcc)
{
	MSG msg = { 0 };
	int ret = 0;
	do
	{
		ret = GetMessage(&msg, NULL, 0, 0);

		if ( -1 == ret || !ret )
			break;


		if ( !TranslateAccelerator(hWindow, hAcc, &msg) 
			&& !IsDialogMessage(hWindow, &msg) ) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	} while(ret);

	return ret;
}

HACCEL GetAccelTable(HINSTANCE hInstance, LPTSTR pszResource)
{
	return LoadAccelerators(hInstance, pszResource);
}

int main(int argc, char ** argv)
{
	return _tWinMain(GetModuleHandle(NULL), NULL, GetCommandLine(), SW_SHOW);
}
