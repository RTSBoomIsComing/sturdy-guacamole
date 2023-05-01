#include "GLTFPrimitive.h"
#include "GLTFModel.h"

#include "Dx11Application.h"
#include "Dx11Helpers.h"
#include "Graphics.h"

#include <tiny_gltf.h>

#include <iostream>

sturdy_guacamole::GLTFPrimitive::GLTFPrimitive(const tinygltf::Model& model, const tinygltf::Primitive& primitive, const GLTFModel& myModel)
{
	bool res = SetPrimitiveTopology(primitive);
	if (!res)
		throw std::exception("SetPrimitiveTopology() failed.");


	ProcessIndices(model, primitive, myModel);
	ProcessAttributes(model, primitive, myModel);
}

void sturdy_guacamole::GLTFPrimitive::Draw(ID3D11DeviceContext* pDeviceContext) const
{
	// set input layout
	pDeviceContext->IASetInputLayout(m_inputLayout.Get());
	// set index buffer
	pDeviceContext->IASetIndexBuffer(m_indexBuffer, m_indexBufferFormat, m_indexBufferOffset);
	// set vertex buffer
	pDeviceContext->IASetVertexBuffers(0, (UINT)m_vertexBuffers.size(), m_vertexBuffers.data(), m_vertexBufferStrides.data(), m_vertexBufferOffsets.data());
	// draw
	pDeviceContext->DrawIndexed(m_indexCount, 0, 0);
}

void sturdy_guacamole::GLTFPrimitive::DrawInstanced(ID3D11DeviceContext* pDeviceContext, UINT instanceCount)
{
	// set input layout
	pDeviceContext->IASetInputLayout(m_inputLayout.Get());
	// set index buffer
	pDeviceContext->IASetIndexBuffer(m_indexBuffer, m_indexBufferFormat, m_indexBufferOffset);
	// set vertex buffer
	pDeviceContext->IASetVertexBuffers(0, (UINT)m_vertexBuffers.size(), m_vertexBuffers.data(), m_vertexBufferStrides.data(), m_vertexBufferOffsets.data());
	// draw
	pDeviceContext->DrawIndexedInstanced(m_indexCount, instanceCount, 0, 0, 0);
}

void sturdy_guacamole::GLTFPrimitive::ProcessIndices(const tinygltf::Model& tinyModel, const tinygltf::Primitive& primitive, const GLTFModel& myModel)
{
	const auto& accessor = tinyModel.accessors[primitive.indices];
	const int bufferViewIdx = accessor.bufferView;

	m_indexBuffer = myModel.m_bufferViews[bufferViewIdx].m_buffer.Get();
	m_indexBufferFormat = (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) ?
		DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

	m_indexBufferOffset = 0;

	m_indexCount = (UINT)accessor.count;
}

