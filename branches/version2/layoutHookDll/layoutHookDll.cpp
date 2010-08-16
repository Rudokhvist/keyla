#include "../keyla/common.h"
#include "layoutHookDll.h"

namespace {

	// Data that must be visible from the hook procedure.
	// Must be placed in a shared section (or memory mapping, etc.)
	// because the procedure is called within other processes.
	//
	struct SharedStruct {

		static const LPCTSTR Id;

		HWND window;
	};
	const LPCTSTR SharedStruct::Id = _T("keyla - layout hook DLL - id - shared {df45ca5e-89f8-4834-a98f-3e13c9f69ecb}");

	// Variables specific for a process we are attached to

	HANDLE Mapping = NULL;
	SharedStruct * Shared = NULL;

	unsigned int LayoutChangedMessage = 0;
	unsigned int SetLayoutMessage = 0;
}

namespace layoutHookDll {

void create(HWND window) {

	Mapping = ::CreateFileMapping(0, 0, PAGE_READWRITE, 0, sizeof(SharedStruct), SharedStruct::Id);
	assert(Mapping != NULL);
	if (Mapping == NULL) return;

	Shared = static_cast<SharedStruct *>(::MapViewOfFile(Mapping, FILE_MAP_ALL_ACCESS, 0, 0, 0));
	assert(Shared != NULL);
	if (Shared == NULL) return;

	Shared->window = window;
}

// Hook procedure.
//
// The most important fact is that this hook is called within the process
// where a window procedure is called. So we have some additional means
// to manipulate keyboard layouts in this process.
//
// For example, posting a WM_INPUTLANGCHANGEREQUEST does not change layout
// when user renames a file on the desktop. ActivateKeyboardLayout() works
// but it can be called within the target process only.
//
// This hook procedure must be placed in a DLL. Otherwise a crash occurs
// in a Windows library when ::SetWindowsHookEx() is called.
//
LRESULT CALLBACK proc(int code, WPARAM wparam, LPARAM lparam) {

	// MSDN says one must do nothing if code is < 0
	if (code < 0)
		return CallNextHookEx(0, code, wparam, lparam);

	const CWPSTRUCT	* p = reinterpret_cast<const CWPSTRUCT *>(lparam);
	assert(p != 0);
	if (p == 0)
		return CallNextHookEx(0, code, wparam, lparam);

	if (p->message == WM_INPUTLANGCHANGE) {
		::PostMessage(Shared->window, ::LayoutChangedMessage, reinterpret_cast<WPARAM>(p->hwnd), /* HKL */ p->lParam);
	}
	else if (p->message == ::SetLayoutMessage) {
		::ActivateKeyboardLayout(reinterpret_cast<HKL>(p->lParam), 0);
		return 0;
	}

	return CallNextHookEx(0, code, wparam, lparam);
}

void destroy() {
	// Do everything in DllMain
}

}

BOOL WINAPI DllMain(HINSTANCE, DWORD reason, void * lpvReserved) {
	switch (reason) {
		case DLL_PROCESS_ATTACH:		
			
			// Initialization for every process we are attached to			
			verify(LayoutChangedMessage = ::RegisterWindowMessage(layoutHookDll::LayoutChangedMessage));
			verify(SetLayoutMessage = ::RegisterWindowMessage(layoutHookDll::SetLayoutMessage));

			Mapping = ::OpenFileMapping(PAGE_READONLY, FALSE, SharedStruct::Id);
				// This fails when DLL is attached to keyla's process. See create()

			if (Mapping != NULL) {
				Shared = static_cast<SharedStruct *>(::MapViewOfFile(Mapping, FILE_MAP_ALL_ACCESS, 0, 0, 0));
				assert(Shared != NULL);
				if (Shared == NULL) return FALSE;
			}
		
		break;
		case DLL_PROCESS_DETACH:

			// Deinitialization
			if (Shared != NULL) {
				verify(UnmapViewOfFile(Shared));
			}
			if (Mapping != NULL) {
				verify(CloseHandle(Mapping));
			}
		
		break;
	}
	return TRUE;
}