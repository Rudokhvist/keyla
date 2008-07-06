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
#include "util/FatalError.h"
#include "../win32pp/WinCore.h"

#include <commctrl.h>
#pragma comment(lib, "comctl32")

class ApplicationClass : public CWinApp {
public:

	virtual BOOL InitInstance() {
		if (!CWinApp::InitInstance()) return FALSE;

		INITCOMMONCONTROLSEX icc = {sizeof icc, ICC_WIN95_CLASSES};
		InitCommonControlsEx(&icc);

		try {
			layoutList::load();
			settings::load();
			mainWindow::create();
			layoutHook::create();
			keyboardHook::create();
		}
		catch(FatalError & e) {
			MessageBox(0, e.message().c_str(), TEXT("keyla"), 0);
			return FALSE;
		}
		return TRUE;
	}

	virtual ~ApplicationClass() {
		keyboardHook::destroy();
		layoutHook::destroy();
	}
} Application;

int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int) {
	Application.Run();
}