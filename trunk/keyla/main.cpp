//
// ћодуль MAIN заведует инициализацией при запуске программы и
// очисткой ресурсов при завершении программы
//

#pragma comment(lib, "comctl32")

#include "common.h"
#include "application.h"

int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int) {
	Application().Run();
}