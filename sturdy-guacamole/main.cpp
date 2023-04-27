#include "Win32Application.h"
#include "ImguiApplication.h"
#include "Dx11Application.h"
#include "Graphics.h"

// include my glTF to DirectX11 converter libraries
#include "MeshPrimitive.h"
#include "SceneNode.h"
#include "ConstantBuffers.h"

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
bool LoadModel(tinygltf::Model* model, const char* filename);
int main()
{
	using namespace DirectX::SimpleMath;

	// Initialize win32 application singleton instance
	sturdy_guacamole::Win32Application win32App{};

	// Set WndProc for the application window
	::SetWindowLongPtrW(win32App.GetWindowHandle(), GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc));

	// Initialize dx11 application singleton instance
	sturdy_guacamole::Dx11Application dx11App{ win32App.GetWindowHandle() };

	// Create imgui application
	sturdy_guacamole::ImguiApplication imguiApp{ win32App.GetWindowHandle(), dx11App.GetDevice(), dx11App.GetDeviceContext() };

	// Initialize mouse singleton instance
	std::unique_ptr<DirectX::Mouse> mouse = std::make_unique<DirectX::Mouse>();
	mouse->SetWindow(win32App.GetWindowHandle());
	DirectX::Mouse::ButtonStateTracker mouse_tracker;

	// Initialize keyboard singleton instance
	std::unique_ptr<DirectX::Keyboard> keyboard = std::make_unique<DirectX::Keyboard>();
	DirectX::Keyboard::KeyboardStateTracker kb_tracker;

	// Test code

	// Initialize Graphics singleton instance
	sturdy_guacamole::Graphics gfx{};

	// Create Texture2D for RenderTarget
	CD3D11_TEXTURE2D_DESC textureDesc{ DXGI_FORMAT_R16G16B16A16_FLOAT, 1280, 960, 1, 0,
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
	CD3D11_VIEWPORT viewport{ 0.0F, 0.0F, float(1280), float(960) };
	g_pDeviceContext->RSSetViewports(1, &viewport);

	// tiny_gltf loader
	tinygltf::Model model;
	// L"D:\\GitHub\\glTF-Sample-Models\\2.0\\ABeautifulGame\\glTF\\ABeautifulGame.gltf"
	// L"D:\\GitHub\\glTF-Sample-Models\\2.0\\Cube\\glTF\\Cube.gltf"
	// L"D:\\GitHub\\glTF-Sample-Models\\2.0\\Triangle\\glTF\\Triangle.gltf"
	std::filesystem::path gltfPath{ L"D:\\GitHub\\glTF-Sample-Models\\2.0\\ABeautifulGame\\glTF\\ABeautifulGame.gltf" };
	bool res = LoadModel(&model, gltfPath.string().data());
	if (res == false)
		throw std::runtime_error("failed to load gltf");


	std::vector<sturdy_guacamole::SceneNode> sceneNodes{};

	auto processElements = [&](const tinygltf::Node& node, const Matrix& parentGlobalTransform)
		-> const sturdy_guacamole::SceneNode&
	{
		std::cout << "node name: " << node.name << std::endl;
		sturdy_guacamole::SceneNode sceneNode{};

		for (size_t i{}; i < node.matrix.size(); i++)
		{
			sceneNode.m_globalTransform.m[i / 4][i % 4] = static_cast<float>(node.matrix[i]);
		}
		if (node.scale.size() > 0)
			sceneNode.m_globalTransform *= Matrix::CreateScale(static_cast<float>(node.scale[0]), static_cast<float>(node.scale[1]), static_cast<float>(node.scale[2]));
		
		if (node.rotation.size() > 0)
			sceneNode.m_globalTransform *= Matrix::CreateFromQuaternion(Quaternion(static_cast<float>(node.rotation[0]), static_cast<float>(node.rotation[1]), static_cast<float>(node.rotation[2]), static_cast<float>(node.rotation[3])));
	
		if (node.translation.size() > 0)
			sceneNode.m_globalTransform *= Matrix::CreateTranslation(static_cast<float>(node.translation[0]), static_cast<float>(node.translation[1]), static_cast<float>(node.translation[2]));
		
		sceneNode.m_globalTransform = parentGlobalTransform * sceneNode.m_globalTransform;
		sceneNodes.push_back(sceneNode);

		if (static_cast<size_t>(node.mesh) < model.meshes.size())
		{
			const auto& mesh = model.meshes[node.mesh];
			std::cout << "mesh name: " << mesh.name << std::endl;
		}

		return sceneNode;
	};

	std::function<void(const tinygltf::Node&, const Matrix&)> traverseNode{};
	traverseNode = [&](const tinygltf::Node& node, const Matrix& parentGlobalTransform)
	{
		// process node
		const auto& currSceneNode = processElements(node, parentGlobalTransform);

		// traverse child nodes
		for (const auto& childNode : node.children)
		{
			assert(static_cast<size_t>(childNode) < model.nodes.size());
			traverseNode(model.nodes[childNode], currSceneNode.m_globalTransform);
		}
	};

	// scene graph traversal
	for (const auto& rootNode : model.scenes[model.defaultScene].nodes)
	{
		assert(static_cast<size_t>(rootNode) < model.nodes.size());

		traverseNode(model.nodes[rootNode], Matrix::Identity);
	}

	// create vector of MeshPrimitive
	std::vector<sturdy_guacamole::MeshPrimitive> meshPrimitives{};
	for (const auto& mesh : model.meshes)
	{
		// process primitive and render it
		for (const auto& primitive : mesh.primitives)
		{
			meshPrimitives.emplace_back(model, primitive);

			// Set the input layout
			// g_pDeviceContext->IASetInputLayout(inputLayout.Get());

		}
	}

	// Test end

	Vector3 viewerPos{ 0.0f, 0.0f, 1.0f };
	Vector3 viewerRot{ 0.0f, 0.0f, 0.0f };

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


		//if (kb_tracker.pressed.A)
		//	std::cout << "key A pressed" << std::endl;

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
		ID3D11RenderTargetView* ppRenderTargetViews[]
		{
			g_pRenderTargetView
		};
		g_pDeviceContext->OMSetRenderTargets(ARRAYSIZE(ppRenderTargetViews), ppRenderTargetViews, nullptr);
		g_pDeviceContext->ClearRenderTargetView(g_pRenderTargetView, clear_color);

		

		// create view, projection matrix
		//Matrix viewMatrix = Matrix::CreateLookAt(viewerPos, Vector3::Zero, Vector3::UnitY);
		Matrix viewMatrix = DirectX::XMMatrixLookToRH(viewerPos, viewerForward, viewerUp);
		Matrix projMatrix = Matrix::CreatePerspectiveFieldOfView(
			DirectX::XMConvertToRadians(90.0f), 1280.0f / 960.0f, 0.1f, 100.0f);

		sturdy_guacamole::CommonConstants commonConstants{};
		commonConstants.ViewerPos = Vector4{ viewerPos };
		commonConstants.ViewMatrix = viewMatrix;
		commonConstants.ProjMatrix = projMatrix;
		commonConstants.ViewProjMatrix = (viewMatrix * projMatrix);

		// Start rendering



		D3D11_MAPPED_SUBRESOURCE mappedResource{};
		g_pDeviceContext->Map(commonConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		::memcpy(mappedResource.pData, &commonConstants, sizeof(commonConstants));
		g_pDeviceContext->Unmap(commonConstantBuffer.Get(), 0);



		// Set constant buffer
		ID3D11Buffer* pConstantBuffers[]
		{
			meshConstantBuffer.Get(),
			commonConstantBuffer.Get(),
		};
		g_pDeviceContext->VSSetConstantBuffers(0, ARRAYSIZE(pConstantBuffers), pConstantBuffers);




		// Set pipeline state
		static int primitiveIdx{};
		

		for (const auto& sceneNode : sceneNodes)
		{
			// Create mesh constants
			sturdy_guacamole::MeshConstants meshConstants{ sceneNode.m_globalTransform };
			
			D3D11_MAPPED_SUBRESOURCE mappedResource{};
			g_pDeviceContext->Map(meshConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			::memcpy(mappedResource.pData, &meshConstants, sizeof(meshConstants));
			g_pDeviceContext->Unmap(meshConstantBuffer.Get(), 0);

			meshPrimitives[primitiveIdx].Draw(g_pDeviceContext);
		}

		// Rendering
		//for (const auto& meshPrimitive : meshPrimitives)
		//{
		//	meshPrimitive.Draw(g_pDeviceContext);
		//}


		// Start the Dear ImGui frame
		imguiApp.NewFrame();
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

		ImGui::Begin("Details Panel");
		ImGui::Text("Hello world!");
		ImGui::Text("fps: %f", ImGui::GetIO().Framerate);
		ImGui::Text("delta time: %f", ImGui::GetIO().DeltaTime);
		ImGui::DragFloat3("viewerRot", reinterpret_cast<float*>(&viewerRot));
		ImGui::DragInt("Select MeshPrimitive number", &primitiveIdx, 0.1F, 0, (UINT)meshPrimitives.size() - 1);
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

bool LoadModel(tinygltf::Model* model, const char* filename) {
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	bool res = loader.LoadASCIIFromFile(model, &err, &warn, filename);
	if (!warn.empty()) {
		std::cout << "WARN: " << warn << std::endl;
	}

	if (!err.empty()) {
		std::cout << "ERR: " << err << std::endl;
	}

	if (!res)
		std::cout << "Failed to load glTF: " << filename << std::endl;
	else
		std::cout << "Loaded glTF: " << filename << std::endl;

	return res;
}