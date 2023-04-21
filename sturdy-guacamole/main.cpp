#include "Win32Application.h"
#include "ImguiApplication.h"
#include "Dx11Application.h"

#include <iostream>

namespace sturdy_guacamole
{
}

// Forward declarations
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API void ImGui_ImplDX11_RenderDrawData(ImDrawData* draw_data);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main()
{
	// Initialize singleton win32 application
	sturdy_guacamole::Win32Application win32App{};

	// Set WndProc for the application window
	::SetWindowLongPtrW(win32App.GetWindowHandle(), GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc));

	// Initialize singleton dx11 application
	sturdy_guacamole::Dx11Application dx11App{ win32App.GetWindowHandle() };

	// Initialize singleton imgui application
	sturdy_guacamole::ImguiApplication imguiApp{ win32App.GetWindowHandle(), dx11App.Device.Get(), dx11App.DeviceContext.Get()};

	bool quit{};
	while (!quit)
	{
		// Process any messages in the queue
		MSG msg{};
		while (::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
			if (msg.message == WM_QUIT)
				quit = true;
		}
		if (quit)
			break;

		// Run game code here

		// Start the Dear ImGui frame
		imguiApp.NewFrame();
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Image((void*)nullptr, ImVec2(1024, 768));
		ImGui::End();

		ImGui::Begin("Details Panel");
		ImGui::Text("Hello world!");
		ImGui::Text("fps: %f", ImGui::GetIO().Framerate);
		ImGui::Text("delta time: %f", ImGui::GetIO().DeltaTime);
		ImGui::End();

		// Rendering
		ImGui::Render();
		const float clear_color[4] { 0.45f, 0.55f, 0.60f, 1.00f };
		g_deviceContext->OMSetRenderTargets(1, g_renderTargetView.GetAddressOf(), nullptr);
		g_deviceContext->ClearRenderTargetView(g_renderTargetView.Get(), clear_color);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows (if there are floating windows)
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		g_swapChain->Present(1, 0); // Present with vsync

	}

	return 0;
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_device && wParam != SIZE_MINIMIZED)
		{
			if (g_renderTargetView)
				g_renderTargetView = nullptr;

			g_swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

			// Create a render target view
			ComPtr<ID3D11Texture2D> backBuffer;
			g_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
			g_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &g_renderTargetView);
		}
		return 0;
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