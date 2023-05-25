#pragma once

#include <array>
#include <span>

namespace sturdy_guacamole::utility
{
	template <typename T, size_t count>
	std::array<float, count> ConvertToFloatArray(std::span<T> view)
	{
		std::array<float, count> result{};
		for (size_t i = 0; i < count && i < view.size(); ++i) {
			result[i] = static_cast<float>(view[i]);
		}
		return result;
	}
}