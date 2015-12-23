//
// We do not use ::RegisterHotKey due to its limitation.
// For example, it can't distinguish right and left command keys.
//

#include "common.h"
#include "core.h"
#include "keyboardHook.h"
#include "util/HotKey.h"
#include "util/HotKeyAutomaton.h"

namespace {
	
HHOOK hook = 0;

class Automaton : public HotKeyAutomaton {
public:
	// Call before postEvent()
	inline void storeParams(int code, WPARAM wparam, LPARAM lparam) {
		m_code = code;
		m_wparam = wparam;
		m_lparam = lparam;
	}

	// Call after postEvent() to get the value to return from the keyboard hook procedure
	inline LRESULT getResult() { return m_ret; }

protected:
	inline /* virtual */ void callbackNoop() {
		m_ret = ::CallNextHookEx(0, m_code, m_wparam, m_lparam);
	}
	inline /* virtual */ void callbackProgress(const HotKey & hotkey) {
		m_ret = ::CallNextHookEx(0, m_code, m_wparam, m_lparam);
	}
	inline /* virtual */ void callbackHotKey(const HotKey & hotkey) {
		
		/* NOTE:
		
		We must not skip ::CallNextHookEx() if a key is being released.
		Otherwise, for example, when Left Ctrl is chosen a shortcut, Windows
		will think it's still pressed when it has been actually pressed and released.

		The only way this function is called when a key is being released
		is that it is a command key and the hotkey contains only command keys.
		So we check hotkey.vk().

		*/

		if (core::keyPressed(hotkey.vk(), hotkey.modifiers()) && hotkey.vk()) 
			m_ret = TRUE;
		else
			m_ret = ::CallNextHookEx(0, m_code, m_wparam, m_lparam);
	}

private:
	int m_code;
	WPARAM m_wparam;
	LPARAM m_lparam;

	LRESULT m_ret;
};
Automaton Autom;

// Keyboard hook procedure. It must be WH_KEYBOARD_LL in order to
// catch all shortcuts, including ones used by Windows (e.g., Win-D)
LRESULT CALLBACK proc(int code, WPARAM wparam, LPARAM lparam) {

	// MSDN says one must do nothing when code is < 0
	if (code < 0)
		return CallNextHookEx(0, code, wparam, lparam);

	Autom.storeParams(code, wparam, lparam);
	Autom.postEvent(wparam, lparam);
	return Autom.getResult();
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

	void resetState() {
		OutputDebugString(L"resetState()\n");
		Autom.initialize();
	}

}