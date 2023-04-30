#pragma once
#include "Singleton.h"

#include <d3d11.h>
#include <filesystem>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

namespace sturdy_guacamole
{
	class Graphics : public Singleton<Graphics>
	{
	public:
		Graphics();
		~Graphics() = default;

	public:
		struct
		{
			ComPtr<ID3DBlob> basic{};
		} vsBlob;

		struct
		{
			ComPtr<ID3D11VertexShader> basic{};
		} vtxShader;

		struct
		{
			ComPtr<ID3D11PixelShader> basic{};
		} pixShader;

		struct
		{
			ComPtr<ID3D11RasterizerState> solid{};
			ComPtr<ID3D11RasterizerState> wireframe{};
		} rasterizerState;
	};
}
