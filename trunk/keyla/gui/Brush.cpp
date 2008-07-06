#pragma once

#include "../common.h"
#include "Brush.h"

Brush::Brush() : m_brush(0) {
}

Brush::Brush(COLORREF color) : m_brush(CreateSolidBrush(color)) {
	assert(m_brush != 0);
}

Brush::~Brush() {
	if (m_brush != 0) {
		DeleteObject(m_brush);
		m_brush = 0;
	}
}

Brush::operator HBRUSH() {
	return m_brush;
}
