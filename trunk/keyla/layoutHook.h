//
// Module LAYOUT_HOOK provide some functions
// to get or set keyboard layout in a window
//

#pragma once

namespace layoutHook {

	// Initialize
	void create(HWND mainWindow);

	// Get keyboard layout of a window
	// If fails, returns 0
	HKL getLayout(HWND window);

	// Set keyboard layout of a window
	void setLayout(HWND window, HKL layout);

	// Destroy
	void destroy();
}