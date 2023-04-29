#pragma once
#include <directxtk/SimpleMath.h>

#include <string>

namespace tinygltf
{
	class Node;
}

namespace sturdy_guacamole
{
	class GLTFModel;
	class Mesh;

	class GLTFNode
	{
	public:
		GLTFNode() = default;
		GLTFNode(const tinygltf::Node& tinyNode, const sturdy_guacamole::GLTFModel& myModel);
		~GLTFNode() = default;

		std::string m_name{};
		const Mesh* m_mesh{};
		std::vector<const GLTFNode*> m_children{};
		DirectX::SimpleMath::Vector3 m_scale{ 1.0f, 1.0f, 1.0f };
		DirectX::SimpleMath::Quaternion m_rotation{};
		DirectX::SimpleMath::Vector3 m_translation{};
		DirectX::SimpleMath::Matrix m_matrix{};
	};
} 
