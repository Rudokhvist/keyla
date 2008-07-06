#include "../common.h"
#include "HotKeyEdit.h"
#include "../res/resource.h"

/* static */ HotKeyEdit * HotKeyEdit::ActiveInstance = 0;

/* static */ HHOOK HotKeyEdit::KeyboardHook = 0;
/* static */ unsigned int HotKeyEdit::Modifiers = 0;

/* static */ Menu HotKeyEdit::ContextMenu(MAKEINTRESOURCE(IDM_HOTKEYEDIT));

HotKeyEdit::HotKeyEdit() : m_lastInitializedHwnd(0) {
}

void HotKeyEdit::setHotKey(const HotKey & hotKey) {
	m_hotKey = hotKey;
    SetWindowText(m_hotKey.text().c_str());
}

void HotKeyEdit::setHotKey(unsigned int vk, unsigned int modifiers) {
	m_hotKey.set(vk, modifiers);
    SetWindowText(m_hotKey.text().c_str());
}

void HotKeyEdit::clearHotKey() {
	m_hotKey.clear();
    SetWindowText(m_hotKey.text().c_str());
}

const HotKey & HotKeyEdit::hotKey() const {
	return m_hotKey;
}

/* virtual */ void HotKeyEdit::PreCreate(CREATESTRUCT & cs) {
	cs.lpszClass = TEXT("EDIT");
	cs.style = WS_CHILD | ES_READONLY;
}

/* virtual */ LRESULT HotKeyEdit::WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {

	if (m_lastInitializedHwnd != window) {
		m_lastInitializedHwnd = window;
		initialize();
	}

	switch (message) {
		case WM_CONTEXTMENU:
			showContextMenu(ContextMenu, GetHwnd());
			return 0;

		// Когда на элементе управления фокус ввода, должен работать хук на клавиатуру.
		// Выбран тип хука WH_KEYBOARD_LL, поскольку только с его помощью можно отловить
		// такие "системные" сочетания клавиш, как Win-D

		case WM_SETFOCUS:
			ActiveInstance = this;
			Modifiers = 0;
			
			KeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHook, GetModuleHandle(0), 0);
			assert(KeyboardHook != 0);
			return 0;			
		
		case WM_KILLFOCUS:
			verify(UnhookWindowsHookEx(KeyboardHook));
			KeyboardHook = 0;				
		
			ActiveInstance = 0;
			Modifiers = 0;
			return 0;
	}
	return CWnd::WndProc(window, message, wparam, lparam);
}

/* virtual */ BOOL HotKeyEdit::OnCommand(WPARAM wparam, LPARAM lparam) {
	unsigned int code = HIWORD(wparam);
	unsigned int id = LOWORD(wparam);
	HWND hwnd = reinterpret_cast<HWND>(lparam);

	// Сообщения от (контекстного) меню
	if (hwnd == 0 && code == 0)
		switch (id) {
			case ID_HOTKEYEDIT_CLEAR:
				clearHotKey();
				return TRUE;
		}

	return FALSE;
}

/* static */ LRESULT CALLBACK HotKeyEdit::keyboardHook(int code, WPARAM wparam, LPARAM lparam) {

	// В документации сказано, что в случае code < 0 хук должен ничего не делать
	if (code < 0)
		return CallNextHookEx(0, code, wparam, lparam);

	// Некуда записывать информацию, поэтому ничего не делаем
	if (ActiveInstance == 0) {
		assert(false);
		return CallNextHookEx(0, code, wparam, lparam); 
	}

	bool pressed;
	if (wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN)
		pressed = true;
	else if (wparam == WM_KEYUP || wparam == WM_SYSKEYUP)
		pressed = false;
	else {
		assert(false);
		return CallNextHookEx(0, code, wparam, lparam);
	}

	const KBDLLHOOKSTRUCT * p = reinterpret_cast<const KBDLLHOOKSTRUCT *>(lparam);
	assert(p != 0);	

	unsigned int vk = p->vkCode;
	bool extended = ((p->flags & LLKHF_EXTENDED) != 0);

	if (pressed) {

			 if (vk == VK_LCONTROL) Modifiers |= HotKey::LControl;
		else if (vk == VK_LMENU)    Modifiers |= HotKey::LAlt;
		else if (vk == VK_LSHIFT)   Modifiers |= HotKey::LShift;
		else if (vk == VK_RCONTROL) Modifiers |= HotKey::RControl;
		else if (vk == VK_RMENU)    Modifiers |= HotKey::RAlt;
		else if (vk == VK_RSHIFT)   Modifiers |= HotKey::RShift;

		else {
			// Если была нажата другая клавиша
			ActiveInstance->setHotKey(vk, Modifiers | (extended ? HotKey::Extended : 0));
			return TRUE;
		}

		// Если была нажата одна из вышеперечисленных клавиш
		ActiveInstance->setHotKey(0, Modifiers);

	} else {

			 if (vk == VK_LCONTROL) Modifiers &= ~HotKey::LControl;
		else if (vk == VK_LMENU)    Modifiers &= ~HotKey::LAlt;
		else if (vk == VK_LSHIFT)   Modifiers &= ~HotKey::LShift;
		else if (vk == VK_RCONTROL) Modifiers &= ~HotKey::RControl;
		else if (vk == VK_RMENU)    Modifiers &= ~HotKey::RAlt;
		else if (vk == VK_RSHIFT)   Modifiers &= ~HotKey::RShift;

		else {
			// Если была нажата другая клавиша
			return TRUE;
		}

		// Если была нажата одна из вышеперечисленных клавиш
		if (ActiveInstance->hotKey().vk() == 0)
			ActiveInstance->setHotKey(0, Modifiers);			
	}

	return TRUE; // обработали нажатие
}

void HotKeyEdit::initialize() {
	
	HCURSOR cursor = LoadCursor(0, IDC_ARROW);
	#pragma warning(push)
	#pragma warning(disable: 4244) // warning C4244: 'argument' : conversion from 'LONG_PTR' to 'LONG', possible loss of data
	SetClassLongPtr(GetHwnd(), GCLP_HCURSOR, reinterpret_cast<LONG_PTR>(cursor));
	#pragma warning(pop)
}