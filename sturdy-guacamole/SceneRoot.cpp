#include "SceneRoot.h"
#include "Mesh.h"

#include <tiny_gltf.h>
sturdy_guacamole::SceneRoot::SceneRoot(const tinygltf::Node& node, const tinygltf::Model& model)
{
	DFS(node, model, nullptr);
}

void sturdy_guacamole::SceneRoot::DFS(const tinygltf::Node& node, const tinygltf::Model& model, std::shared_ptr<SceneNode> parent)
{
	// process node
	auto currSceneNode = make_shared<SceneNode>(node, parent);
	m_dfsList.push_back(std::move(currSceneNode));

	// traverse child nodes
	for (const auto& childNode : node.children)
	{
		assert(static_cast<size_t>(childNode) < model.nodes.size());
		DFS(model.nodes[childNode], model, m_dfsList.back());
	}
}
