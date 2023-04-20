#include "Win32Application.h"

#include <iostream>

namespace sturdy_guacamole
{
	HWND g_hWnd{};
}

// forward declarations
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main()
{
	// Create application window
	sturdy_guacamole::Win32Application win32app{};

	// Set global window handle
	sturdy_guacamole::g_hWnd = win32app.GetWindowHandle();

	// Set WndProc for the application window
	::SetWindowLongPtrW(sturdy_guacamole::g_hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc));

	while (true)
	{
		MSG msg{};
		if (::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
			if (msg.message == WM_QUIT)
				break;
		}
	}

	return 0;
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

