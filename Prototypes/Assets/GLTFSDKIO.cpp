

//#include "GLTFSDKIO.h"
//
//#ifndef __unix__
//
//#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
//
//
//#include <experimental/filesystem>
//#include <GLTFSDK/GLTF.h>
//#include <GLTFSDK/GLTFResourceReader.h>
//#include <GLTFSDK/GLBResourceReader.h>
//#include <GLTFSDK/Deserialize.h>
//
//#include <fstream>
//#include <sstream>
//#include <iostream>
//
//using namespace Microsoft::glTF;
//
//namespace CForge {
//
//
//	class StreamReader : public IStreamReader {
//	public:
//		StreamReader(std::experimental::filesystem::path PathBase) {
//			m_PathBase = PathBase;
//		}
//
//		std::shared_ptr<std::istream> GetInputStream(const std::string& filename)const override {
//			auto streamPath = m_PathBase / std::experimental::filesystem::u8path(filename);
//			std::shared_ptr<std::istream> stream = std::make_shared<std::ifstream>(streamPath, std::ios_base::binary);
//
//			if (!stream || !(*stream)) throw CForgeExcept("Unable to open stream!");
//			return stream;
//		}
//
//		//std::shared_ptr<std::istream> GetInputStream(const std::string& filename) const override
//		//{
//		//	// In order to construct a valid stream:
//		//	// 1. The filename argument will be encoded as UTF-8 so use filesystem::u8path to
//		//	//    correctly construct a path instance.
//		//	// 2. Generate an absolute path by concatenating m_pathBase with the specified filename
//		//	//    path. The filesystem::operator/ uses the platform's preferred directory separator
//		//	//    if appropriate.
//		//	// 3. Always open the file stream in binary mode. The glTF SDK will handle any text
//		//	//    encoding issues for us.
//		//	auto streamPath = m_pathBase / std::experimental::filesystem::u8path(filename);
//		//	auto stream = std::make_shared<std::ifstream>(streamPath, std::ios_base::binary);
//
//		//	// Check if the stream has no errors and is ready for I/O operations
//		//	if (!stream || !(*stream))
//		//	{
//		//		throw std::runtime_error("Unable to create a valid input stream for uri: " + filename);
//		//	}
//
//		//	return stream;
//		//}
//
//	private:
//		std::experimental::filesystem::path m_PathBase;
//	};
//
//
//
//	GLTFSDKIO::GLTFSDKIO(void) /* : I3DMeshIO("GLTFSDKIO")*/ {
//
//	}//Constructor
//
//	GLTFSDKIO::~GLTFSDKIO(void) {
//
//	}//Destructor
//
//	void GLTFSDKIO::load(const std::string Filepath, T3DMesh<float>* pMesh) {
//
//
//		auto Reader = std::make_unique<StreamReader>("");
//
//		std::unique_ptr<GLTFResourceReader> RReader;
//
//		
//		/*auto gltfStream = Reader->GetInputStream(Filepath.c_str());
//		auto gltfResourceReader = std::make_unique<GLTFResourceReader>(std::move(Reader));*/
//
//		auto glbStream = Reader->GetInputStream(Filepath.c_str());
//		auto glbResourceReader = std::make_unique<GLBResourceReader>(std::move(Reader), std::move(glbStream));
//
//		std::string Manifest = glbResourceReader->GetJson();
//
//		RReader = std::move(glbResourceReader);
//
//		Document Doc;
//		try {
//			Doc = Deserialize(Manifest);
//		}
//		catch (const GLTFException& ex) {
//			throw CForgeExcept("Failed to deserialize gltf");
//		}
//
//		printf("Number of meshes: %d\n", Doc.meshes.Size());
//		for (uint32_t i = 0; i < Doc.meshes.Size(); ++i) {
//			auto M = Doc.meshes.Get(i);
//			printf("Mesh %d has %d primitives\n", i, M.primitives.size());
//			printf("Mesh %d has %d weights\n", i, M.weights.size());
//		}
//
//
//	}//load
//
//	void GLTFSDKIO::store(const std::string Filepath, const T3DMesh<float>* pMesh) {
//
//	}//store
//
//	void GLTFSDKIO::release(void) {
//		delete this;
//	}//release
//
//	bool GLTFSDKIO::accepted(const std::string Filepath, I3DMeshIO::Operation Op) {
//		if (Filepath.find(".glb") != std::string::npos) return true;
//		return false;
//	}//accepted
//
//}//name space
//
//#endif