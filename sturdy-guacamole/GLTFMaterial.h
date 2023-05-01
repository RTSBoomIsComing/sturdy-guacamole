#pragma once
#include <string>

namespace tinygltf
{
	class Model;
	class Material;
}

namespace sturdy_guacamole
{
	class GLTFModel;

	class GLTFMaterial
	{
	public:
		GLTFMaterial(const GLTFModel& myModel);
		~GLTFMaterial() = default;
		std::string m_name{};
	};
}

