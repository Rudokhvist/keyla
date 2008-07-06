#include "../common.h"
#include "GuiLayoutList.h"
#include "SettingsDialog.h"
#include "../layoutList.h"

#include <commctrl.h>

using namespace std;

HotKeyEditDelegate::HotKeyEditDelegate() : m_layoutList(0) {
}

HWND HotKeyEditDelegate::Create(LayoutList & layoutList) {
	m_layoutList = &layoutList;
	return HotKeyEdit::Create(layoutList);
}

/* virtual */ LRESULT HotKeyEditDelegate::WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
	switch (message) {
		case WM_KILLFOCUS:
			if (!m_layoutList) {
				assert(false);
				break;
			}
			m_layoutList->delegateDeactivated();
			break;
	}
	return HotKeyEdit::WndProc(window, message, wparam, lparam);
}

LayoutList::LayoutList() : m_settingsDialog(0), m_delegateActive(false), m_delegateRow(0) {
}

void LayoutList::initialize(SettingsDialog & settingsDialog) {
	m_settingsDialog = &settingsDialog;

	// Ёлемент управлени€ должен быть уж создан
	HWND hwnd = GetHwnd();
	assert(hwnd != 0);

	m_delegate.DestroyWindow();
	m_delegate.Create(*this);

	HFONT font = reinterpret_cast<HFONT>(SendMessage(GetHwnd(), WM_GETFONT, 0, 0));
	SendMessage(m_delegate, WM_SETFONT, reinterpret_cast<WPARAM>(font), 0);

	ListView_SetExtendedListViewStyle(hwnd, LVS_EX_GRIDLINES);

	LVCOLUMN lvc = {LVCF_TEXT};
	lvc.pszText = TEXT("язык");
	ListView_InsertColumn(hwnd, 0, &lvc);
	lvc.pszText = TEXT("—очетание клавиш");
	ListView_InsertColumn(hwnd, 1, &lvc);

	update();
}

void LayoutList::update() {

	// Ёлемент управлени€ должен быть уж создан
	HWND hwnd = GetHwnd();
	assert(hwnd != 0);

	for (size_t i = 0; i < layoutList::LayoutList.size(); ++i) {
		HKL layout = layoutList::LayoutList[i];
		tstring layoutLanguage = layoutList::layoutLanguage(layout);
		tstring layoutHotKey = m_settingsDialog->m_settings.layoutHotKeys[i].text();

		LVITEM lvi = {0};
		lvi.mask = LVIF_TEXT;
		lvi.iItem = static_cast<int>(i);
		lvi.pszText = const_cast<LPTSTR>(layoutLanguage.c_str());
		ListView_InsertItem(hwnd, &lvi);

		ListView_SetItemText(hwnd, i, 1, const_cast<LPTSTR>(layoutHotKey.c_str()));
	}

	ListView_SetColumnWidth(hwnd, 0, LVSCW_AUTOSIZE_USEHEADER);
	ListView_SetColumnWidth(hwnd, 1, LVSCW_AUTOSIZE_USEHEADER);
}

void LayoutList::delegateDeactivated() {

	// ћетод может вызыватьс€ "дл€ надЄжности", то есть когда, возможно, делагат уже скрыт
	if (!m_delegateActive) return;

	m_settingsDialog->m_settings.layoutHotKeys[m_delegateRow] = m_delegate.hotKey();
	ListView_SetItemText(GetHwnd(), m_delegateRow, 1, const_cast<LPTSTR>(m_delegate.hotKey().text().c_str()));
	
	verify(ShowWindow(m_delegate, SW_HIDE));
	
	m_delegateActive = false;
	m_delegateRow = 0;
}

/* virtual */ LRESULT LayoutList::WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
	switch (message) {
		case WM_LBUTTONUP: {
			// ѕо двойному щелчку на €чейке второго столбца начинаем
			// редактирование в этой €чейке сочетани€ клавиш

			POINT pt;
			pt.x = LOWORD(lparam);
			pt.y = HIWORD(lparam);

			LVHITTESTINFO lvhti = {pt};
			ListView_SubItemHitTest(window, &lvhti);
			unsigned int row = lvhti.iItem;
			unsigned int col = lvhti.iSubItem;
			if (row == -1 || col != 1)
				break;

			RECT rc;
			ListView_GetSubItemRect(window, row, col, LVIR_LABEL, &rc);
			SetWindowPos(m_delegate, 0, rc.left + 1, rc.top, rc.right - rc.left, rc.bottom - rc.top - 1, SWP_NOZORDER | SWP_SHOWWINDOW);
			
			SetFocus(m_delegate);
			m_delegate.setHotKey(m_settingsDialog->m_settings.layoutHotKeys[row]);

			m_delegateActive = true;			
			m_delegateRow = row;

			return 0;
		}
	}
	return CWnd::WndProc(window, message, wparam, lparam);
}

