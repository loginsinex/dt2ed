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

class CPropSheet
{
	HINSTANCE				m_hInstance;
	HWND					m_hWnd;				// this field will set by CDialog::PropSheetDialogProc !!!
	BOOL					m_fCreated;
	BOOL					m_fSetToClose;
	vector<PROPSHEETPAGE>	m_vPSList;
	DWORD					m_dwFlags;
	CDialog	*				m_pParent;

	void					SetFlag(DWORD dwFlag, BOOL fSet);
	INT_PTR					pSendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0);
	CDialog *				WindowToDialog(HWND hWnd);
	HWND					DialogToWindow(CDialog * pDialog);
	friend INT_PTR CALLBACK CDialog::PropSheetDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:

	CPropSheet(HINSTANCE hInstance);
	~CPropSheet();

	BOOL			AddSheet(CDialog * pDialog, LPARAM lParam = 0, LPTSTR pszTitle = NULL, BOOL fForceCreate = FALSE);
	INT_PTR			Show(CDialog * pParent, LPARAM lParam = 0, LPTSTR pszCaption = NULL, DWORD dwFirstPage = 0);
	CDialog *		GetParent();
	HWND			GetWindowHandle();

	VOID			FApply(BOOL fApply);
	VOID			FAppendTitle(BOOL fAppendTitle);

	VOID			EChanged(BOOL fChanged, CDialog * pCallee);
	VOID			ESetClose();
	BOOL			EGetClose();
	HWND			ETabCtl();
	VOID			EDoCommand(UINT uButton);
	VOID			ENeedReboot();
	VOID			ENeedRestart();
	VOID			ETitle(LPTSTR lpszTitle, BOOL fPropertiesForFlag = FALSE);
	CDialog *		EGetPage(INT iIndex);
	INT				EGetIndex(CDialog * pDialog);
	CDialog *		EGetCurrentPage();
	INT				EGetCurrentIndex();
	VOID			ESetCurrentPage(INT iIndex);
	VOID			ESetCurrentPage(CDialog * pDialog);
};