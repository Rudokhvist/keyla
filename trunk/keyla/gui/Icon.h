#pragma once

//
// Class of an Icon
//
// Copy policy - copying must be restricted
// Type cast policy - there is an implicit type cast to HICON
//
class Icon {
public:

	// Default constructor
	Icon();

	// Constructor that loads an icon from resources
	// resourceName - resource-name or MAKEINTRESOURCE(resource-id)
	Icon(LPCTSTR resourceName);

	// Destructor that automatically destroys the contained object (i.e., HICON)
	~Icon();

	operator HICON();

private:

	HICON m_icon;
};
