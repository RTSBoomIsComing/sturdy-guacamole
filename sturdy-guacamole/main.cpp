#include "Win32Application.h"
#include "ImguiApplication.h"
#include "Dx11Application.h"
#include "Dx11Helpers.h"
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
	using sturdy_guacamole::ThrowIfFailed;

	// Initialize win32 application singleton instance
	sturdy_guacamole::Win32Application win32App{};

	// Set WndProc for the application window
	::SetWindowLongPtrW(win32App.m_hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc));

	// Initialize dx11 application singleton instance
	sturdy_guacamole::Dx11Application dx11App{ win32App.m_hWnd };

	// Create imgui application
	sturdy_guacamole::ImguiApplication imguiApp{ win32App.m_hWnd, dx11App.m_device.Get(), dx11App.m_deviceContext.Get() };

	// Initialize mouse singleton instance
	std::unique_ptr<DirectX::Mouse> mouse = std::make_unique<DirectX::Mouse>();
	mouse->SetWindow(win32App.m_hWnd);
	DirectX::Mouse::ButtonStateTracker mouse_tracker;

	// Initialize keyboard singleton instance
	std::unique_ptr<DirectX::Keyboard> keyboard = std::make_unique<DirectX::Keyboard>();
	DirectX::Keyboard::KeyboardStateTracker kb_tracker;

	// Initialize Graphics singleton instance
	sturdy_guacamole::Graphics gfx{};

	// L"D:\\GitHub\\glTF-Sample-Models\\2.0\\ABeautifulGame\\glTF\\ABeautifulGame.gltf"
	// L"D:\\GitHub\\glTF-Sample-Models\\2.0\\Cube\\glTF\\Cube.gltf"
	// L"D:\\GitHub\\glTF-Sample-Models\\2.0\\Triangle\\glTF\\Triangle.gltf"
	// L"D:\\GitHub\\glTF-Sample-Models\\2.0\\Avocado\\glTF\\Avocado.gltf"
	// L"D:\\GitHub\\glTF-Sample-Models\\2.0\\DamagedHelmet\\glTF\\DamagedHelmet.gltf"
	
	std::filesystem::path gltfPath{ L"D:\\GitHub\\glTF-Sample-Models\\2.0\\DamagedHelmet\\glTF\\DamagedHelmet.gltf" };

	// Load glTF model
	sturdy_guacamole::GLTFModel gltfModel{ gltfPath };

	// Create common constant buffer
	CD3D11_BUFFER_DESC bufferDesc{ sizeof(sturdy_guacamole::CommonConstants), D3D11_BIND_CONSTANT_BUFFER,
		D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE };

	ComPtr<ID3D11Buffer> commonConstantBuffer;
	ThrowIfFailed(g_pDevice->CreateBuffer(&bufferDesc, nullptr, &commonConstantBuffer));


	// Create mesh constant buffer
	bufferDesc.ByteWidth = sizeof(sturdy_guacamole::MeshConstants);
	ComPtr<ID3D11Buffer> meshConstantBuffer;
	ThrowIfFailed(g_pDevice->CreateBuffer(&bufferDesc, nullptr, &meshConstantBuffer));

	// Create GSConstant Buffer
	bufferDesc.ByteWidth = sizeof(sturdy_guacamole::NormalGSConstants);
	ComPtr<ID3D11Buffer> normalGSConstantBuffer;
	ThrowIfFailed(g_pDevice->CreateBuffer(&bufferDesc, nullptr, &normalGSConstantBuffer));

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
		float clear_color[]{ 0.0f, 0.2f, 0.4f, 1.0f };
		g_pDeviceContext->ClearRenderTargetView(gfx.m_rtview.main.Get(), clear_color);
		g_pDeviceContext->ClearDepthStencilView(gfx.m_dsview.main.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		g_pDeviceContext->OMSetRenderTargets(1, gfx.m_rtview.main.GetAddressOf(), gfx.m_dsview.main.Get());



		// create view, projection matrix
		Matrix viewMatrix = DirectX::XMMatrixLookToRH(viewerPos, viewerForward, viewerUp);

		Matrix projMatrix = Matrix::CreatePerspectiveFieldOfView(
			DirectX::XMConvertToRadians(90.0f), float(win32App.m_width) / float(win32App.m_height), 0.01f, 100.0f);

		sturdy_guacamole::CommonConstants commonConstants{};
		commonConstants.ViewMatrix = viewMatrix;
		commonConstants.ViewInv = viewMatrix.Invert();
		commonConstants.ProjMatrix = projMatrix;
		commonConstants.ProjInv = projMatrix.Invert();
		commonConstants.ViewProjMatrix = (viewMatrix * projMatrix);
		commonConstants.ViewProjInv = commonConstants.ViewProjMatrix.Invert();
		commonConstants.ViewerPos = Vector4{ viewerPos };

		// Update common constant buffer
		D3D11_MAPPED_SUBRESOURCE mappedResource{};
		g_pDeviceContext->Map(commonConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		::memcpy(mappedResource.pData, &commonConstants, sizeof(commonConstants));
		g_pDeviceContext->Unmap(commonConstantBuffer.Get(), 0);

		//  Set vertex shader constant buffer
		ID3D11Buffer* pConstantBuffers[] = {
			meshConstantBuffer.Get(),
			commonConstantBuffer.Get(),
		};
		g_pDeviceContext->VSSetConstantBuffers(0, ARRAYSIZE(pConstantBuffers), pConstantBuffers);

		// Update geometry shader constant buffer
		static bool bDrawNormals{};
		static sturdy_guacamole::NormalGSConstants normalGSConstants{};
		g_pDeviceContext->Map(normalGSConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		::memcpy(mappedResource.pData, &normalGSConstants, sizeof(normalGSConstants));
		g_pDeviceContext->Unmap(normalGSConstantBuffer.Get(), 0);

		// Set geometry shader constant buffer
		pConstantBuffers[0] = normalGSConstantBuffer.Get();
		g_pDeviceContext->GSSetConstantBuffers(0, ARRAYSIZE(pConstantBuffers), pConstantBuffers);

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
							// set primitive topology
							g_pDeviceContext->IASetPrimitiveTopology(primitive.m_topology);
							primitive.Draw(g_pDeviceContext.Get());

							if (bDrawNormals)
							{
								// set input layout
								g_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
								g_pDeviceContext->GSSetShader(gfx.m_geoShader.normal.Get(), nullptr, 0);
								g_pDeviceContext->PSSetShader(gfx.m_pixShader.normal.Get(), nullptr, 0);
								primitive.Draw(g_pDeviceContext.Get());
								g_pDeviceContext->GSSetShader(nullptr, nullptr, 0);
								g_pDeviceContext->PSSetShader(gfx.m_pixShader.basic.Get(), nullptr, 0);
							}
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

		ImGui::Text("Draw normals");
		ImGui::Checkbox("On", &bDrawNormals);
		ImGui::SameLine();
		ImGui::SliderFloat("Scale", &normalGSConstants.Scale, 0.0f, 0.01f);
		ImGui::End();

		ImGui::Begin("Images");
		for (const auto& image : gltfModel.m_images)
		{
			ImGui::Image(image.Get(), ImVec2(256, 256));
		}
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
			using namespace sturdy_guacamole;

			UINT newWidth = LOWORD(lParam);
			UINT newHeight = HIWORD(lParam);

			sturdy_guacamole::Win32App::Get().m_width = newWidth;
			sturdy_guacamole::Win32App::Get().m_height = newHeight;

			g_pDeviceContext->OMSetRenderTargets(0U, nullptr, nullptr);
			g_pDeviceContext->Flush();

			// Release all outstanding references to the swap chain's buffers
			Graphics::Get().m_rtview.main = nullptr;
			Graphics::Get().m_dsview.main = nullptr;

			// Create the main render target view
			g_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

			ComPtr<ID3D11Texture2D> backBuffer;
			g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

			g_pDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &Graphics::Get().m_rtview.main);

			// Create the main depth stencil view
			ComPtr<ID3D11Texture2D> dsBuffer{};
			CD3D11_TEXTURE2D_DESC dsBufDesc{ DXGI_FORMAT_D24_UNORM_S8_UINT, newWidth, newHeight, 1u, 0u, D3D11_BIND_DEPTH_STENCIL };
			ThrowIfFailed(g_pDevice->CreateTexture2D(&dsBufDesc, nullptr, &dsBuffer));

			CD3D11_DEPTH_STENCIL_VIEW_DESC dsviewDesc{ D3D11_DSV_DIMENSION_TEXTURE2D };

			D3D11_DEPTH_STENCIL_DESC dsDesc{};
			ThrowIfFailed(g_pDevice->CreateDepthStencilView(dsBuffer.Get(), &dsviewDesc, &Graphics::Get().m_dsview.main));

			// Set new viewport
			CD3D11_VIEWPORT viewport{ 0.0F, 0.0F, static_cast<float>(newWidth), static_cast<float>(newHeight) };
			g_pDeviceContext->RSSetViewports(1, &viewport);

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
