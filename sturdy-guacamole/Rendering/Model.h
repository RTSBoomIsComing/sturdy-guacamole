#pragma once
#include "../Group.h"
#include "Material.h"

#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include <directxtk/SimpleMath.h>


#include <vector>
#include <string>
#include <filesystem>
#include <optional>
#include <stdint.h>


namespace sturdy_guacamole
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
}

namespace sturdy_guacamole
{
	struct Name {
		std::string str{};
	};

	struct Transform
	{
		DirectX::SimpleMath::Matrix matrix{};
	};

	struct alignas(16) Translation
	{
		DirectX::SimpleMath::Vector3 vec3{};
	};

	struct Rotation { 
		DirectX::SimpleMath::Quaternion quat{};
	};

	struct alignas(16) Scale {
		DirectX::SimpleMath::Vector3 vec3{ 1.0f, 1.0f, 1.0f };
	};

	/*
	* https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#nodes-and-hierarchy
	* The node hierarchy MUST be a set of disjoint strict trees.
	* That is node hierarchy MUST NOT contain cycles and each node MUST have zero or one parent node.
	*
	* A node is called a root node when it doesn¡¯t have a parent.
	*/
	struct Parent {
		std::optional<uint16_t> id{};
	};

	struct Children
	{
		std::uint32_t count{};
		std::optional<uint16_t> first{};
	};

	struct Sibling
	{
		std::optional<uint16_t> prev{};
		std::optional<uint16_t> next{};
	};

}

namespace sturdy_guacamole::rendering
{
	using NodeGroup = Group<Name, Transform, Translation, Rotation, Scale, Parent, Children, Sibling>;

	/*
	* The Model class contains a collection of Scenes, Animations, Skins, Cameras, Materials, and Meshes.
	* The Model class also contains a collection of data like ID3D11Buffer, ID3D11ShaderResourceView, and ID3D11SamplerState.
	* The Model class is designed according to data-oriented design.
	*/
	class Model
	{
	public:
		Model(const std::filesystem::path& file_name);
		~Model() = default;

		NodeGroup m_nodes{};


		//struct
		//{
		//	std::vector<std::string> name{};
		//	std::vector<std::optional<uint16_t>> first_primitive{};
		//} m_mesh_list;

		//struct
		//{
		//	std::vector<std::optional<uint16_t>> next_sibling{};
		//	std::vector<std::optional<uint16_t>> material{};
		//	std::vector<IA_Preset> ia_preset{};
		//} m_primitive_list;

		//struct
		//{
		//	std::vector<std::string>	name{};
		//	std::vector<Material>		info{};
		//	std::vector<PS_Preset>		ps_preset{};
		//} m_material_list;

		//

		//struct
		//{
		//	std::vector<std::string> name{};
		//	std::vector<std::optional<uint16_t>> sampler{};
		//	std::vector<std::optional<uint16_t>> image{};
		//} m_texture_list;

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
		//using BufferView = std::variant<ID3D11Buffer*, std::span<float>>;
		//std::vector<BufferView> buffer_view_list;

		//std::vector<float> m_float_data_buffer{};

		//struct
		//{
		//	std::vector<ComPtr<ID3D11Buffer>>				buffer{};
		//	std::vector<ComPtr<ID3D11InputLayout>>			input_layout{};
		//	std::vector<ComPtr<ID3D11ShaderResourceView>>	srview{};
		//	std::vector<ComPtr<ID3D11SamplerState>>			sampler{};
		//} m_d3d11_resource_list;
	};

}
