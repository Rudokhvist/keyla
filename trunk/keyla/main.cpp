//
// Module MAIN initializes and turns off other modules
//

#pragma comment(lib, "comctl32")

#include "common.h"
#include "application.h"

int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int) {
	Application().Run();
}