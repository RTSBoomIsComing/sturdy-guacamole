#pragma once
#include "../Rendering/Model.h"

#include <tiny_gltf.h>
#include <filesystem>
#include <vector>
#include <array>
#include <span>

namespace sturdy_guacamole::import
{
	class GLTFImporter
	{
	public:
		GLTFImporter() = default;
		~GLTFImporter() = default;
		bool Import(const std::filesystem::path& file_name, rendering::Model& model);

	private:
		rendering::Model* pModel{};

		bool Load(const std::filesystem::path& file_name, tinygltf::Model& tiny_model);
		void ProcessScenes(const std::vector<tinygltf::Scene>& tiny_scenes);

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
