#pragma once

//
// Class Brush
//
// Copy policy - copying must be restricted
// Type cast policy - there is an implicit type cast to HBRUSH
//
class Brush {
public:

	// Default constructor
	Brush();

	// Constuctor to create a brush of the special color
	explicit Brush(COLORREF color);

	// Destructor that automatically destroys the contained resource (i.e., HBRUSH)
	~Brush();

	operator HBRUSH();

private:

	HBRUSH m_brush;
};
