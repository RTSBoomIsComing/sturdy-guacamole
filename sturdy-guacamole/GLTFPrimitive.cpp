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
	// set primitive topology
	pDeviceContext->IASetPrimitiveTopology(m_primitiveTopology);
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
	// set primitive topology
	pDeviceContext->IASetPrimitiveTopology(m_primitiveTopology);
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

	for (const auto& [attribute_name, accessor_idx] : attributes)
	{
		const auto& accessor = tinyModel.accessors[accessor_idx];
		const int bufferViewIdx = accessor.bufferView;
		const auto& myBufferView = myModel.m_bufferViews[bufferViewIdx];

		D3D11_INPUT_ELEMENT_DESC inputElementDesc{};
		if (attribute_name.find("POSITION") != std::string::npos)
		{
			inputElementDesc.SemanticName = "POSITION";
			inputElementDesc.SemanticIndex = 0;

			// POSITION, NORMAL, TANGENT are always float3
			inputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (attribute_name.find("NORMAL") != std::string::npos)
		{
			inputElementDesc.SemanticName = "NORMAL";
			inputElementDesc.SemanticIndex = 0;

			// POSITION, NORMAL, TANGENT are always float3
			inputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (attribute_name.find("TEXCOORD") != std::string::npos)
		{
			inputElementDesc.SemanticName = "TEXCOORD";
			inputElementDesc.SemanticIndex = std::stoi(attribute_name.substr(attribute_name.find('_') + 1));

			switch (accessor.componentType)
			{
			case TINYGLTF_COMPONENT_TYPE_FLOAT:
				inputElementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
				break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
				inputElementDesc.Format = DXGI_FORMAT_R8G8_UNORM;
				break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
				inputElementDesc.Format = DXGI_FORMAT_R16G16_UNORM;
				break;
			default:
				assert("wrong texcoord format. See the glTF 2.0 specification");
			}
		}
		else
		{
			std::cout << "does not support attribute: " << attribute_name << std::endl;
			continue;
		}

		ID3D11Buffer* pBuffer = myBufferView.m_buffer.Get();
		UINT inputSlot{};
		for (; inputSlot < m_vertexBuffers.size(); ++inputSlot)
		{
			if (m_vertexBuffers[inputSlot] == pBuffer)
				break;
		}

		if (inputSlot == m_vertexBuffers.size())
		{
			// if not found
			m_vertexBuffers.push_back(pBuffer);
			m_vertexBufferStrides.push_back(accessor.ByteStride(tinyModel.bufferViews[bufferViewIdx])); //myBufferView.m_byteStride
			m_vertexBufferOffsets.push_back(0); // byte offset is already applied in GLTFBufferView
		}

		inputElementDesc.InputSlot = inputSlot;
		inputElementDesc.AlignedByteOffset = (UINT)accessor.byteOffset;
		inputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		inputElementDescs.push_back(inputElementDesc);
	}

	// create input layout
	const auto& vsBlob = Graphics::Get().m_vtxShader.basic_blob;
	HRESULT hr = g_pDevice->CreateInputLayout(inputElementDescs.data(), (UINT)inputElementDescs.size(),
		vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_inputLayout);
	ThrowIfFailed(hr);
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
		res = false;
		break;
	case TINYGLTF_MODE_LINE_STRIP:
		m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
		break;
	default:
		res = false;
	}

	return res;
}
