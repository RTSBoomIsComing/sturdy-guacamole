#include "Win32Application.h"
#include "ImguiApplication.h"

#include <iostream>

namespace sturdy_guacamole
{
	HWND g_hWnd{};
}

// Forward declarations
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main()
{
	using namespace sturdy_guacamole;

	// Create win32 application window
	sturdy_guacamole::Win32Application win32App{};

	// Set global window handle
	sturdy_guacamole::g_hWnd = win32App.GetWindowHandle();

	// Set WndProc for the application window
	::SetWindowLongPtrW(g_hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc));

	// Create imgui application
	sturdy_guacamole::ImguiApplication imguiApp{ g_hWnd, nullptr, nullptr};



	MSG msg{};
	while (msg.message != WM_QUIT)
	{
		// Process any messages in the queue
		while (::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
		}

		// Run game code here
	}

	return 0;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

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

