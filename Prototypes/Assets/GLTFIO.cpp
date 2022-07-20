#include "GLTFIO.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>
#include <iostream>
#include <filesystem>

//using namespace tinygltf;

namespace CForge {

	GLTFIO::GLTFIO(void) {
		pMesh = nullptr;

		materialIndex = 0;

		filePath = "";
	}//Constructor

	GLTFIO::~GLTFIO(void) {

	}//Destructor

	void GLTFIO::load(const std::string Filepath, T3DMesh<float>* pMesh) {
		this->pMesh = pMesh;
		
		coord.clear();
		normal.clear();
		tangent.clear();
		texCoord.clear();
		color.clear();
		joint.clear();
		weight.clear();

		offsets.clear();

		materialIndex = 0;

		filePath = Filepath;

		TinyGLTF loader;

		std::string err;
		std::string warn;

		bool res = loader.LoadASCIIFromFile(&model, &err, &warn, Filepath);

		for (int i = 0; i < model.accessors.size(); i++) {

			std::string type;

			switch (model.accessors[i].type) {
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

			switch (model.accessors[i].componentType) {
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

			std::cout << "Accessor " << i << " type: " << type << " offset: " << model.accessors[i].byteOffset << std::endl;
		}

		for (int i = 0; i < model.bufferViews.size(); i++) {
			std::cout << "BufferView offset: " << model.bufferViews[i].byteOffset << std::endl;
		}

		readMeshes();

		readSkeletalAnimations();

		std::cout << "END" << std::endl;

	}//load

	void GLTFIO::readMeshes() {
		for (int i = 0; i < model.meshes.size(); i++) {
			Mesh currentMesh = model.meshes[i];

			for (int k = 0; k < model.meshes[i].primitives.size(); k++) {
				Primitive currentPrimitive = currentMesh.primitives[k];

				readPrimitive(& currentPrimitive);
			}
		}
		
		auto pCoord = new std::vector<Eigen::Matrix<float, 3, 1>>;
		for (auto i : coord) pCoord->push_back(i);
		auto pNormal = new std::vector<Eigen::Matrix<float, 3, 1>>;
		for (auto i : normal) pNormal->push_back(i);
		auto pTangent = new std::vector<Eigen::Matrix<float, 3, 1>>;
		for (auto i : tangent) pTangent->push_back(i);
		auto pTexCoord = new std::vector<Eigen::Matrix<float, 3, 1>>;
		for (auto i : texCoord) pTexCoord->push_back(i);

		pMesh->vertices(pCoord);
		pMesh->normals(pNormal);
		pMesh->tangents(pTangent);
		pMesh->textureCoordinates(pTexCoord);
	}

	void GLTFIO::readPrimitive(Primitive* pPrimitive) {
		readAttributes(pPrimitive);

		readSubMeshes(pPrimitive);
	}

	void GLTFIO::readAttributes(Primitive* pPrimitive) {
		int counter = 0;
		
		for (auto keyValuePair : pPrimitive->attributes) {
			if (keyValuePair.first == "POSITION") {
				std::vector<std::vector<float>> positions;
				getAccessorData(keyValuePair.second, &positions);

				for (auto pos : positions) {
					Eigen::Matrix<float, 3, 1> mat;

					mat(0) = pos[0];
					mat(1) = pos[1];
					mat(2) = pos[2];

					coord.push_back(mat);

					counter++;
				}

				continue;
			}

			if (keyValuePair.first == "NORMAL") {
				std::vector<std::vector<float>> normals;
				getAccessorData(keyValuePair.second, &normals);

				for (auto pos : normals) {
					Eigen::Matrix<float, 3, 1> mat;

					mat(0) = pos[0];
					mat(1) = pos[1];
					mat(2) = pos[2];

					normal.push_back(mat);
				}

				continue;
			}

			if (keyValuePair.first == "TANGENT") {
				std::vector<std::vector<float>> tangents;
				getAccessorData(keyValuePair.second, &tangents);

				for (auto pos : tangents) {
					Eigen::Matrix<float, 3, 1> mat;

					mat(0) = pos[0];
					mat(1) = pos[1];
					mat(2) = pos[2];

					tangent.push_back(mat);
				}

				continue;
			}

			if (keyValuePair.first.find("TEXCOORD") != std::string::npos) {
				int n = -1;

				auto s = keyValuePair.first.substr(9);

				sscanf(s.c_str(), "%d", &n);

				//TODO: support multiple textures
				if (n != 0) continue;

				std::vector<std::vector<float>> texcoords;
				getAccessorData(keyValuePair.second, &texcoords);

				for (auto pos : texcoords) {
					Eigen::Matrix<float, 3, 1> mat;

					mat(0) = pos[0];
					mat(1) = pos[1];

					texCoord.push_back(mat);
				}

				continue;
			}

			if (keyValuePair.first.find("JOINTS") != std::string::npos) {
				int n = -1;

				auto s = keyValuePair.first.substr(7);

				sscanf(s.c_str(), "%d", &n);

				//TODO: support multiple joints
				if (n != 0) continue;

				std::vector<std::vector<float>> texcoords;
				getAccessorData(keyValuePair.second, &texcoords);

				for (auto pos : texcoords) {
					Eigen::Matrix<float, 4, 1> mat;

					mat(0) = pos[0];
					mat(1) = pos[1];
					mat(2) = pos[2];
					mat(3) = pos[3];

					joint.push_back(mat);
				}

				continue;
			}

			if (keyValuePair.first.find("WEIGHTS") != std::string::npos) {
				int n = -1;

				auto s = keyValuePair.first.substr(8);

				sscanf(s.c_str(), "%d", &n);

				//TODO: support multiple weights
				if (n != 0) continue;

				std::vector<std::vector<float>> texcoords;
				getAccessorData(keyValuePair.second, &texcoords);

				for (auto pos : texcoords) {
					Eigen::Matrix<float, 4, 1> mat;

					mat(0) = pos[0];
					mat(1) = pos[1];
					mat(2) = pos[2];
					mat(3) = pos[3];

					weight.push_back(mat);
				}

				continue;
			}

			if (keyValuePair.first.find("COLOR") != std::string::npos) {
				int n = -1;

				auto s = keyValuePair.first.substr(8);

				sscanf(s.c_str(), "%d", &n);

				//TODO: support multiple colors
				if (n != 0) continue;

				std::vector<std::vector<float>> texcoords;
				getAccessorData(keyValuePair.second, &texcoords);

				for (auto pos : texcoords) {
					Eigen::Matrix<float, 4, 1> mat;

					mat(0) = pos[0];
					mat(1) = pos[1];
					mat(2) = pos[2];
					mat(3) = pos[3];

					color.push_back(mat);
				}

				continue;
			}
		}//for attributes

		for (int i = normal.size(); i < coord.size(); i++) normal.push_back(*(new Eigen::Matrix<float, 3, 1>));

		offsets.push_back(counter);

		std::cout << "  " << counter << " vertices" << std::endl;
	}

	void GLTFIO::readSubMeshes(Primitive* pPrimitive) {
		T3DMesh<float>::Submesh* pSubMesh = new T3DMesh<float>::Submesh;

		std::vector<T3DMesh<float>::Face> faces;
		readFaces(pPrimitive, &faces);
		pSubMesh->Faces = faces;

		T3DMesh<float>::Material* pMaterial = new T3DMesh<float>::Material;
		readMaterial(pPrimitive->material, pMaterial);
		pMesh->addMaterial(pMaterial, false);
		pSubMesh->Material = materialIndex;
		materialIndex++;

		pMesh->addSubmesh(pSubMesh, false);
	}

	void GLTFIO::readFaces(Primitive* pPrimitive, std::vector<T3DMesh<float>::Face>* faces) {
		int accessorIndex = pPrimitive->indices;

		std::vector<unsigned short> indices;
		getAccessorDataScalar(accessorIndex, &indices);

		unsigned long offset = 0;
		for (int i = 0; i < offsets.size() - 1; i++) offset += i;

		if (pPrimitive->mode == TINYGLTF_MODE_TRIANGLES) {
			for (int i = 0; i < indices.size(); i += 3) {
				T3DMesh<float>::Face face;

				face.Vertices[0] = (int32_t)(offset + indices[i]);
				face.Vertices[1] = (int32_t)(offset + indices[i + 1]);
				face.Vertices[2] = (int32_t)(offset + indices[i + 2]);

				faces->push_back(face);
			}

			return;
		}

		if (pPrimitive->mode == TINYGLTF_MODE_TRIANGLE_STRIP) {
			for (int i = 0; i < indices.size(); i++) {
				T3DMesh<float>::Face face;

				face.Vertices[0] = (int32_t)(offset + indices[i]);
				face.Vertices[1] = (int32_t)(offset + indices[i + (1 + i % 2)]);
				face.Vertices[2] = (int32_t)(offset + indices[i + (2 - i % 2)]);

				faces->push_back(face);
			}

			return;
		}

		if (pPrimitive->mode == TINYGLTF_MODE_TRIANGLE_FAN) {
			for (int i = 0; i < indices.size(); i += 3) {
				T3DMesh<float>::Face face;

				face.Vertices[0] = (int32_t)(offset + indices[i + 1]);
				face.Vertices[1] = (int32_t)(offset + indices[i + 2]);
				face.Vertices[2] = (int32_t)(offset + indices[0]);

				faces->push_back(face);
			}

			return;
		}
	}

	void GLTFIO::readMaterial(const int materialIndex, T3DMesh<float>::Material* pMaterial) {
		Material gltfMaterial = model.materials[materialIndex];

		pMaterial->ID = materialIndex;

		pMaterial->Metallic = gltfMaterial.pbrMetallicRoughness.metallicFactor;
		pMaterial->Roughness = gltfMaterial.pbrMetallicRoughness.roughnessFactor;


		pMaterial->Color[0] = gltfMaterial.pbrMetallicRoughness.baseColorFactor[0];
		pMaterial->Color[1] = gltfMaterial.pbrMetallicRoughness.baseColorFactor[1];
		pMaterial->Color[2] = gltfMaterial.pbrMetallicRoughness.baseColorFactor[2];
		pMaterial->Color[3] = gltfMaterial.pbrMetallicRoughness.baseColorFactor[3];

		

		pMaterial->TexAlbedo = getTexturePath(gltfMaterial.pbrMetallicRoughness.baseColorTexture.index);
		pMaterial->TexNormal = getTexturePath(gltfMaterial.normalTexture.index);
		// pMaterial->TexDepth = getTexturePath(gltfMaterial.occlusionTexture.index);
	}

	std::string GLTFIO::getTexturePath(const int textureIndex) {
		int sourceIndex = model.textures[textureIndex].source;

		//combine texture uri with gltf file parent directory to get the full path

		std::filesystem::path path = filePath;
		path = path.parent_path();
		path.append(model.images[sourceIndex].uri);

		return path.string();
	}

	void GLTFIO::readSkeletalAnimations() {
		std::vector<T3DMesh<float>::Bone*>* pBones = new std::vector<T3DMesh<float>::Bone*>;

		readNodes(pBones);

		pMesh->bones(pBones, false);
	}

	void GLTFIO::readNodes(std::vector<T3DMesh<float>::Bone*>* pBones) {
		for (int i = 0; i < model.nodes.size(); i++) {
			Node node = model.nodes[i];

			T3DMesh<float>::Bone* pBone = new T3DMesh<float>::Bone;
			pBone->ID = i;

			pBone->Name = node.name;

			if (node.translation.size() > 0) {
				pBone->Position(0) = node.translation[0];
				pBone->Position(1) = node.translation[1];
				pBone->Position(2) = node.translation[2];
			}

			pBone->pParent = nullptr;
			
			pBones->push_back(pBone);
		}


		// Do a second pass to link all bones together.

		for (int i = 0; i < model.nodes.size(); i++) {
			std::vector<T3DMesh<float>::Bone*> children;

			auto pBone = pBones->at(i);

			for (auto c : model.nodes[i].children) {
				auto pChild = pBones->at(c);
				pChild->pParent = pBones->at(i);
				children.push_back(pBones->at(c));
			}

			pBone->Children = children;
		}
	}

	int GLTFIO::sizeOfGltfComponentType(const int componentType) {
		switch (componentType) {
		default:
			return 1;
		case TINYGLTF_COMPONENT_TYPE_BYTE:
			return 1;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
			return 1;
		case TINYGLTF_COMPONENT_TYPE_SHORT:
			return 2;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			return 2;
		case TINYGLTF_COMPONENT_TYPE_INT:
			return 4;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
			return 4;
		case TINYGLTF_COMPONENT_TYPE_FLOAT:
			return 4;
		case TINYGLTF_COMPONENT_TYPE_DOUBLE:
			return 8;
		}
	}//sizeOfGltfComponentType

	int GLTFIO::componentCount(const int type) {
		switch (type) {
		default:
			return 1;
		case TINYGLTF_TYPE_SCALAR:
			return 1;
		case TINYGLTF_TYPE_VEC2:
			return 2;
		case TINYGLTF_TYPE_VEC3:
			return 3;
		case TINYGLTF_TYPE_VEC4:
			return 4;
		case TINYGLTF_TYPE_MAT2:
			return 4;
		case TINYGLTF_TYPE_MAT3:
			return 9;
		case TINYGLTF_TYPE_MAT4:
			return 16;
		}
	}

	void GLTFIO::store(const std::string Filepath, const T3DMesh<float>* pMesh) {

	}//store

	void GLTFIO::release(void) {
		delete this;
	}//release

	bool GLTFIO::accepted(const std::string Filepath, I3DMeshIO::Operation Op) {
		return (Filepath.find(".glb") != std::string::npos);
	}//accepted

}//name space