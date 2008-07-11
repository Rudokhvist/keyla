#include "../common.h"
#include "../layoutList.h"
#include "../settings.h"
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

	// Элемент управления должен быть уж создан
	HWND hwnd = GetHwnd();
	assert(hwnd != 0);

	// Создание делегата
	m_delegate.DestroyWindow();
	m_delegate.Create(*this);

	// Настройка делегата
	HFONT font = reinterpret_cast<HFONT>(SendMessage(GetHwnd(), WM_GETFONT, 0, 0));
	SendMessage(m_delegate, WM_SETFONT, reinterpret_cast<WPARAM>(font), 0);

	// Настройка списка
	ListView_SetExtendedListViewStyle(hwnd, LVS_EX_GRIDLINES);

	// Добавление колонок
	LVCOLUMN lvc = {LVCF_TEXT};
	lvc.pszText = TEXT("Язык");
	ListView_InsertColumn(hwnd, 0, &lvc);
	lvc.pszText = TEXT("Сочетание клавиш");
	ListView_InsertColumn(hwnd, 1, &lvc);

	// Сохранение локальной копии настроек
	m_layoutHotKeys = settings::Settings.layoutHotKeys;

	update();
}

void GuiLayoutList::update() {

	// Элемент управления должен быть уж создан
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

	// Метод может вызываться "для надёжности", то есть когда, возможно, делагат уже скрыт
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
			// По щелчку на ячейке второго столбца начинаем
			// редактирование в этой ячейке сочетания клавиш

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

