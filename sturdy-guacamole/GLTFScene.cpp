#include "GLTFScene.h"
#include "GLTFModel.h"

#include <tiny_gltf.h>

sturdy_guacamole::GLTFScene::GLTFScene(const tinygltf::Scene& tinyScene, const GLTFModel& myModel)
	: m_name{ tinyScene.name }
{
	m_traversals.resize(tinyScene.nodes.size());
	for (size_t i{}; i < tinyScene.nodes.size(); i++)
	{
		int root_node_idx = tinyScene.nodes[i];
		DFS(&myModel.m_nodes[root_node_idx], nullptr, m_traversals[i]);
	}
}

void sturdy_guacamole::GLTFScene::DFS(const GLTFNode* node, const traversalStep* parent, Traversal& traversal)
{
	using namespace DirectX::SimpleMath;
	traversalStep step{};
	step.m_node = node;
	step.m_parent = (parent) ? parent->m_node : nullptr;
	step.m_globalTransform = (parent) ? node->m_matrix * parent->m_globalTransform : node->m_matrix;
	traversal.push_back(step);

	for (const auto& child : node->m_children)
	{
		DFS(child, &step, traversal);
	}
}