void sturdy_guacamole::GLTFPrimitive::ProcessAttributes(const tinygltf::Model& tinyModel, const tinygltf::Primitive& primitive, const GLTFModel& myModel)
{
	// used in ID3D11Device::CreateInputLayout();
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescs{};

	const auto& attributes = primitive.attributes;

	{
		const int accessor_idx = attributes.at("POSITION");
		const auto& accessor = tinyModel.accessors[accessor_idx];
		const auto& myBufView = myModel.m_bufferViews[accessor.bufferView];

		const UINT inputSlot = this->FindVertexBuffer(myBufView.m_buffer.Get());
		if (inputSlot == m_vertexBuffers.size())
			this->AddVertexBuffer(myBufView, 12u);
				
		D3D11_INPUT_ELEMENT_DESC ieDesc{};
		ieDesc.SemanticName = "POSITION";
		ieDesc.SemanticIndex = 0;
		ieDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT; // POSITION, NORMAL, TANGENT are always float3
		ieDesc.InputSlot = inputSlot;
		ieDesc.AlignedByteOffset = (UINT)accessor.byteOffset;
		ieDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		inputElementDescs.push_back(ieDesc);
	}

	if (attributes.contains("NORMAL"))
	{
		const int accessor_idx = attributes.at("NORMAL");
		const auto& accessor = tinyModel.accessors[accessor_idx];
		const auto& myBufView = myModel.m_bufferViews[accessor.bufferView];

		const UINT inputSlot = this->FindVertexBuffer(myBufView.m_buffer.Get());
		if (inputSlot == m_vertexBuffers.size())
			this->AddVertexBuffer(myBufView, 12u);

		D3D11_INPUT_ELEMENT_DESC ieDesc{};
		ieDesc.SemanticName = "NORMAL";
		ieDesc.SemanticIndex = 0;
		ieDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT; // POSITION, NORMAL, TANGENT are always float3
		ieDesc.InputSlot = inputSlot;
		ieDesc.AlignedByteOffset = (UINT)accessor.byteOffset;
		ieDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		inputElementDescs.push_back(ieDesc);
	}
	else
	{
		const UINT inputSlot = this->FindVertexBuffer(nullptr);
		if (inputSlot == m_vertexBuffers.size())
			this->AddDummyVertexBuffer();

		D3D11_INPUT_ELEMENT_DESC ieDesc{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, inputSlot };
		inputElementDescs.push_back(ieDesc);
	}

	if (attributes.contains("TEXCOORD_0"))
	{
		const int accessor_idx = attributes.at("TEXCOORD_0");
		const auto& accessor = tinyModel.accessors[accessor_idx];
		const auto& myBufView = myModel.m_bufferViews[accessor.bufferView];

		UINT byteLength{};
		DXGI_FORMAT format{};
		switch (accessor.componentType)
		{
		case TINYGLTF_COMPONENT_TYPE_FLOAT:
			format = DXGI_FORMAT_R32G32_FLOAT;
			byteLength = 8u;
			break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
			format = DXGI_FORMAT_R8G8_UNORM;
			byteLength = 2u;
			break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			format = DXGI_FORMAT_R16G16_UNORM;
			byteLength = 4u;
			break;
		default:
			assert("wrong texcoord format. See the glTF 2.0 specification");
		}

		const UINT inputSlot = this->FindVertexBuffer(myBufView.m_buffer.Get());
		if (inputSlot == m_vertexBuffers.size())
			this->AddVertexBuffer(myBufView, byteLength);

		D3D11_INPUT_ELEMENT_DESC ieDesc{};
		ieDesc.SemanticName = "TEXCOORD";
		ieDesc.SemanticIndex = 0;
		ieDesc.Format = format;
		ieDesc.InputSlot = inputSlot;
		ieDesc.AlignedByteOffset = (UINT)accessor.byteOffset;
		ieDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputElementDescs.push_back(ieDesc);
	}
	else
	{
		const UINT inputSlot = this->FindVertexBuffer(nullptr);
		if (inputSlot == m_vertexBuffers.size())
			this->AddDummyVertexBuffer();

		D3D11_INPUT_ELEMENT_DESC ieDesc{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, inputSlot };
		inputElementDescs.push_back(ieDesc);
	}

	// create input layout
	const auto& vsBlob = Graphics::Get().m_vtxShader.basic_blob;
	ThrowIfFailed(g_pDevice->CreateInputLayout(inputElementDescs.data(), (UINT)inputElementDescs.size(),
		vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_inputLayout));
}

bool sturdy_guacamole::GLTFPrimitive::SetPrimitiveTopology(const tinygltf::Primitive& primitive)
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
	case TINYGLTF_MODE_POINTS:
		m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		break;
	case TINYGLTF_MODE_LINE:
		m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		break;
	case TINYGLTF_MODE_LINE_STRIP:
		m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
		break;
	case TINYGLTF_MODE_TRIANGLE_FAN:
		OutputDebugStringW(L"TINYGLTF_MODE_TRIANGLE_FAN is not supported");
	case TINYGLTF_MODE_LINE_LOOP:
		OutputDebugStringW(L"TINYGLTF_MODE_LINE_LOOP is not supported");
	default:
		res = false;
	}

	return res;
}

void sturdy_guacamole::GLTFPrimitive::AddVertexBuffer(const GLTFBufferView& myBufView, UINT byteLength)
{
	ID3D11Buffer* pBuffer = myBufView.m_buffer.Get();

	m_vertexBuffers.push_back(pBuffer);
	m_vertexBufferOffsets.push_back(0); // byte offset is already applied in GLTFBufferView

	UINT byteStride = (myBufView.m_byteStride > 0) ? myBufView.m_byteStride : byteLength;
	m_vertexBufferStrides.push_back(byteStride);
}

void sturdy_guacamole::GLTFPrimitive::AddDummyVertexBuffer()
{
	m_vertexBuffers.push_back(nullptr);
	m_vertexBufferOffsets.push_back(0);
	m_vertexBufferStrides.push_back(0);
}

UINT sturdy_guacamole::GLTFPrimitive::FindVertexBuffer(const ID3D11Buffer* pVertexBuffer) const
{
	UINT inputSlot{};
	for (; inputSlot < m_vertexBuffers.size(); ++inputSlot)
	{
		if (m_vertexBuffers[inputSlot] == pVertexBuffer)
			break;
	}

	return inputSlot;
}
