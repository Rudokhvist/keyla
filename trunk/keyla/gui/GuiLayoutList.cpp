#include "../common.h"
#include "../layoutList.h"
#include "../settings.h"
#include "../res/resource.h"
#include "GuiLayoutList.h"
#include "GuiSettingsWindow.h"

#include <commctrl.h>

HotKeyEditDelegate::HotKeyEditDelegate() : m_layoutList(0) {
}

HWND HotKeyEditDelegate::Create(GuiLayoutList & layoutList) {
	m_layoutList = &layoutList;
	return GuiHotKey::Create(layoutList);
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
	return GuiHotKey::WndProc(window, message, wparam, lparam);
}

GuiLayoutList::GuiLayoutList() : m_delegateActive(false), m_delegateRow(0) {
}

void GuiLayoutList::initialize() {

	// Control must be already created
	HWND hwnd = GetHwnd();
	assert(hwnd != 0);

	// Create the delegate
	m_delegate.DestroyWindow();
	m_delegate.Create(*this);

	// Set up the delegate
	HFONT font = reinterpret_cast<HFONT>(SendMessage(GetHwnd(), WM_GETFONT, 0, 0));
	SendMessage(m_delegate, WM_SETFONT, reinterpret_cast<WPARAM>(font), 0);

	// Customize the ListView
	ListView_SetExtendedListViewStyle(hwnd, LVS_EX_GRIDLINES);

	// Now add some columns
	tstring _1 = LoadStringLang(IDS_LANGUAGE);
	tstring _2 = LoadStringLang(IDS_SHORTCUT);
	LVCOLUMN lvc = {LVCF_TEXT};
	lvc.pszText = const_cast<LPTSTR>(_1.c_str());
	ListView_InsertColumn(hwnd, 0, &lvc);
	lvc.pszText = const_cast<LPTSTR>(_2.c_str());
	ListView_InsertColumn(hwnd, 1, &lvc);

	// Save a local copy of Settings to change
	m_layoutHotKeys = settings::Settings.layoutHotKeys;

	update();
}

void GuiLayoutList::update() {

	// Control must be already created
	HWND hwnd = GetHwnd();
	assert(hwnd != 0);

	for (size_t i = 0; i < layoutList::GuiLayoutList.size(); ++i) {
		HKL layout = layoutList::GuiLayoutList[i];
		tstring layoutLanguage = layoutList::layoutLanguage(layout);
		tstring layoutHotKey = m_layoutHotKeys[i].text();

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

void GuiLayoutList::apply() {
	settings::Settings.layoutHotKeys = m_layoutHotKeys;
}

void GuiLayoutList::delegateDeactivated() {

	// This method may be called to ensure that the delegate is not active,
	// even if it is already destroyed. So we need this check
	if (!m_delegateActive) return;

	m_layoutHotKeys[m_delegateRow] = m_delegate.hotKey();
	ListView_SetItemText(GetHwnd(), m_delegateRow, 1, const_cast<LPTSTR>(m_delegate.hotKey().text().c_str()));
	
	verify(ShowWindow(m_delegate, SW_HIDE));
	
	m_delegateActive = false;
	m_delegateRow = 0;
}

/* virtual */ LRESULT GuiLayoutList::WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
	switch (message) {
		case WM_LBUTTONUP: {
			// When a cell in the second column is clicked, edit the shortcut in it

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
			m_delegate.setHotKey(m_layoutHotKeys[row]);

			m_delegateActive = true;			
			m_delegateRow = row;

			return 0;
		}
	}
	return CWnd::WndProc(window, message, wparam, lparam);
}

