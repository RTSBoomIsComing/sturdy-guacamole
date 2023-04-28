#include "Scene.h"

#include <tiny_gltf.h>
sturdy_guacamole::Scene::Scene(const tinygltf::Scene& scene, const tinygltf::Model& model)
	: m_name{ scene.name }
{
	for (int tinyRootNodeIdx : scene.nodes)
	{
		assert(static_cast<size_t>(tinyRootNodeIdx) < model.nodes.size());
		m_sceneRoots.push_back(SceneRoot{ model.nodes[tinyRootNodeIdx], model });
	}
}