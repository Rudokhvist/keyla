#pragma once

#include "../common.h"
#include "Menu.h"

Menu::Menu() : m_menu(0) {
}

Menu::Menu(LPCTSTR resourceName) : m_menu(LoadMenu(GetModuleHandle(0), resourceName)) {
	assert(m_menu != 0);
}

Menu::~Menu() {
	if (m_menu != 0) {
		DestroyMenu(m_menu);
		m_menu = 0;
	}
}

Menu::operator HMENU() {
	return m_menu;
}
