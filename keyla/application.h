#include "../win32xx/wxx_wincore.h"
#include "common.h"


class Application : public CWinApp {
public:

	Application();
	virtual BOOL InitInstance();
	virtual ~Application();

	void toggle();
	bool isActive() {
		return m_active;
	}

private:

	HANDLE m_exclusion;
	bool m_active;
};

inline Application& GetApplication() { return static_cast<Application&>(GetApp()); }