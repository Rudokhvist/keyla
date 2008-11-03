#include "common.h"

void showContextMenu(HMENU menu, HWND window) {
	// This is necessary to make the menu disappear when user clicks outside of it
	SetForegroundWindow(window);

	POINT pt;
	verify(GetCursorPos(&pt));

	verify(TrackPopupMenu(GetSubMenu(menu, 0), 0, pt.x, pt.y, 0, window, 0));
}