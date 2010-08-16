//
// Module TRAY_ICON owns the tray icon
//

namespace trayIcon {

	// Create the tray icon
	void create(HWND mainWindow);

	// Show the context menu. Messages of the menu are sent to the MAIN_WINDOW module
	void showMenu();

	// Select the icon that corresponds the layout passed
	void indicateLayout(HKL layout);

	// Destroy the tray icon
	void destroy();

	// Get the context menu
	HMENU getMenu();
}