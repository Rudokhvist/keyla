#include "common.h"
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
	}

	HKL getLayout(HWND window) {
		return GetKeyboardLayout(GetWindowThreadProcessId(window, 0));
	}

	void setLayout(HWND window, HKL layout) {		
		::SendMessageTimeout(window, SetLayoutMessage, 0, reinterpret_cast<LPARAM>(layout), 0, 1000 /* ms */, NULL);
		// If timeout elapsed, layout will just not be changed
	}

	void destroy() {
		verify(UnhookWindowsHookEx(hook));
		layoutHookDll::destroy();
	}

}