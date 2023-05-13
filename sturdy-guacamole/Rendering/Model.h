#pragma once
#include "Material.h"

#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include <vector>
#include <string>
#include <optional>
#include <variant>
#include <span>
#include <stdint.h>

namespace sturdy_guacamole::rendering
{
	struct IA_Preset
	{
		// ID3D11DeviceContext::IASetIndexBuffer();
		struct {
			ID3D11Buffer* buffer{};
			DXGI_FORMAT	format{ DXGI_FORMAT_R16_UINT };
			UINT offset{};
			UINT count{};
		} index;

		// ID3D11DeviceContext::IASetVertexBuffers();
		struct {
			std::vector<ID3D11Buffer*>	buffer_array{};
			std::vector<UINT> stride_array{};
			std::vector<UINT> offset_array{};
			UINT count{};
		} vertex;

		// ID3D11Device::IASetInputLayout();
		ID3D11InputLayout* input_layout{};

		// ID3D11DeviceContext::IASetPrimitiveTopology();
		D3D11_PRIMITIVE_TOPOLOGY topology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	};

	struct PS_Preset
	{
		// ID3D11DeviceContext::PSSetShaderResources();
		// [0] = baseColorTexture, [1] = metallicRoughnessTexture, [2] = normalTexture, [3] = occlusionTexture, [4] = emissiveTexture
		std::vector<ID3D11ShaderResourceView*> srview_array{};

		// ID3D11DeviceContext::PSSetSamplers();
		std::vector<ID3D11SamplerState*> sampler_array{};
	};

	/*
	* The Model class contains a collection of Scenes, Animations, Skins, Cameras, Materials, and Meshes.
	* The Model class also contains a collection of data like ID3D11Buffer, ID3D11Texture2D, and ID3D11SamplerState.
	* The Model class is designed according to data-oriented design.
	* All of these data are released when the Model class is destroyed.
	*/
	class Model
	{
	public:
		Model() = default;
		~Model() = default;

		struct
		{
			std::vector<std::string> name{};
			std::vector<unsigned int> root{};
		} m_scene_list;

		struct
		{
			std::vector<std::string> name{};
			std::vector<std::array<float, 3>>		translation{};
			std::vector<std::array<float, 4>>		rotation{};
			std::vector<std::array<float, 3>>		scale{};
			std::vector<std::array<float, 16>>		matrix{};

			std::vector<std::vector<uint16_t>>		children{};
			std::vector<std::optional<uint16_t>>	parent{}; // For convenience

			std::vector<std::optional<uint16_t>>	mesh{};
			std::vector<std::optional<uint16_t>>	skin{};
			std::vector<std::vector<float>>			weights{};
		} m_node_list;

		struct
		{
			std::vector<std::string> name{};
			std::vector<uint16_t> primitive{};
		} m_mesh_list;

		struct
		{
			std::vector<IA_Preset> ia_preset{};
			std::optional<uint16_t>	material{};
		} m_primitive_list;

		struct
		{
			std::vector<std::string>	name{};
			std::vector<Material>		info{};
			std::vector<PS_Preset>		preset{};
		} m_material_list;

		struct
		{
			std::vector<std::string> name{};
			std::vector<std::optional<uint16_t>> sampler{};
			std::vector<std::optional<uint16_t>> source{};
		} m_texture_list;

		// TODO : implement animation, skin
		//struct
		//{

		//} animation_list;

		//struct
		//{

		//} skin_list;

		using Accessor = std::variant<ID3D11Buffer*, std::span<float>>;
		std::vector<Accessor> accessor_list;

		std::vector<float> m_float_data_buffer{};

		struct
		{
			std::vector<ComPtr<ID3D11Buffer>> buffer{};
			std::vector<ComPtr<ID3D11InputLayout>> input_layout{};
			std::vector<ComPtr<ID3D11ShaderResourceView>> srview{};
			std::vector<ComPtr<ID3D11SamplerState>> sampler{};
		} m_d3d11_resource_list;
	};

}
