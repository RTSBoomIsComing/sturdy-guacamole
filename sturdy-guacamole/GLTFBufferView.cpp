#include "GLTFBufferView.h"
#include "GLTFModel.h"

#include "Dx11Application.h"
#include "Dx11Helpers.h"

#include <tiny_gltf.h>


sturdy_guacamole::GLTFBufferView::GLTFBufferView(const tinygltf::Model& model, const tinygltf::BufferView& bufferView, const GLTFModel& myModel)
	: m_name{ bufferView.name },
	m_byteLength{ static_cast<UINT>(bufferView.byteLength) },
	m_byteStride{ static_cast<UINT>(bufferView.byteStride) }
{
	// DirectX11 has no concept of a buffer view, 
	// so we just create a buffer and use the offset and length to create a view
	D3D11_BIND_FLAG bindFlag{};
	switch (bufferView.target)
	{
	case TINYGLTF_TARGET_ARRAY_BUFFER:
		bindFlag = D3D11_BIND_VERTEX_BUFFER;
		break;
	case TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER:
		bindFlag = D3D11_BIND_INDEX_BUFFER;
		break;
	default:
		// this buffer view is not a vertex or index buffer
		return;
	}

	CD3D11_BUFFER_DESC bufferDesc{ (UINT)bufferView.byteLength, (UINT)bindFlag };

	D3D11_SUBRESOURCE_DATA subresourceData{};
	//subresourceData.pSysMem = model.buffers[bufferView.buffer].data.data() + bufferView.byteOffset;
	subresourceData.pSysMem = &model.buffers[bufferView.buffer].data[bufferView.byteOffset];

	HRESULT hr = g_pDevice->CreateBuffer(&bufferDesc, &subresourceData, &m_buffer);
	ThrowIfFailed(hr);
}
