#pragma once
#include "Mesh.h"

#include <directxtk/SimpleMath.h>

#include <vector>
#include <string>
#include <optional>

namespace sturdy_guacamole::rendering
{
	/*
	* The Model class represents a glTF 2.0 document. and correspond to tinygltf::Model
	* The Model class contains a collection of Scenes, Animations, Skins, Cameras, Materials, and Meshes.
	* The Model class also contains a collection of data like ID3D11Buffer, ID3D11Texture2D, and ID3D11SamplerState.
	* The Model class is designed according to data-oriented design.
	* All of these data are released when the Model class is destroyed.
	* TODO: The Model class is not movable or copyable.
	*/
	class Model
	{
	public:
		Model() = default;
		~Model() = default;

		struct
		{
			std::vector<std::string> name{};
			std::vector<unsigned int> root_id{};
		}m_scene_list;

		struct
		{
			std::vector<std::string> name{}; // Treat an empty string as null
			std::vector<DirectX::SimpleMath::Vector3> translation{};
			std::vector<DirectX::SimpleMath::Quaternion> rotation{};
			std::vector<DirectX::SimpleMath::Vector3> scale{};
			std::vector<DirectX::SimpleMath::Matrix> matrix{};
			
			std::vector<std::vector<unsigned int>> children_id{};
			std::vector<std::optional<unsigned int>> parent_id{}; // For convenience

			std::vector<std::optional<unsigned int>> mesh_id{};
		} m_node_list;

		struct
		{
			std::vector<std::string>	name{}; // Treat an empty string as null
			std::vector<Mesh>			data{};
		} m_mesh_list;


		struct
		{

			std::vector<std::string>	name{};	// Treat an empty string as null

		} m_material;


	};

}
