#pragma once
#include <tiny_gltf.h>
#include <filesystem>
#include <vector>
#include <array>
#include <span>

namespace sturdy_guacamole::importer
{
	class GLTFImporter : public tinygltf::Model
	{
	public:
		GLTFImporter(const std::filesystem::path& file_path);
		~GLTFImporter() = default;

	private:
		bool Load(const std::filesystem::path& file_path, tinygltf::Model* tiny_model);
	};
}
