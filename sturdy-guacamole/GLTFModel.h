#pragma once
#include "Scene.h"
#include "Mesh.h"
#include "MeshPrimitive.h"
#include <vector>

namespace tinygltf
{
	class Model;
}

namespace sturdy_guacamole
{
	class GLTFModel
	{
	public:
		GLTFModel(const tinygltf::Model& tinyModel);
		~GLTFModel() = default;

		std::vector<Mesh> m_meshes{};
		std::vector<Scene> m_scenes{};
	};
}