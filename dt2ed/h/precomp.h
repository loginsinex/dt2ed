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

#include "targetver.h"

#pragma comment(linker, \
    "\"/manifestdependency:type='Win32' "\
    "name='Microsoft.Windows.Common-Controls' "\
    "version='6.0.0.0' "\
    "processorArchitecture='*' "\
    "publicKeyToken='6595b64144ccf1df' "\
    "language='*'\"")


#define WIN32_LEAN_AND_MEAN             // Исключите редко используемые компоненты из заголовков Windows
// Файлы заголовков Windows:
#include <windows.h>
#include <WindowsX.h>
#include <CommCtrl.h>
#include <CommDlg.h>
#include <Uxtheme.h>

// Файлы заголовков C RunTime
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <xstring>
#include <vector>
#include <map>
#include <atlstr.h>

using namespace std;


#ifdef _WIN64

#define DWL_MSGRESULT   0
#define DWL_DLGPROC     4
#define DWL_USER        8
#define GWL_WNDPROC         (-4)
#define GWL_HINSTANCE       (-6)
#define GWL_HWNDPARENT      (-8)
#define GWL_STYLE           (-16)
#define GWL_EXSTYLE         (-20)
#define GWL_USERDATA        (-21)
#define GWL_ID              (-12)

WINUSERAPI
LONG_PTR
WINAPI
SetWindowLongPtrW(
    __in HWND hWnd,
    __in int nIndex,
    __in LONG_PTR dwNewLong);

#undef SetWindowLong
#define SetWindowLong SetWindowLongPtrW

WINUSERAPI
LONG_PTR
WINAPI
GetWindowLongPtrW(
    __in HWND hWnd,
    __in int nIndex);

#undef GetWindowLong
#define GetWindowLong GetWindowLongPtrW
#endif


// TODO: Установите здесь ссылки на дополнительные заголовки, требующиеся для программы
#include "resource.h"

//#ifndef _DEBUG
inline void * __cdecl operator new(size_t size) 
{ 
	PVOID pParam = GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, size);
	
	if ( !pParam )
		FatalAppExit(0, TEXT("Can't allocate memory\n\nProgram will abnormal closed"));
	return (void *) pParam;
}

inline void __cdecl operator delete(void *ptr) 
{ 
	GlobalFree(ptr);
}
//#endif

typedef struct {
  ULONG         i[2];
  ULONG         buf[4];
  unsigned char in[64];
  unsigned char digest[16];
} MD5_CTX;

#define NTIMPORT		extern "C" __declspec(dllimport)

NTIMPORT void WINAPI MD5Init(MD5_CTX*);
NTIMPORT void WINAPI MD5Update(MD5_CTX*, unsigned char* input, unsigned int inlen);
NTIMPORT void WINAPI MD5Final(MD5_CTX*);

#ifndef DEBUG
#undef _tprintf
#define _tprintf(...)	{ }
#endif

#include "GUI\Wrapper\Controls\CWinPaneScroller.h"
#include "GUI\Wrapper\csizeanchor.h"
#include "GUI\Wrapper\cfont.h"
#include "GUI\Wrapper\cdialog.h"
#include "GUI\Wrapper\cpropsheet.h"
#include "GUI\Wrapper\Controls\CMenuWrapper.h"
#include "GUI\Wrapper\Controls\CControl.h"
#include "GUI\Wrapper\Controls\clistview.h"
#include "GUI\Wrapper\Controls\crebar.h"
#include "GUI\Wrapper\Controls\ccombobox.h"
#include "GUI\Wrapper\Controls\cbufview.h"
#include "GUI\Wrapper\Controls\bitmap.h"
#include "GUI\Wrapper\Controls\ctoolbar.h"

#include "clzw.h"

#include "CTilesCreator.h"
// -- NES classes

#include "NES\CNesFile.h"
#include "NES\CNesRooms.h"
#include "NES\CNesTiles.h"
#include "NES\CNesEditor.h"
#include "NES\cnestexteditor.h"
#include "NES\CNesGraphics.h"

#include "GUI\NesCanvas\CNesCanvas.h"
#include "GUI\NesCanvas\cnestileimg.h"

// -- GUI classes

#include "GUI\Dialogs\caboutdlg.h"
#include "GUI\Dialogs\cgfxeditordlg.h"
#include "GUI\Dialogs\clevelsettingsdlg.h"
#include "GUI\Dialogs\cgamesettingsdlg.h"
#include "GUI\Dialogs\cnestexteditordlg.h"
#include "GUI\Dialogs\CObjectInsertDlg.h"
#include "GUI\Dialogs\cselslotdlg.h"
#include "GUI\Dialogs\ctileseditdlg.h"
#include "GUI\Dialogs\cblockeditordlg.h"
#include "GUI\Dialogs\ctilesetdlg.h"
#include "GUI\Dialogs\cchooseleveldlg.h"
#include "GUI\Dialogs\cmainwnd.h"