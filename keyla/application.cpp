#include "activeWindowHook.h"
#include "application.h"
#include "core.h"
#include "exclusion/exclusion.h"
#include "keyboardHook.h"
#include "layoutHook.h"
#include "layoutList.h"
#include "mainWindow.h"
#include "settings.h"
#include "trayIcon.h"

#include <commctrl.h>

Application::Application() : m_exclusion(0), m_active(false) {
}

/* virtual */ BOOL Application::InitInstance() {
	if (!CWinApp::InitInstance())
		return FALSE;

	m_exclusion = ::CreateMutex(0, FALSE, createExclusionName(_T("keyla{f91f8362-290e-4ad5-8408-83d8dd3a5e54}"), UNIQUE_TO_SESSION).c_str());
    assert(m_exclusion);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
		return FALSE;

	INITCOMMONCONTROLSEX icc = {sizeof icc, ICC_WIN95_CLASSES};
	InitCommonControlsEx(&icc);

	layoutList::load();
	settings::load();
	mainWindow::create();
	activeWindowHook::create();
	keyboardHook::create();
	
	m_active = true;
	return TRUE;
}

/* virtual */ Application::~Application() {
	m_active = false;

	keyboardHook::destroy();
	activeWindowHook::destroy();

	if (m_exclusion) {
		verify(::CloseHandle(m_exclusion));
		m_exclusion = 0;
	}
}

void Application::toggle() {
	m_active = !m_active;

	// Icons for active and disabled mode are different, so change it
	trayIcon::indicateLayout(core::getLayout());
}
