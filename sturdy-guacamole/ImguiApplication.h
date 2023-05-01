#pragma once
#include <imgui.h>

struct ID3D11Device;
struct ID3D11DeviceContext;

namespace sturdy_guacamole
{
	class ImguiApplication
	{
	public:
		ImguiApplication(void* hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		~ImguiApplication();
		
		// Start the Dear ImGui frame
		void NewFrame();
	};

	using ImguiApp = ImguiApplication;
}


