#pragma once
#include "SceneRoot.h"

#include <memory>
#include <vector>
#include <string>

namespace tinygltf
{
	class Model;
	struct Scene;
}

namespace sturdy_guacamole
{
	class Mesh;

	class Scene
	{
	public:
		Scene(const tinygltf::Scene& scene, const tinygltf::Model& model, const std::vector<std::shared_ptr<Mesh>>& meshes);
		~Scene() = default;

		std::string m_name{};

		std::vector<SceneRoot> m_sceneRoots{};
	};
}

