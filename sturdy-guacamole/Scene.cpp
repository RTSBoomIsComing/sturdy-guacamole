#include "Scene.h"
#include "Mesh.h"
#include <tiny_gltf.h>
sturdy_guacamole::Scene::Scene(const tinygltf::Scene& scene, const tinygltf::Model& model, const std::vector<Mesh>& meshes)
	: m_name{ scene.name }
{
	for (int tinyRootNodeIdx : scene.nodes)
	{
		assert(static_cast<size_t>(tinyRootNodeIdx) < model.nodes.size());
		m_sceneRoots.push_back(SceneRoot{ model.nodes[tinyRootNodeIdx], model, meshes });
	}
}