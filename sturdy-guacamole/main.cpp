#include "Win32Application.h"
#include "ImguiApplication.h"
#include "Dx11Application.h"

#include "Graphics.h"
#include "ConstantBuffers.h"

// include my glTF to DirectX11 converter libraries
#include "GLTFModel.h"



// Include DirectX Tool Kit
#include <directxtk/SimpleMath.h>
#include <directxtk/Keyboard.h>
#include <directxtk/Mouse.h>

// Include standard library headers
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <format>
#include <stdexcept>
#include <functional>

namespace sturdy_guacamole
{

}

// Forward declarations
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API void ImGui_ImplDX11_RenderDrawData(ImDrawData* draw_data);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main()
{
	using namespace DirectX::SimpleMath;

	// Initialize win32 application singleton instance
	sturdy_guacamole::Win32Application win32App{};

	// Set WndProc for the application window
	::SetWindowLongPtrW(win32App.m_hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc));

	// Initialize dx11 application singleton instance
	sturdy_guacamole::Dx11Application dx11App{ win32App.m_hWnd };

	// Create imgui application
	sturdy_guacamole::ImguiApplication imguiApp{ win32App.m_hWnd, dx11App.GetDevice(), dx11App.GetDeviceContext() };

	// Initialize mouse singleton instance
	std::unique_ptr<DirectX::Mouse> mouse = std::make_unique<DirectX::Mouse>();
	mouse->SetWindow(win32App.m_hWnd);
	DirectX::Mouse::ButtonStateTracker mouse_tracker;

	// Initialize keyboard singleton instance
	std::unique_ptr<DirectX::Keyboard> keyboard = std::make_unique<DirectX::Keyboard>();
	DirectX::Keyboard::KeyboardStateTracker kb_tracker;

	// Initialize Graphics singleton instance
	sturdy_guacamole::Graphics gfx{};

	CD3D11_VIEWPORT viewport{ 0.0F, 0.0F, static_cast<float>(win32App.m_width), static_cast<float>(win32App.m_height) };
	g_pDeviceContext->RSSetViewports(1, &viewport);

	// L"D:\\GitHub\\glTF-Sample-Models\\2.0\\ABeautifulGame\\glTF\\ABeautifulGame.gltf"
	// L"D:\\GitHub\\glTF-Sample-Models\\2.0\\Cube\\glTF\\Cube.gltf"
	// L"D:\\GitHub\\glTF-Sample-Models\\2.0\\Triangle\\glTF\\Triangle.gltf"
	// L"D:\\GitHub\\glTF-Sample-Models\\2.0\\Avocado\\glTF\\Avocado.gltf"
	std::filesystem::path gltfPath{ L"D:\\GitHub\\glTF-Sample-Models\\2.0\\ABeautifulGame\\glTF\\ABeautifulGame.gltf" };

	// Load glTF model
	sturdy_guacamole::GLTFModel gltfModel{ gltfPath };

	// Create common constant buffer
	CD3D11_BUFFER_DESC bufferDesc{ sizeof(sturdy_guacamole::CommonConstants), D3D11_BIND_CONSTANT_BUFFER,
		D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE };

	ComPtr<ID3D11Buffer> commonConstantBuffer;
	HRESULT hr = g_pDevice->CreateBuffer(&bufferDesc, nullptr, &commonConstantBuffer);
	if (FAILED(hr))
		throw hr;

	// Create mesh constant buffer
	bufferDesc.ByteWidth = sizeof(sturdy_guacamole::MeshConstants);

	ComPtr<ID3D11Buffer> meshConstantBuffer;
	hr = g_pDevice->CreateBuffer(&bufferDesc, nullptr, &meshConstantBuffer);
	if (FAILED(hr))
		throw hr;

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
		float deltaTime = ImGui::GetIO().DeltaTime;


		// Process keyboard and mouse inputs
		using ButtonState = DirectX::Mouse::ButtonStateTracker::ButtonState;
		auto mouse_state = mouse->GetState();

		// Control the camera (viewer) position and rotation
		static Vector3 viewerPos{ 0.0f, 0.0f, 1.0f };
		static Vector3 viewerRot{ 0.0f, 0.0f, 0.0f };
		if (mouse_state.positionMode == DirectX::Mouse::MODE_RELATIVE)
		{
			viewerRot.x -= float(mouse_state.y) * 0.3f * deltaTime;
			viewerRot.y -= float(mouse_state.x) * 0.1f * deltaTime;
		}

		mouse_tracker.Update(mouse_state);
		if (mouse_tracker.rightButton == ButtonState::PRESSED)
		{
			mouse->SetMode(DirectX::Mouse::MODE_RELATIVE);
		}
		else if (mouse_tracker.rightButton == ButtonState::RELEASED)
		{
			mouse->SetMode(DirectX::Mouse::MODE_ABSOLUTE);
		}

		auto kb_state = keyboard->GetState();
		kb_tracker.Update(kb_state);

		Vector3 viewerForward = Vector3::Transform(Vector3::Forward, Matrix::CreateFromYawPitchRoll(viewerRot));
		Vector3 viewerUp = Vector3::Transform(Vector3::Up, Matrix::CreateFromYawPitchRoll(viewerRot));
		Vector3 viewerRight = Vector3::Transform(Vector3::Right, Matrix::CreateFromYawPitchRoll(viewerRot));

		if (kb_state.W)
			viewerPos += viewerForward * deltaTime;

		if (kb_state.S)
			viewerPos -= viewerForward * deltaTime;

		if (kb_state.A)
			viewerPos -= viewerRight * deltaTime;

		if (kb_state.D)
			viewerPos += viewerRight * deltaTime;

		if (kb_state.Space)
			viewerPos += viewerUp * deltaTime;

		if (kb_state.C)
			viewerPos -= viewerUp * deltaTime;


		// Clear the back buffer
		ID3D11RenderTargetView* ppRenderTargetViews[] = {
			g_pRenderTargetView
		};
		g_pDeviceContext->OMSetRenderTargets(ARRAYSIZE(ppRenderTargetViews), ppRenderTargetViews, nullptr);

		float clear_color[]{ 0.0f, 0.2f, 0.4f, 1.0f };
		g_pDeviceContext->ClearRenderTargetView(g_pRenderTargetView, clear_color);
		// TODO : clear depth stencil view
		// g_pDeviceContext->ClearDepthStencilView()



		// create view, projection matrix
		Matrix viewMatrix = DirectX::XMMatrixLookToRH(viewerPos, viewerForward, viewerUp);

		Matrix projMatrix = Matrix::CreatePerspectiveFieldOfView(
			DirectX::XMConvertToRadians(90.0f), float(win32App.m_width) / float(win32App.m_height), 0.1f, 100.0f);

		sturdy_guacamole::CommonConstants commonConstants{};
		commonConstants.ViewerPos = Vector4{ viewerPos };
		commonConstants.ViewMatrix = viewMatrix;
		commonConstants.ProjMatrix = projMatrix;
		commonConstants.ViewProjMatrix = (viewMatrix * projMatrix);

		// Update common constant buffer
		D3D11_MAPPED_SUBRESOURCE mappedResource{};
		g_pDeviceContext->Map(commonConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		::memcpy(mappedResource.pData, &commonConstants, sizeof(commonConstants));
		g_pDeviceContext->Unmap(commonConstantBuffer.Get(), 0);



		//  Set constant buffer
		ID3D11Buffer* pConstantBuffers[] = {
			meshConstantBuffer.Get(),
			commonConstantBuffer.Get(),
		};
		g_pDeviceContext->VSSetConstantBuffers(0, ARRAYSIZE(pConstantBuffers), pConstantBuffers);

		// Start rendering
		for (const auto& scene : gltfModel.m_scenes)
		{
			for (const auto& traversal : scene.m_traversals)
			{
				for (const auto& step : traversal)
				{
					// Create mesh constants
					sturdy_guacamole::MeshConstants meshConstants = {
							step.m_globalTransform,
							step.m_globalTransform.Invert().Transpose()
					};

					// Update mesh constant buffer
					D3D11_MAPPED_SUBRESOURCE mappedResource{};
					g_pDeviceContext->Map(meshConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
					::memcpy(mappedResource.pData, &meshConstants, sizeof(meshConstants));
					g_pDeviceContext->Unmap(meshConstantBuffer.Get(), 0);
					if (step.m_node->m_mesh)
					{
						for (const auto& primitive : step.m_node->m_mesh->m_meshPrimitives)
						{
							primitive.Draw(g_pDeviceContext);
						}
					}
				}
			}
		}

		// Start the Dear ImGui frame
		imguiApp.NewFrame();
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

		ImGui::Begin("Details Panel");
		ImGui::Text("Hello world!");
		ImGui::Text("fps: %f", ImGui::GetIO().Framerate);
		ImGui::Text("delta time: %f", ImGui::GetIO().DeltaTime);
		ImGui::DragFloat3("viewerRot", reinterpret_cast<float*>(&viewerRot));
		ImGui::End();

		ImGui::Render();
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
		{
			UINT newWidth = LOWORD(lParam);
			UINT newHeight = HIWORD(lParam);

			sturdy_guacamole::Dx11Application::ResizeRenderTarget(newWidth, newHeight);
			sturdy_guacamole::Win32Application::Get().m_width = newWidth;
			sturdy_guacamole::Win32Application::Get().m_height = newHeight;
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;

	case WM_ACTIVATE:
	case WM_ACTIVATEAPP:
		DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
		DirectX::Mouse::ProcessMessage(msg, wParam, lParam);
		break;

	case WM_SYSKEYDOWN:
		if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
		{
			// This is where you'd implement the classic ALT+ENTER hotkey for fullscreen toggle
		}
		DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
		break;

	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		DirectX::Mouse::ProcessMessage(msg, wParam, lParam);
		break;

	case WM_MOUSEACTIVATE:
		// When you click to activate the window, we want Mouse to ignore that event.
		return MA_ACTIVATEANDEAT;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
