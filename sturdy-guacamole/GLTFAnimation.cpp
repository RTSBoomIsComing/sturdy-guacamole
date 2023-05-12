#include "GLTFAnimation.h"
#include "GLTFModel.h"

#include <tiny_gltf.h>

sturdy_guacamole::GLTFAnimation::GLTFAnimation(const tinygltf::Model& tinyModel, const tinygltf::Animation& tinyAnim, const sturdy_guacamole::GLTFModel& myModel)
	:
	m_name{ tinyAnim.name }
{
	for (const auto& channel : tinyAnim.channels)
	{
		const auto& sampler = tinyAnim.samplers[channel.sampler];

		// process sampler input
		{
			const auto& input = tinyModel.accessors[sampler.input];
			const auto& bufferView = tinyModel.bufferViews[input.bufferView];
			const auto& buffer = tinyModel.buffers[bufferView.buffer];
			const auto& byteOffset = input.byteOffset + bufferView.byteOffset;

			int byteStride = input.ByteStride(bufferView);
			assert(byteStride > 0);

			for (size_t i = 0; i < input.count; ++i)
			{
				// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#skins
				// 5.8.1. animation.sampler.input
				// The accessor MUST be of scalar type with floating-point components.
				float time = *reinterpret_cast<const float*>(&buffer.data[byteOffset + i * byteStride]);
				//m_times.push_back(time);
			}
		}

		// process sampler output

		channel.target_node;
		channel.target_path;

		const auto& output = tinyModel.accessors[sampler.output];
		const auto& bufferView = tinyModel.bufferViews[output.bufferView];
		const auto& buffer = tinyModel.buffers[bufferView.buffer];
		const auto& byteOffset = output.byteOffset + bufferView.byteOffset;
		const auto& count = output.count;

		int byteStride = output.ByteStride(bufferView);
		assert(byteStride > 0);

		const auto& componentType = output.componentType;
		const auto& type = output.type;

		// translation is always VEC3, float
		if (channel.target_path.compare("translation") == 0)
		{

			assert(type == TINYGLTF_TYPE_VEC3);
			assert(componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
			for (size_t i = 0; i < count; ++i)
			{
				float x = *reinterpret_cast<const float*>(&buffer.data[byteOffset + i * byteStride]);
				float y = *reinterpret_cast<const float*>(&buffer.data[byteOffset + i * byteStride + sizeof(float)]);
				float z = *reinterpret_cast<const float*>(&buffer.data[byteOffset + i * byteStride + sizeof(float) * 2]);
				//m_translations.emplace_back(x, y, z);
			}
		}

		if (channel.target_path.compare("rotation") == 0)
		{

		}

		if (channel.target_path.compare("scale") == 0)
		{

		}

		if (channel.target_path.compare("weights") == 0)
		{

		}
	}
}
