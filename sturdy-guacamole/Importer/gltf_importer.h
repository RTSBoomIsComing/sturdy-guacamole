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
		void ProcessScenes(const std::vector<tinygltf::Scene>& tiny_scenes);
		void ProcessNodes(const std::vector<tinygltf::Node>& tiny_nodes);

		template <typename T, size_t count>
		std::array<float, count> ConvertToFloatArray(const std::span<T>& view)
		{
			std::array<float, count> result{};
			for (size_t i = 0; i < count && i < view.size(); ++i) {
				result[i] = static_cast<float>(view[i]);
			}

			return result;
		}
	};
}
