#include "MeshPrimitive.h"
#include "Graphics.h"

// include tinygltf library
#include <tiny_gltf.h>

sturdy_guacamole::MeshPrimitive::MeshPrimitive(const tinygltf::Model& model, const tinygltf::Primitive& primitive)
{
	ProcessIndices(model, primitive);
	ProcessAttributes(model, primitive);

}

void sturdy_guacamole::MeshPrimitive::ProcessIndices(const tinygltf::Model& model, const tinygltf::Primitive& primitive)
{
	const auto& accessor = model.accessors[primitive.indices];
	const auto& bufferView = model.bufferViews[accessor.bufferView];
	const auto& buffer = model.buffers[bufferView.buffer];

	m_indexBuffer = sturdy_guacamole::Graphics::CreateIndexBuffer(
		buffer.data.data() + bufferView.byteOffset, (UINT)bufferView.byteLength);

	m_indexBufferFormat = (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) ?
		DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

	m_indexBufferOffset = 0;

	m_indexCount = (UINT)accessor.count;
}

void sturdy_guacamole::MeshPrimitive::ProcessAttributes(const tinygltf::Model& model, const tinygltf::Primitive& primitive)
{
	// suggest that attributes is interleaved in a buffer
	const auto& attributes = primitive.attributes;

	// anyway POSITION attribute must be exist
	if (attributes.find("POSITION") != attributes.end())
	{
		const auto& accessor = model.accessors[attributes.at("POSITION")];
		const auto& bufferView = model.bufferViews[accessor.bufferView];
		const auto& buffer = model.buffers[bufferView.buffer];
		
		// create vertex buffer
		m_vertexBuffer = sturdy_guacamole::Graphics::CreateVertexBuffer(
			buffer.data.data() + bufferView.byteOffset, (UINT)bufferView.byteLength);

		m_vertexBufferStride = (UINT)accessor.ByteStride(bufferView);
		m_vertexBufferOffset = 0;

		// cteate input element description
		D3D11_INPUT_ELEMENT_DESC inputElementDesc{};
		inputElementDesc.SemanticName = "POSITION";
		inputElementDesc.SemanticIndex = 0; //e.g. POSITION0, TEXCOORD0, TEXCOORD1, etc.

		// POSITION, NORMAL, TANGENT are always float3
		inputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;

		// if there are multiple bufferViews for vertex attributes, this value should be different
		inputElementDesc.InputSlot = 0;

		inputElementDesc.AlignedByteOffset = (UINT)accessor.byteOffset;
		inputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputElementDesc.InstanceDataStepRate = 0;

		m_inputElementDescs.push_back(inputElementDesc);
	}
}
