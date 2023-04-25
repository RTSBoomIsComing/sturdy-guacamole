#include "Win32Application.h"
#include "ImguiApplication.h"
#include "Dx11Application.h"
#include "Graphics.h"
#include "MeshPrimitive.h"

// Include tiny_gltf
#pragma warning( disable : 4996 )
#undef min
#undef max
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include <tiny_gltf.h>

// Include DirectXMath
#include <DirectXMath.h>

// Include standard library headers
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <format>

namespace sturdy_guacamole
{

}

// Forward declarations
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API void ImGui_ImplDX11_RenderDrawData(ImDrawData* draw_data);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
std::shared_ptr<tinygltf::Model> LoadModel(const char* filename);
int main()
{
	// Initialize win32 application singleton instance
	sturdy_guacamole::Win32Application win32App{};

	// Set WndProc for the application window
	::SetWindowLongPtrW(win32App.GetWindowHandle(), GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc));

	// Initialize dx11 application singleton instance
	sturdy_guacamole::Dx11Application dx11App{ win32App.GetWindowHandle() };

	// Create imgui application
	sturdy_guacamole::ImguiApplication imguiApp{ win32App.GetWindowHandle(), dx11App.GetDevice(), dx11App.GetDeviceContext() };


	// Test code

	// Initialize Graphics singleton instance
	sturdy_guacamole::Graphics gfx{};

	// Create Texture2D for RenderTarget
	CD3D11_TEXTURE2D_DESC textureDesc{ DXGI_FORMAT_R16G16B16A16_FLOAT, 1024, 768, 1, 0,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE };

	ComPtr<ID3D11Texture2D> renderTarget;
	g_pDevice->CreateTexture2D(&textureDesc, nullptr, &renderTarget);

	// Create RenderTargetView
	ComPtr<ID3D11RenderTargetView> renderTargetView;
	g_pDevice->CreateRenderTargetView(renderTarget.Get(), nullptr, &renderTargetView);

	// Create ShaderResourceView
	ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	g_pDevice->CreateShaderResourceView(renderTarget.Get(), nullptr, &shaderResourceView);

	// Set the render target
	g_pDeviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), nullptr);

	// clear the render target
	const float clear_color[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
	g_pDeviceContext->ClearRenderTargetView(renderTargetView.Get(), clear_color);

	// Set the viewport
	CD3D11_VIEWPORT viewport{ 0.0F, 0.0F, float(1024), float(768) };
	g_pDeviceContext->RSSetViewports(1, &viewport);

	// tiny_gltf loader
	std::shared_ptr<tinygltf::Model> model;
	try
	{
		model = LoadModel("D:\\Assets\\glTF\\SimpleTriangle\\SimpleTriangle.gltf");
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	// create vector of MeshPrimitive
	std::vector<sturdy_guacamole::MeshPrimitive> meshPrimitives{};

	auto& defaultScene = model->scenes[model->defaultScene];
	for (size_t i{}; i < defaultScene.nodes.size(); i++)
	{
		auto& rootNode = model->nodes[defaultScene.nodes[i]];
		if (0 <= rootNode.mesh && rootNode.mesh < model->meshes.size())
		{
			// process mesh
			auto& mesh = model->meshes[rootNode.mesh];

			// process primitive and render it
			for (const auto& primitive : mesh.primitives)
			{
				meshPrimitives.emplace_back(*model, primitive);
			
				g_pDeviceContext->IASetVertexBuffers(0, 1, meshPrimitives.back().m_vertexBuffer.GetAddressOf(), 
					&meshPrimitives.back().m_vertexBufferStride, &meshPrimitives.back().m_vertexBufferOffset);

				// Set the index buffer
				g_pDeviceContext->IASetIndexBuffer(meshPrimitives.back().m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

				// Set the input layout
				// g_pDeviceContext->IASetInputLayout(inputLayout.Get());

			}
		}
	}


	// Test end


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

		// Start rendering
		
		// Clear the back buffer
		ID3D11RenderTargetView* ppRenderTargetViews[]
		{
			g_pRenderTargetView
		};
		g_pDeviceContext->OMSetRenderTargets(ARRAYSIZE(ppRenderTargetViews), ppRenderTargetViews, nullptr);
		g_pDeviceContext->ClearRenderTargetView(g_pRenderTargetView, clear_color);


		// Set pipeline state
		// ...
		
		// Rendering
		g_pDeviceContext->DrawIndexed(3, 0, 0);

		// Start the Dear ImGui frame
		imguiApp.NewFrame();
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

		ImGui::Begin("Details Panel");
		ImGui::Text("Hello world!");
		ImGui::Text("fps: %f", ImGui::GetIO().Framerate);
		ImGui::Text("delta time: %f", ImGui::GetIO().DeltaTime);
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

std::shared_ptr<tinygltf::Model> LoadModel(const char* filename) {
	auto model = std::make_shared<tinygltf::Model>();
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	bool res = loader.LoadASCIIFromFile(model.get(), &err, &warn, filename);
	if (!warn.empty()) {
		std::cout << "WARN: " << warn << std::endl;
	}

	if (!err.empty()) {
		std::cout << "ERR: " << err << std::endl;
	}

	if (!res)
		throw std::runtime_error(std::format("Failed to load glTF: {}", filename));
	else
		std::cout << "Loaded glTF: " << filename << std::endl;

	return model;
}