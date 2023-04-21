#include "Win32Application.h"
#include "ImguiApplication.h"
#include "Dx11Application.h"
#include "Graphics.h"

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

	// Create imgui application
	sturdy_guacamole::ImguiApplication imguiApp{ win32App.GetWindowHandle(), dx11App.GetDevice(), dx11App.GetDeviceContext()};

	

	// Set the viewport
	D3D11_VIEWPORT viewport = CD3D11_VIEWPORT(0.0F, 0.0F, float(1024), float(768));
	g_pDeviceContext->RSSetViewports(1, &viewport);


	// Create Texture2D for RenderTarget
	ComPtr<ID3D11Texture2D> renderTarget;
	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = 1024;
	desc.Height = 768;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	g_pDevice->CreateTexture2D(&desc, nullptr, &renderTarget);

	// Create RenderTargetView
	ComPtr<ID3D11RenderTargetView> renderTargetView;
	g_pDevice->CreateRenderTargetView(renderTarget.Get(), nullptr, &renderTargetView);
	g_pDeviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), nullptr);

	// Initialize Graphics singleton instance
	sturdy_guacamole::Graphics gfx{};

	// Create ShaderResourceView
	ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	g_pDevice->CreateShaderResourceView(renderTarget.Get(), nullptr, &shaderResourceView);

	// Main message loop
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
		ImGui::Image(shaderResourceView.Get(), ImVec2(1024, 768));
		ImGui::End();

		ImGui::Begin("Details Panel");
		ImGui::Text("Hello world!");
		ImGui::Text("fps: %f", ImGui::GetIO().Framerate);
		ImGui::Text("delta time: %f", ImGui::GetIO().DeltaTime);
		ImGui::End();

		// Rendering
		ImGui::Render();
		const float clear_color[4] { 0.45f, 0.55f, 0.60f, 1.00f };
		ID3D11RenderTargetView* ppRenderTargetViews[]
		{ 
			g_pRenderTargetView 
		};
		g_pDeviceContext->OMSetRenderTargets(ARRAYSIZE(ppRenderTargetViews), ppRenderTargetViews, nullptr);
		g_pDeviceContext->ClearRenderTargetView(g_pRenderTargetView, clear_color);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows (if there are floating windows)
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		g_pSwapChain->Present(1, 0); // Present with vsync

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
		if (g_pDevice && wParam != SIZE_MINIMIZED)
			sturdy_guacamole::Dx11Application::ResizeRenderTarget();
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