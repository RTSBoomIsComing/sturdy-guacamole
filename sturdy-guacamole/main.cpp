#include "Win32Application.h"
#include "ImguiApplication.h"
#include "D3d11Application.h"

#include <iostream>

namespace sturdy_guacamole
{
	ID3D11Device* g_pDevice{};
	ID3D11DeviceContext* g_pDeviceContext{};
	IDXGISwapChain* g_pSwapChain{};
}

// Forward declarations
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main()
{
	using namespace sturdy_guacamole;

	// Initialize singleton win32 application
	sturdy_guacamole::Win32Application win32App{};

	// Set WndProc for the application window
	::SetWindowLongPtrW(win32App.GetWindowHandle(), GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc));

	// Initialize singleton d3d11 application
	sturdy_guacamole::D3d11Application d3d11App{ Win32Application::Get().GetWindowHandle() };

	// Initialize singleton imgui application
	sturdy_guacamole::ImguiApplication imguiApp{ Win32Application::Get().GetWindowHandle(), D3d11Application::Get().Device.Get(), D3d11Application::Get().DeviceContext.Get()};

	bool quit{};
	while (!quit)
	{
		// Process any messages in the queue
		MSG msg{};
		while (::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
			if (quit = msg.message == WM_QUIT)
				break;
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

