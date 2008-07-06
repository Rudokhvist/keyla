#include "common.h"

void showContextMenu(HMENU menu, HWND window) {
	// „тобы меню исчезало при щелчке вне него
	SetForegroundWindow(window);

	POINT pt;
	verify(GetCursorPos(&pt));

	verify(TrackPopupMenu(GetSubMenu(menu, 0), 0, pt.x, pt.y, 0, window, 0));
}