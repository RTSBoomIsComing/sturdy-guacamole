#include "SceneNode.h"
#include "Mesh.h"

#include <tiny_gltf.h>

sturdy_guacamole::SceneNode::SceneNode(const tinygltf::Node& node, std::shared_ptr<SceneNode> parent, const std::vector<Mesh>& meshes)
	: m_name{ node.name }, m_parent{ std::move(parent) }
{
	using namespace DirectX::SimpleMath;
	
	Matrix parentGlobalTransform = (m_parent) ? m_parent->m_globalTransform : Matrix::Identity;
	Matrix localTransform{}; // Identity matrix
	for (size_t i{}; i < node.matrix.size(); i++)
	{
		localTransform.m[i / 4][i % 4] = static_cast<float>(node.matrix[i]);
	}
	if (node.scale.size() > 0)
		localTransform *= Matrix::CreateScale(static_cast<float>(node.scale[0]), static_cast<float>(node.scale[1]), static_cast<float>(node.scale[2]));

	if (node.rotation.size() > 0)
		localTransform *= Matrix::CreateFromQuaternion(Quaternion(static_cast<float>(node.rotation[0]), static_cast<float>(node.rotation[1]), static_cast<float>(node.rotation[2]), static_cast<float>(node.rotation[3])));

	if (node.translation.size() > 0)
		localTransform *= Matrix::CreateTranslation(static_cast<float>(node.translation[0]), static_cast<float>(node.translation[1]), static_cast<float>(node.translation[2]));

	m_globalTransform = parentGlobalTransform * localTransform;


	size_t meshIdx = static_cast<size_t>(node.mesh);
	if (meshIdx < meshes.size())
	{
		m_mesh = &meshes[meshIdx];
	}
}

