#include "GLTFNode.h"
#include "GLTFModel.h"
#include "Mesh.h"
#include <tiny_gltf.h>

sturdy_guacamole::GLTFNode::GLTFNode(const tinygltf::Node& tinyNode, const sturdy_guacamole::GLTFModel& myModel)
	:
	m_name{ tinyNode.name }
{
	using namespace DirectX::SimpleMath;

	if (static_cast<size_t>(tinyNode.mesh) < myModel.m_meshes.size())
		m_mesh = &myModel.m_meshes[tinyNode.mesh];

	for (int i : tinyNode.children)
	{
		m_children.push_back(&myModel.m_nodes[i]);
	}

	if (tinyNode.scale.size() > 0)
		m_scale = { (float)tinyNode.scale[0], (float)tinyNode.scale[1], (float)tinyNode.scale[2] };

	if (tinyNode.rotation.size() > 0)
		m_rotation = { (float)tinyNode.rotation[0], (float)tinyNode.rotation[1],(float)tinyNode.rotation[2], (float)tinyNode.rotation[3] };

	if (tinyNode.translation.size() > 0)
		m_translation = { (float)tinyNode.translation[0], (float)tinyNode.translation[1], (float)tinyNode.translation[2] };

	for (size_t j{}; j < tinyNode.matrix.size(); j++)
	{
		m_matrix.m[j / 4][j % 4] = static_cast<float>(tinyNode.matrix[j]);
	}

	// if no matrix, construct from TRS
	if (tinyNode.matrix.empty())
	{
		auto S = Matrix::CreateScale(m_scale);
		auto R = Matrix::CreateFromQuaternion(m_rotation);
		auto T = Matrix::CreateTranslation(m_translation);
		m_matrix = S * R * T;
	}

}