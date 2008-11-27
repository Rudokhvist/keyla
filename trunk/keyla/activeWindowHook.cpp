#include "common.h"
#include "activeWindowHook.h"
#include "core.h"
#include "layoutHook.h"

static UINT_PTR TimerId = 0;
static HWND LastActiveWindow = 0;

static void WINAPI timerProc(HWND, UINT message, UINT_PTR timerId, DWORD time) {
	HWND hwnd = GetForegroundWindow();

	if (hwnd != LastActiveWindow) {
		LastActiveWindow = hwnd;
		core::activeWindowChanged(hwnd, layoutHook::getLayout(hwnd));
			// If layoutHook::getLayout() fails, we will show our default icon
		return;
	}
}

namespace activeWindowHook {

	void create() {
		TimerId = SetTimer(0, 0, 100 /* milliseconds */, timerProc);
		assert(TimerId != 0);
	}

	void destroy() {
		verify(KillTimer(0, TimerId));
	}

}