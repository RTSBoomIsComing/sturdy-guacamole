#include "SceneNode.h"
#include "Mesh.h"

#include <tiny_gltf.h>

sturdy_guacamole::SceneNode::SceneNode(const tinygltf::Node& node, std::shared_ptr<SceneNode> parent)
	: m_name{ node.name }, m_parent{ std::move(parent) }
{
	using namespace DirectX::SimpleMath;
	
	Matrix parentGlobalTransform = (m_parent) ? m_parent->m_globalTransform : Matrix::Identity;
	Matrix localTransform{}; // Identity matrix

	m_globalTransform = parentGlobalTransform * localTransform;
}

