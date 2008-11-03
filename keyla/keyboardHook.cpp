#include "common.h"
#include "core.h"
#include "keyboardHook.h"
#include "util/HotKey.h"

static HHOOK hook = 0;

// Bitwise OR of HotKey::Modifiers that were pressed
static unsigned int Modifiers = 0;

// Keyboard hook procedure. It must be WH_KEYBOARD_LL in order to
// catch all shortcuts, including ones used by Windows (e.g., Win-D)
static LRESULT CALLBACK proc(int code, WPARAM wparam, LPARAM lparam) {
	// MSDN says one must do nothing if code is < 0
	if (code < 0)
		return CallNextHookEx(0, code, wparam, lparam);

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
			// If the key is not in the list above
			if (core::keyPressed(vk, Modifiers | (extended ? HotKey::Extended : 0))) {
				return TRUE;
			} else {
				return CallNextHookEx(0, code, wparam, lparam);
			}
		}

	} else {

			 if (vk == VK_LCONTROL) Modifiers &= ~HotKey::LControl;
		else if (vk == VK_LMENU)    Modifiers &= ~HotKey::LAlt;
		else if (vk == VK_LSHIFT)   Modifiers &= ~HotKey::LShift;
		else if (vk == VK_RCONTROL) Modifiers &= ~HotKey::RControl;
		else if (vk == VK_RMENU)    Modifiers &= ~HotKey::RAlt;
		else if (vk == VK_RSHIFT)   Modifiers &= ~HotKey::RShift;		
	}

	return CallNextHookEx(0, code, wparam, lparam);
}

namespace keyboardHook {
	
	void create() {
		hook = SetWindowsHookEx(WH_KEYBOARD_LL, proc, GetModuleHandle(0), 0);
		assert(hook != 0);
	}

	void destroy() {
		verify(UnhookWindowsHookEx(hook));
	}

}