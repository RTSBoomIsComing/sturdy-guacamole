#include "GLTFModel.h"
#include "Scene.h"
#include "Mesh.h"
#include "MeshPrimitive.h"


#include <tiny_gltf.h>


sturdy_guacamole::GLTFModel::GLTFModel(const tinygltf::Model& tinyModel)
{
	for (const auto& tinyMesh : tinyModel.meshes)
	{
		// have some problem
		m_meshes.push_back(Mesh{ tinyModel, tinyMesh });
	}

	for (const auto& scene : tinyModel.scenes)
	{
		m_scenes.push_back(Scene{ scene, tinyModel, m_meshes });
	}
}
