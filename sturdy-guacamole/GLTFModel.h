#pragma once
#include "Scene.h"
#include "Mesh.h"

#include <vector>
#include <filesystem>

namespace tinygltf
{
	class Model;
}

namespace sturdy_guacamole
{
	class GLTFModel
	{
	public:
		GLTFModel(const std::filesystem::path& path);
		~GLTFModel() = default;

		std::vector<Mesh> m_meshes{};
		std::vector<Scene> m_scenes{};

	private:
		bool LoadModel(tinygltf::Model& outModel, const std::filesystem::path& path);
	};
}