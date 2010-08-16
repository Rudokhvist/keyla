#pragma once

//
// Class of a Menu
//
// Copy policy - copying must be restricted
// Type cast policy - there is an implicit cast to HMENU
//
class Menu {
public:

	// Default constructor
	Menu();

	// Constructor to load a menu from resources
	// resourceName - resource-name or MAKEINTRESOURCE(resource-id)
	Menu(LPCTSTR resourceName);

	// Destructor that automatically destroys the contained object (i.e., HMENU)
	~Menu();

	operator HMENU();

private:

	HMENU m_menu;
};
