
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

CAboutDlg::CAboutDlg(HINSTANCE hInstance) : CDialog(hInstance, MAKEINTRESOURCE(IDD_ABOUTDLG))
{

}

void CAboutDlg::OnOK()
{
	Close( 0 );
}

VOID CAboutDlg::OnInit(LPARAM lParam)
{
	SetCtlText(IDC_STATICABOUT, 
		TEXT("[NES] Duck Tales 2 [U] level editor v1.1.1.\n\n")
		TEXT("Shortcuts:\n * Ctrl + Space - copy/cut item under cursor\n")
		TEXT(" * Space - paste catched data;\n")
		TEXT(" * INSERT - insert new object under cursor;\n")
		TEXT(" * DELETE - delete object under cursor;\n")
		TEXT(" * +/- - change object id under cursor;\n")
		TEXT(" * PgUp/PgDn - change object id under cursor +/-0x20.\n\n")
		TEXT("©Copyright by ALXR 2015\n")
		TEXT("Glitch removal fallthrough between floors code ©Copyright by Ti_\n")
		TEXT("Sprites handler patch ©Copyright by Ti_\n"));
}

INT_PTR CAboutDlg::OnNotify(LPNMHDR lpnm)
{
	switch(lpnm->idFrom)
	{
	case IDC_SYSLINK1:
		{
			if ( lpnm->code == NM_CLICK || lpnm->code == NM_RETURN )
			{
				NMLINK * nml = (NMLINK*) lpnm;
				ShellExecute(*this, TEXT("open"), TEXT("http://www.emu-land.net/forum/index.php/topic,74434.0.html"), NULL, NULL, SW_SHOW);
			}
			break;
		}
	}
	return 0;
}