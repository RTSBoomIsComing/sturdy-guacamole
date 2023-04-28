#pragma once
#include "Scene.h"
#include "Mesh.h"

#include <vector>
#include <filesystem>

namespace tinygltf
{
	class Model;
}

namespace sturdy_guacamole
{
	class GLTFModel
	{
	public:
		struct Node
		{
			std::string m_name{};
			const Mesh* m_mesh{};
			std::vector<const Node*> m_children{};

			DirectX::SimpleMath::Vector3 m_scale{};
			DirectX::SimpleMath::Quaternion m_rotation{};
			DirectX::SimpleMath::Vector3 m_translation{};
			DirectX::SimpleMath::Matrix m_matrix{};
		};
	public:
		GLTFModel(const std::filesystem::path& path);
		~GLTFModel() = default;


		std::vector<Node> m_nodes{};
		std::vector<Mesh> m_meshes{};
		std::vector<Scene> m_scenes{};

	private:
		bool LoadModel(tinygltf::Model& outModel, const std::filesystem::path& path);
	};
}