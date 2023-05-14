#pragma once
#include "Material.h"

#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include <directxtk/SimpleMath.h>

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
	* The Model class also contains a collection of data like ID3D11Buffer, ID3D11ShaderResourceView, and ID3D11SamplerState.
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
			std::vector<unsigned int> nodes{};
		} m_scene_list;

		struct
		{
			std::vector<std::string> name{};
			std::vector<DirectX::SimpleMath::Vector3>		translation{};
			std::vector<DirectX::SimpleMath::Quaternion>	rotation{};
			std::vector<DirectX::SimpleMath::Vector3>		scale{};
			std::vector<DirectX::SimpleMath::Matrix>		matrix{};

			std::vector<std::vector<uint16_t>>		children{};

			/*
			* https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#nodes-and-hierarchy
			* The node hierarchy MUST be a set of disjoint strict trees.
			* That is node hierarchy MUST NOT contain cycles and each node MUST have zero or one parent node.
			* 
			* A node is called a root node when it doesn¡¯t have a parent.
			*/
			std::vector<std::optional<uint16_t>>	parent{};

			std::vector<std::optional<uint16_t>>	mesh{};

			// TODO : implement skin and morph animation
			//std::vector<std::optional<uint16_t>>	skin{};
			//std::vector<std::vector<float>>		weights{};
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

			// id of m_d3d11_resource_list.sampler
			std::vector<std::optional<uint16_t>> sampler{};

			// id of m_d3d11_resource_list.srview
			std::vector<std::optional<uint16_t>> image{};
		} m_texture_list;

		// TODO : implement animation, skin
		//struct
		//{

		//} animation_list;

		//struct
		//{

		//} skin_list;

		/*
		* The BufferView does not represent of glTF 2.0 itself
		* 
		* 
		*/
		using BufferView = std::variant<ID3D11Buffer*, std::span<float>>;
		std::vector<BufferView> buffer_view_list;

		std::vector<float> m_float_data_buffer{};

		struct
		{
			std::vector<ComPtr<ID3D11Buffer>>				buffer{};
			std::vector<ComPtr<ID3D11InputLayout>>			input_layout{};
			std::vector<ComPtr<ID3D11ShaderResourceView>>	srview{};
			std::vector<ComPtr<ID3D11SamplerState>>			sampler{};
		} m_d3d11_resource_list;
	};

}
