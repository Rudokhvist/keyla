#include "common.h"
#include "core.h"
#include "layoutHook.h"

static UINT_PTR TimerId = 0;
static HWND LastActiveWindow = 0;
static HKL LastActiveLayout = 0;

static void WINAPI timerProc(HWND, UINT message, UINT_PTR timerId, DWORD time) {
	HWND hwnd = GetForegroundWindow();
	if (hwnd == 0) return;

	HKL layout = GetKeyboardLayout(GetWindowThreadProcessId(hwnd, 0));

	if (hwnd != LastActiveWindow) {
		LastActiveWindow = hwnd;
		LastActiveLayout = layout;		
		core::activeWindowChanged(hwnd, layout);
		return;
	}
	
	if (layout != LastActiveLayout) {		
		if (core::layoutChanged(layout)) {
			// Отменить изменение раскладки
			PostMessage(hwnd, WM_INPUTLANGCHANGEREQUEST, 0, reinterpret_cast<LPARAM>(LastActiveLayout));
			return;
		}
		LastActiveLayout = layout;
		return;
	}
}

namespace layoutHook {

	void create() {
		TimerId = SetTimer(0, 0, 100 /* milliseconds */, timerProc);
		assert(TimerId != 0);
	}

	void destroy() {
		verify(KillTimer(0, TimerId));
	}

}