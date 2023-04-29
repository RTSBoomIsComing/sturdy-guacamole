#include "GLTFMesh.h"

#include <tiny_gltf.h>

sturdy_guacamole::GLTFMesh::GLTFMesh(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const GLTFModel& myModel)
	: m_name{ mesh.name }
{
	for (const auto& primitive : mesh.primitives)
	{
		m_meshPrimitives.push_back(GLTFPrimitive{ model, primitive, myModel });
	}
}
