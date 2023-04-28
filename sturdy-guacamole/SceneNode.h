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
		SceneNode(const tinygltf::Node& node, std::shared_ptr<SceneNode> parent, const std::vector<Mesh>& meshes);
		~SceneNode() = default;

		std::string m_name{};
		std::shared_ptr<SceneNode> m_parent{};

		// there is no reason to have TRS, only global transform is desired
		DirectX::SimpleMath::Matrix m_globalTransform{};

		const Mesh* m_mesh{};
	};
}

