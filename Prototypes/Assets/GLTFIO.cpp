#include "GLTFIO.h"

#define TINYGLTF_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

using namespace tinygltf;

namespace CForge {

	GLTFIO::GLTFIO(void) {

	}//Constructor

	GLTFIO::~GLTFIO(void) {

	}//Destructor

	void GLTFIO::load(const std::string Filepath, T3DMesh<float>* pMesh) {
		TinyGLTF Loader;
		

	}//load

	void GLTFIO::store(const std::string Filepath, const T3DMesh<float>* pMesh) {

	}//store

	void GLTFIO::release(void) {
		delete this;
	}//release

	bool GLTFIO::accepted(const std::string Filepath, I3DMeshIO::Operation Op) {
		return (Filepath.find(".glb") != std::string::npos);
	}//accepted

}//name space