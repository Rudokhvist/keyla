#include "../win32xx/wxx_wincore.h"
#include "common.h"
#include "mainWindow.h"


class Application : public CWinApp {
public:

	Application();
	virtual BOOL InitInstance();
	virtual ~Application();

	void toggle();
	bool isActive() {
		return m_active;
	}

	MainWindow& GetMainWindow() { return m_mainWindow; }

private:
	MainWindow m_mainWindow;
	HANDLE m_exclusion;
	bool m_active;
};

inline Application& GetApplication() { return static_cast<Application&>(GetApp()); }