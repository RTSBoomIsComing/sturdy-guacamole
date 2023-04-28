#pragma once
//#include "Scene.h"
#include "Mesh.h"

#include <directxtk/SimpleMath.h>

#include <vector>
#include <filesystem>

namespace tinygltf
{
	class Model;
	class Node;
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

			DirectX::SimpleMath::Vector3 m_scale{ 1.0f, 1.0f, 1.0f };
			DirectX::SimpleMath::Quaternion m_rotation{};
			DirectX::SimpleMath::Vector3 m_translation{};
			DirectX::SimpleMath::Matrix m_matrix{};
		};

		struct DFSNode
		{
			const Node* m_node{};
			const Node* m_parent{}; // nullptr if root
			DirectX::SimpleMath::Matrix m_globalTransform{};
		};
		using DFSList = std::vector<DFSNode>;

		struct Scene
		{
			std::string m_name{};
			std::vector<const Node*> m_roots{};

			std::vector<DFSList> m_dfsLists{};
		};

	public:
		GLTFModel(const std::filesystem::path& path);
		~GLTFModel() = default;


		std::vector<Node> m_nodes{};
		std::vector<Mesh> m_meshes{};
		std::vector<Scene> m_scenes{};

	private:
		bool LoadModel(tinygltf::Model& outModel, const std::filesystem::path& path);
		void PopulateNodes(const tinygltf::Model& tinyModel);
		//void DFS
		void GenerateDFSList(const Node* node, const DFSNode* parent, DFSList& dfsList) const;

	};
}