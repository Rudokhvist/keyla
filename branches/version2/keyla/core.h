//
// Module CORE provides functions to manipulate layouts
//
// If the opposite is not stated, all functions act according to the global settings
//

#pragma once

namespace core {

	// Switch to next layout
	void nextLayout();

	// Switch to particular layout
	// index - index of the layout in the list stored in the LAYOUT_LIST module
	void setLayout(unsigned int index);

	// Function that gets called when user activates other window
	// activeWindow - window that became active
	// layout - layout in the window
	void activeWindowChanged(HWND activeWindow, HKL layout);

	// Function that gets calles when layout is changed in a window
	// window - the window
	// layout - the new layout (may be HKL_NEXT or other HKL_* constant)
	void layoutChanged(HWND window, HKL layout);

	// Function that gets called when a key pressed
	//
	// vk - virtual key code
	// modifiers - bitwise OR of HotKey::Modifiers
	// return - whether the key was handled
	bool keyPressed(unsigned int vk, unsigned int modifiers);

	
	// Get current layout
	HKL getLayout();
}