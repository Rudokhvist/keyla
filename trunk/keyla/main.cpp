//
// ћодуль MAIN заведует инициализацией при запуске программы и
// очисткой ресурсов при завершении программы
//

#include "common.h"
#include "keyboardHook.h"
#include "layoutHook.h"
#include "layoutList.h"
#include "mainWindow.h"
#include "settings.h"
#include "../win32xx/WinCore.h"

#include <commctrl.h>
#pragma comment(lib, "comctl32")

class Application : public CWinApp {
public:

	virtual BOOL InitInstance() {
		if (!CWinApp::InitInstance()) return FALSE;

		INITCOMMONCONTROLSEX icc = {sizeof icc, ICC_WIN95_CLASSES};
		InitCommonControlsEx(&icc);

		layoutList::load();
		settings::load();
		mainWindow::create();
		layoutHook::create();
		keyboardHook::create();
		
		return TRUE;
	}

	virtual ~Application() {
		keyboardHook::destroy();
		layoutHook::destroy();
	}
};

int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int) {
	Application().Run();
}