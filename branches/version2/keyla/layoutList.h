//
// Module LAYOUT_LIST stores the list of layouts enabled in the system
// 

#pragma once

#include <vector>

namespace layoutList {

	// List of layouts
	extern std::vector<HKL> LayoutList;

	// Load list of layouts
	void load();

	// A helper function that returns language identifier of the layout passed as string
	tstring layoutLangId(HKL layout);

	// A helper function that returns language if the layout passed
	tstring layoutLanguage(HKL layout);
}