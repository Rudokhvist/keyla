#include "../common.h"
#include "GuiHotKey.h"
#include "../res/resource.h"

/* static */ GuiHotKey * GuiHotKey::ActiveInstance = 0;

/* static */ HHOOK GuiHotKey::KeyboardHook = 0;
/* static */ unsigned int GuiHotKey::Modifiers = 0;

/* static */ Menu GuiHotKey::ContextMenu(MAKEINTRESOURCE(IDM_HOTKEYEDIT));

GuiHotKey::GuiHotKey() : m_lastInitializedHwnd(0) {
}

void GuiHotKey::setHotKey(const HotKey & hotKey) {
	m_hotKey = hotKey;
    SetWindowText(GetHwnd(), m_hotKey.text().c_str());
}

void GuiHotKey::setHotKey(unsigned int vk, unsigned int modifiers) {
	m_hotKey.set(vk, modifiers);
    SetWindowText(GetHwnd(), m_hotKey.text().c_str());
}

void GuiHotKey::clearHotKey() {
	m_hotKey.clear();
    SetWindowText(GetHwnd(), m_hotKey.text().c_str());
}

const HotKey & GuiHotKey::hotKey() const {
	return m_hotKey;
}

/* virtual */ void GuiHotKey::PreCreate(CREATESTRUCT & cs) {
	cs.lpszClass = TEXT("EDIT");
	cs.style = WS_CHILD | ES_READONLY;
}

/* virtual */ LRESULT GuiHotKey::WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {

	if (m_lastInitializedHwnd != window) {
		m_lastInitializedHwnd = window;
		initialize();
	}

	switch (message) {

		case WM_CONTEXTMENU:
			showContextMenu(ContextMenu, GetHwnd());
			return 0;

		case WM_COMMAND: {
			unsigned int code = HIWORD(wparam);
			unsigned int id = LOWORD(wparam);
			HWND hwnd = reinterpret_cast<HWND>(lparam);

			// Messages from the (context) menu
			if (hwnd == 0 && code == 0)
				switch (id) {
					case ID_HOTKEYEDIT_CLEAR:
						clearHotKey();
						return 0;
				}

			// We did not recognize the command
			return 0;
		}

		// When the control is focused, run the keyboard hook. It must be WH_KEYBOARD_LL
		// in order to catch all shortcuts, including ones used by Windows (e.g., Win-D)

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

/* static */ LRESULT CALLBACK GuiHotKey::keyboardHook(int code, WPARAM wparam, LPARAM lparam) {

	// MSDN says one must do nothing when code is < 0
	if (code < 0)
		return CallNextHookEx(0, code, wparam, lparam);

	// There is no attached control somehow. Do nothing.
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
			// The key is not in the list above
			ActiveInstance->setHotKey(vk, Modifiers | (extended ? HotKey::Extended : 0));
			return TRUE;
		}

		// The key is in the list above
		ActiveInstance->setHotKey(0, Modifiers);

	} else {

			 if (vk == VK_LCONTROL) Modifiers &= ~HotKey::LControl;
		else if (vk == VK_LMENU)    Modifiers &= ~HotKey::LAlt;
		else if (vk == VK_LSHIFT)   Modifiers &= ~HotKey::LShift;
		else if (vk == VK_RCONTROL) Modifiers &= ~HotKey::RControl;
		else if (vk == VK_RMENU)    Modifiers &= ~HotKey::RAlt;
		else if (vk == VK_RSHIFT)   Modifiers &= ~HotKey::RShift;

		else {
			// The key is not in the list above
			return TRUE;
		}

		// The key is in the list above
		if (ActiveInstance->hotKey().vk() == 0)
			ActiveInstance->setHotKey(0, Modifiers);			
	}

	// The key event was handled
	return TRUE;
}

void GuiHotKey::initialize() {
	
	HCURSOR cursor = LoadCursor(0, IDC_ARROW);
	#pragma warning(push)
	#pragma warning(disable: 4244) // warning C4244: 'argument' : conversion from 'LONG_PTR' to 'LONG', possible loss of data
	SetClassLongPtr(GetHwnd(), GCLP_HCURSOR, reinterpret_cast<LONG_PTR>(cursor));
	#pragma warning(pop)
}