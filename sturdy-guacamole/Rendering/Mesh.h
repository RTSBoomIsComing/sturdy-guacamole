#pragma once
#include <string>
#include <vector>
#include <optional>

namespace sturdy_guacamole::rendering
{
	struct Mesh
	{
		struct Primitive
		{
			struct Attributes
			{
				std::optional<unsigned int> position	{};
				std::optional<unsigned int> normal		{};
				std::optional<unsigned int> tangent		{};
				std::optional<unsigned int> texcoord_0	{};
				std::optional<unsigned int> color_0		{};
				std::optional<unsigned int> joints_0	{};
				std::optional<unsigned int> weights_0	{};
			} attributes;

			std::optional<unsigned int> indices		{};
			std::optional<unsigned int> material	{};
			unsigned int mode						{ 4 }; // default: 4 (TRIANGLES)
			std::vector<Attributes> targets			{};
		};
		std::vector<Primitive> primitives{};
		std::vector<float> weights{};
	};
}