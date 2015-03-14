#include "common.h"
#include <Wtsapi32.h>
// Wtsapi32.h doesn't damn help.
#define WM_WTSSESSION_CHANGE	0x02B1
#define WTS_SESSION_LOCK 7

#include "core.h"
#include "layoutHook.h"
#include "mainWindow.h"
#include "../layoutHookDll/layoutHookDll.h"

namespace {

unsigned int LayoutChangedMessage = 0;
unsigned int SetLayoutMessage = 0;

HHOOK hook = 0;

bool messageHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam, LRESULT * ret) {
	if (message == LayoutChangedMessage) {
		core::layoutChanged(reinterpret_cast<HWND>(wparam), reinterpret_cast<HKL>(lparam));
		return true;
	}
	if (message == WM_WTSSESSION_CHANGE && (wparam == WTS_SESSION_LOCK)) {
		core::resetState();
		return true;
	}

	return false;
}

}

namespace layoutHook {

	void create(HWND mainWindow) {

		verify(LayoutChangedMessage = ::RegisterWindowMessage(layoutHookDll::LayoutChangedMessage));
		verify(SetLayoutMessage = ::RegisterWindowMessage(layoutHookDll::SetLayoutMessage));
		mainWindow::addMessageHandler(messageHandler);

		layoutHookDll::create(mainWindow);
		hook = SetWindowsHookEx(WH_CALLWNDPROC, layoutHookDll::proc, GetModuleHandle(_T("layoutHookDll.dll")), 0);
		assert(hook != 0);

		verify(WTSRegisterSessionNotification(mainWindow, NOTIFY_FOR_THIS_SESSION));
	}

	HKL getLayout(HWND window) {
		return GetKeyboardLayout(GetWindowThreadProcessId(window, 0));
	}

	void setLayout(HWND window, HKL layout) {

		// First method. Does not work on desktop
		::PostMessage(window, WM_INPUTLANGCHANGEREQUEST, 0, reinterpret_cast<LPARAM>(layout));

		// Second method. Does not work in command prompt
		::SendMessageTimeout(window, SetLayoutMessage, 0, reinterpret_cast<LPARAM>(layout), 0, 1000 /* ms */, NULL);
			// If timeout elapsed, layout will just not be changed
	}

	void destroy(HWND mainWindow) {
		verify(UnhookWindowsHookEx(hook));
		layoutHookDll::destroy();
		verify(WTSUnRegisterSessionNotification(mainWindow));
	}

}