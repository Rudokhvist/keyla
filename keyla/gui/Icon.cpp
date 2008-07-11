#pragma once

#include "../common.h"
#include "Icon.h"

Icon::Icon() : m_icon(0) {
}

Icon::Icon(LPCTSTR resourceName) {
	const unsigned int flags = LR_LOADTRANSPARENT;
	HGDIOBJ obj = LoadImage(GetModuleHandle(0), resourceName, IMAGE_ICON, 0, 0, flags);
	m_icon = static_cast<HICON>(obj);
	assert(m_icon != 0);
}

Icon::~Icon() {
	if (m_icon != 0) {
		DestroyIcon(m_icon);
		m_icon = 0;
	}
}

Icon::operator HICON() {
	return m_icon;
}
