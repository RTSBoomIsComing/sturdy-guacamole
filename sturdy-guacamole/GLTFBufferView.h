#pragma once

#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include <vector>
#include <string>

namespace tinygltf
{
	class Model;
	struct BufferView;
}

namespace sturdy_guacamole
{
	class GLTFModel;
	class GLTFBufferView
	{
	public:
		GLTFBufferView(const tinygltf::Model& model, const tinygltf::BufferView& bufferView, const GLTFModel& myModel);
		~GLTFBufferView() = default;

		std::string m_name{};
		UINT m_byteLength{};
		UINT m_byteStride{};
		ComPtr<ID3D11Buffer> m_buffer{};
		
		// it would be always 0. once creating buffer, the offset is already applied
		// UINT m_byteOffset{}; 
	};
}


