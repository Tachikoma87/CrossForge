#include "GLTFIO.hpp"

#define TINYGLTF_IMPLEMENTATION
#ifndef __EMSCRIPTEN__
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif

#if (defined(__EMSCRIPTEN__) || defined(_MSC_VER) || (defined(__GNUC__) && (__GNUC__ > 7)) )
#include <filesystem>
#define STD_FS std::filesystem
#else
#define EXPERIMENTAL_FILESYSTEM 1
#include <experimental/filesystem>
#define STD_FS std::experimental::filesystem
#endif

#include <tiny_gltf.h>

#include <iostream>
#include <algorithm>

using namespace tinygltf;

namespace CForge {

	GLTFIO::GLTFIO(void) {
		m_pMesh = nullptr;

		m_materialIndex = 0;

		m_filePath = "";
	}//Constructor

	GLTFIO::~GLTFIO(void) {

	}//Destructor


	void GLTFIO::load(const std::string Filepath, T3DMesh<float>* pMesh) {
		this->m_pMesh = pMesh;

		m_coord.clear();
		m_normal.clear();
		m_tangent.clear();
		m_texCoord.clear();
		m_color.clear();
		m_joint.clear();
		m_weight.clear();
		m_primitiveIndexRanges.clear();

		m_offsets.clear();

		m_materialIndex = 0;

		m_filePath = Filepath;

		TinyGLTF loader;

		std::string err;
		std::string warn;

		bool res = loader.LoadASCIIFromFile(&m_model, &err, &warn, Filepath);
		//TODO look for glb in filepath and load binary instead.

		if (warn.size()) std::cout << "tinygltf warning: " << warn << std::endl;
		if (err.size()) std::cout << "tinygltf error: " << err << std::endl;

		for (int i = 0; i < m_model.accessors.size(); i++) {

			std::string type;

			switch (m_model.accessors[i].type) {
			case TINYGLTF_TYPE_VEC2:
				type = "Vec2";
				break;
			case TINYGLTF_TYPE_VEC3:
				type = "Vec3";
				break;
			case TINYGLTF_TYPE_VEC4:
				type = "Vec4";
				break;
			case TINYGLTF_TYPE_MAT2:
				type = "Mat2";
				break;
			case TINYGLTF_TYPE_MAT3:
				type = "Mat3";
				break;
			case TINYGLTF_TYPE_MAT4:
				type = "Mat4";
				break;
			case TINYGLTF_TYPE_SCALAR:
				type = "Scalar";
				break;
			default:
				type = "Unknown";
				break;
			}

			switch (m_model.accessors[i].componentType) {
			default:
				type += " Unknown";
				break;
			case TINYGLTF_COMPONENT_TYPE_BYTE:
				type += " byte";
				break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
				type += " unsigned byte";
				break;
			case TINYGLTF_COMPONENT_TYPE_SHORT:
				type += " short";
				break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
				type += " unsigned short";
				break;
			case TINYGLTF_COMPONENT_TYPE_INT:
				type += " int";
				break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
				type += " unsigned int";
				break;
			case TINYGLTF_COMPONENT_TYPE_FLOAT:
				type += " float";
				break;
			case TINYGLTF_COMPONENT_TYPE_DOUBLE:
				type += " double";
				break;
			}

			std::cout << "Accessor " << i << " type: " << type << " offset: " << m_model.accessors[i].byteOffset << std::endl;
		}

		for (int i = 0; i < m_model.bufferViews.size(); i++) {
			std::cout << "BufferView offset: " << m_model.bufferViews[i].byteOffset << std::endl;
		}

		readNodes();

		readSkinningData();

		readMeshes();

		readSkeletalAnimations();

		readMorphTargets();

		std::cout << "END" << std::endl;

	}//load


	void GLTFIO::store(const std::string Filepath, const T3DMesh<float>* pMesh) {
		this->m_pCMesh = pMesh;

		Model emptyModel;
		m_model = emptyModel;

		m_coord.clear();
		m_normal.clear();
		m_tangent.clear();
		m_texCoord.clear();
		m_color.clear();
		m_joint.clear();
		m_weight.clear();

		m_offsets.clear();

		m_primitiveIndexRanges.clear();

		m_filePath = Filepath;

		//Using two buffers. One for vertex data and one for Textures.
		Buffer buffer;
		m_model.buffers.push_back(buffer);
		m_model.buffers.push_back(buffer);

		//Every texture will use this basic sampler.
		Sampler gltfSampler;

		m_model.samplers.push_back(gltfSampler);

		m_model.asset.version = "2.0";

		//Every mesh will hold a single primitive with the submesh data.

		writeNodes();
		writeSkinningData();
		writeSkeletalAnimations();

		TinyGLTF writer;

		writer.WriteGltfSceneToFile(&m_model, Filepath, false, false, true, false);
	}//store


	void GLTFIO::release(void) {
		delete this;
	}//release

	bool GLTFIO::accepted(const std::string Filepath, I3DMeshIO::Operation Op) {
		return (Filepath.find(".glb") != std::string::npos || Filepath.find(".gltf") != std::string::npos);
	}//accepted

}//CForge

//TODO:
/*
* Was passiert mit Skelettanimationen mit unterschiedlichen Keyframes? -> ggf. Umrechnen auf gleiche Keyframes
* Die Berechnung der per face normals schlägt fehl, weil zu große Indices verwendet werden.
* Skelettanimationen speichern.
* Standardanimation der Morphtargets für den Export generieren.
*/
