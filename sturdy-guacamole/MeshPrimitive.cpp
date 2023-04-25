#include "MeshPrimitive.h"
#include "Graphics.h"

// include tinygltf library
#include <tiny_gltf.h>

sturdy_guacamole::MeshPrimitive::MeshPrimitive(const tinygltf::Model& model, const tinygltf::Primitive& primitive)
{
	bool res = SetPrimitiveTopology(primitive);
	if (!res)
	{
		throw std::exception("SetPrimitiveTopology() failed.");
	}

	ProcessIndices(model, primitive);
	ProcessAttributes(model, primitive);
}

void sturdy_guacamole::MeshPrimitive::Draw(ID3D11DeviceContext* pDeviceContext) const
{
	// set index buffer
	pDeviceContext->IASetIndexBuffer(m_indexBuffer.Get(), m_indexBufferFormat, m_indexBufferOffset);
	// set vertex buffer
	pDeviceContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &m_vertexBufferStride, &m_vertexBufferOffset);
	// set primitive topology
	pDeviceContext->IASetPrimitiveTopology(m_primitiveTopology);
	// draw
	pDeviceContext->DrawIndexed(m_indexCount, 0, 0);
}

void sturdy_guacamole::MeshPrimitive::DrawInstanced(ID3D11DeviceContext* pDeviceContext, UINT instanceCount)
{
	// set index buffer
	pDeviceContext->IASetIndexBuffer(m_indexBuffer.Get(), m_indexBufferFormat, m_indexBufferOffset);
	// set vertex buffer
	pDeviceContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &m_vertexBufferStride, &m_vertexBufferOffset);
	// set primitive topology
	pDeviceContext->IASetPrimitiveTopology(m_primitiveTopology);
	// draw
	pDeviceContext->DrawIndexedInstanced(m_indexCount, instanceCount, 0, 0, 0);
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

bool sturdy_guacamole::MeshPrimitive::SetPrimitiveTopology(const tinygltf::Primitive& primitive)
{
	bool res = true;
	switch (primitive.mode)
	{
	case TINYGLTF_MODE_TRIANGLES:
		m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		break;
	case TINYGLTF_MODE_TRIANGLE_STRIP:
		m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		break;
	case TINYGLTF_MODE_TRIANGLE_FAN:
		OutputDebugStringW(L"TINYGLTF_MODE_TRIANGLE_FAN is not supported");
		res = false;
		break;
	case TINYGLTF_MODE_POINTS:
		m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		break;
	case TINYGLTF_MODE_LINE:
		m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		break;
	case TINYGLTF_MODE_LINE_LOOP:
		OutputDebugStringW(L"TINYGLTF_MODE_LINE_LOOP is not supported");
		break;
	case TINYGLTF_MODE_LINE_STRIP:
		m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
		break;
	default:
		res = false;
	}

	return res;
}
