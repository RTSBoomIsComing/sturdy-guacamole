#pragma once
#include <string>
#include <array>
#include <vector>
#include <optional>

namespace sturdy_guacamole::rendering
{
	// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-material
	struct Material
	{
		enum TextureType {
			BaseColor, MetallicRoughness, Normal, Occlusion, Emissive, Count
		};

		std::array<std::optional<std::uint8_t>, TextureType::Count> texCoord{};
		std::array<float, 4> baseColor_factor{ 1.0f, 1.0f, 1.0f, 1.0f };	// default: white (1,1,1,1)
		float metallic_factor{ 1.0f };
		float roughness_factor{ 1.0f };
		float normal_scale{ 1.0f };
		float occlusion_strength{ 1.0f };
		std::array<float, 3> emissive_factor{};	// default: black (0,0,0)

		// TODO : The Material class does not support the following properties yet.
		// std::string alphaMode{ "OPAQUE" };
		// float alphaCutoff{ 0.5f };
		// bool doubleSided{ false };
	};
}