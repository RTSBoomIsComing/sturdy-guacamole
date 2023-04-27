#pragma once

#include <directxtk/SimpleMath.h>

#include <memory>
#include <vector>
#include <string>

namespace tinygltf
{
	class Model;
	class Node;
}

namespace sturdy_guacamole
{
	// forward declaration
	class Mesh;

	// glTF scene node
	class SceneNode
	{
	public:
		SceneNode(const tinygltf::Node& node, const std::vector<std::shared_ptr<Mesh>>& meshes, const DirectX::SimpleMath::Matrix& parentGlobalTransform);
		~SceneNode() = default;

		std::string m_name{};

		// there is no reason to have TRS, only global transform is desired
		DirectX::SimpleMath::Matrix m_globalTransform{};

		std::shared_ptr<Mesh> m_mesh{};
	};
}

