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
			ComPtr<ID3D11VertexShader> basic{};
			ComPtr<ID3DBlob> basic_blob{};
		} m_vtxShader;

		struct
		{
			ComPtr<ID3D11PixelShader> basic{};
		} m_pixShader;

		struct
		{
			ComPtr<ID3D11RasterizerState> solid{};
			ComPtr<ID3D11RasterizerState> wireframe{};
		} m_rsstate; // rasterizer state

		struct
		{
			ComPtr<ID3D11DepthStencilState> basic{};
		} m_dsstate; // depth stencil state

		struct
		{
			ComPtr<ID3D11RenderTargetView> main{};
		} m_rtview; // render target view

		struct
		{
			ComPtr<ID3D11DepthStencilView> main{};
		} m_dsview; // depth stencil view

	private:
		void InitVertexShaders(std::filesystem::path baseDir);
		void InitPixelShaders(std::filesystem::path baseDir);
		void InitRasterizerStates();
		void InitDepthStencilStates();
		void InitRenderTargetViews();
		void InitDepthStencilViews();
	};
}
