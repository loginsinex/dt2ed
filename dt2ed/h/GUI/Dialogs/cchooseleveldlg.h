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

#define		IDC_LIST1				0x1001

class CChooseLevelDlg: public CDialog
{
	INT						m_nLevel;
	std::vector<CString>	m_sLevelsNames;
	VOID					OnPreInit(CDialogTemplate & d);
	VOID					OnInit(LPARAM lParam);
	VOID					OnOK();

public:
	CChooseLevelDlg(HINSTANCE hInstance);
	INT				GetLevel();
	VOID			Let_LevelNames(std::vector<CString> & sLevelsNames);
};