#include "Scene.h"
#include "Mesh.h"
#include <tiny_gltf.h>
sturdy_guacamole::Scene::Scene(const tinygltf::Scene& scene, const tinygltf::Model& model, const std::vector<std::shared_ptr<Mesh>>& meshes)
	: m_name{ scene.name }
{
	for (const auto& tinyRootNode : scene.nodes)
	{
		assert(static_cast<size_t>(tinyRootNode) < model.nodes.size());
		m_sceneRoots.push_back(SceneRoot{ model.nodes[tinyRootNode], model, meshes });
	}
}