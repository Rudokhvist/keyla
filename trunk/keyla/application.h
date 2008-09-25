#include "common.h"
#include "../win32xx/WinCore.h"

class Application : public CWinApp {
public:

	Application();
	virtual BOOL InitInstance();
	virtual ~Application();

	static Application * GetApp() {
		return static_cast<Application *>(CWinApp::GetApp());
	}

	void toggle();
	bool isActive() {
		return m_active;
	}

private:

	HANDLE m_exclusion;
	bool m_active;
};