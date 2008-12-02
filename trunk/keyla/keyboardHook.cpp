//
// We do not use ::RegisterHotKey due to its limitation.
// For example, it can't distinguish right and left command keys.
//

#include "common.h"
#include "core.h"
#include "keyboardHook.h"
#include "util/HotKey.h"

namespace {
	
HHOOK hook = 0;

// Virtual key currently choosen
unsigned int Vk = 0;

// Bitwise OR of HotKey::Modifiers that were pressed
unsigned int Modifiers = 0;

// Keyboard hook procedure. It must be WH_KEYBOARD_LL in order to
// catch all shortcuts, including ones used by Windows (e.g., Win-D)
LRESULT CALLBACK proc(int code, WPARAM wparam, LPARAM lparam) {

	// See alsp GuiHotKey::keyboardHook
	// TODO: extract common code into a function

	// MSDN says one must do nothing when code is < 0
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
			// The key is not in the list above
			Vk = vk;
			Modifiers |= (extended ? HotKey::Extended : 0);
			if (core::keyPressed(Vk, Modifiers))
				return TRUE;
			else
				return ::CallNextHookEx(0, code, wparam, lparam);
		}

		// The key is in the list above
		if (core::keyPressed(Vk, Modifiers))
			return TRUE;
		else
			return CallNextHookEx(0, code, wparam, lparam);
	}
	else {
		     if (vk == VK_LCONTROL) Modifiers &= ~HotKey::LControl;
		else if (vk == VK_LMENU)    Modifiers &= ~HotKey::LAlt;
		else if (vk == VK_LSHIFT)   Modifiers &= ~HotKey::LShift;
		else if (vk == VK_RCONTROL) Modifiers &= ~HotKey::RControl;
		else if (vk == VK_RMENU)    Modifiers &= ~HotKey::RAlt;
		else if (vk == VK_RSHIFT)   Modifiers &= ~HotKey::RShift;
		else {
			Vk = 0;
			Modifiers &= ~HotKey::Extended;
		}
		return CallNextHookEx(0, code, wparam, lparam);
	}	
}

} // namespace

namespace keyboardHook {
	
	void create() {
		hook = SetWindowsHookEx(WH_KEYBOARD_LL, proc, GetModuleHandle(0), 0);
		assert(hook != 0);
	}

	void destroy() {
		verify(UnhookWindowsHookEx(hook));
	}

}