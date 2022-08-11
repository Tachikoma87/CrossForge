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

		readNodes();

		readSkinningData();

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
					mat(1) = 1.0f - pos[1];

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

	void GLTFIO::readIndices(const int accessorIndex, std::vector<int>* pIndices) {
		Accessor acc = model.accessors[accessorIndex];
		
		if (acc.componentType == TINYGLTF_COMPONENT_TYPE_INT) {
			getAccessorDataScalar(accessorIndex, pIndices);
		}
		else if (acc.componentType == TINYGLTF_COMPONENT_TYPE_BYTE) {
			std::vector<char> data;
			getAccessorDataScalar(accessorIndex, &data);
			for (auto i : data) pIndices->push_back((int) i);
		}
		else if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
			std::vector<unsigned char> data;
			getAccessorDataScalar(accessorIndex, &data);
			for (auto i : data) pIndices->push_back((int)i);
		}
		else if (acc.componentType == TINYGLTF_COMPONENT_TYPE_SHORT) {
			std::vector<short> data;
			getAccessorDataScalar(accessorIndex, &data);
			for (auto i : data) pIndices->push_back((int)i);
		}
		else if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
			std::vector<unsigned short> data;
			getAccessorDataScalar(accessorIndex, &data);
			for (auto i : data) pIndices->push_back((int)i);
		}
	}

	void GLTFIO::readFaces(Primitive* pPrimitive, std::vector<T3DMesh<float>::Face>* faces) {
		int accessorIndex = pPrimitive->indices;

		std::vector<int> indices;
		readIndices(accessorIndex, &indices);

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

		// pMaterial->ID = materialIndex;

		pMaterial->Metallic = gltfMaterial.pbrMetallicRoughness.metallicFactor;
		pMaterial->Roughness = gltfMaterial.pbrMetallicRoughness.roughnessFactor;


		pMaterial->Color[0] = gltfMaterial.pbrMetallicRoughness.baseColorFactor[0];
		pMaterial->Color[1] = gltfMaterial.pbrMetallicRoughness.baseColorFactor[1];
		pMaterial->Color[2] = gltfMaterial.pbrMetallicRoughness.baseColorFactor[2];
		pMaterial->Color[3] = gltfMaterial.pbrMetallicRoughness.baseColorFactor[3];


		pMaterial->TexAlbedo = getTexturePath(gltfMaterial.pbrMetallicRoughness.baseColorTexture.index);
		pMaterial->TexNormal = getTexturePath(gltfMaterial.normalTexture.index);
		pMaterial->TexMetallicRoughness = getTexturePath(gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index);
		pMaterial->TexOcclusion = getTexturePath(gltfMaterial.occlusionTexture.index);
		pMaterial->TexEmissive = getTexturePath(gltfMaterial.emissiveTexture.index);
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
		T3DMesh<float>::SkeletalAnimation* pAnim;

		int id_counter = 0;

		for (Animation animation : model.animations) {
			pAnim = new T3DMesh<float>::SkeletalAnimation;

			pAnim->Name = animation.name;

			std::vector<T3DMesh<float>::BoneKeyframes*> keyframes;

			for (AnimationChannel channel : animation.channels) {
				T3DMesh<float>::BoneKeyframes* pBoneKF = nullptr;

				for (auto frame : keyframes) {
					if (frame->BoneID == channel.target_node) {
						pBoneKF = frame;
						break;
					}
				}

				if (pBoneKF == nullptr) {
					pBoneKF = new T3DMesh<float>::BoneKeyframes;
					pBoneKF->BoneID = channel.target_node;
					pBoneKF->ID = id_counter++;
				}

				if (channel.target_path == "translation") {
					std::vector<float> timeValues;
					std::vector<Eigen::Vector3f> translations;

					getAccessorDataScalar(animation.samplers[channel.sampler].input, &timeValues);
					getAccessorData(animation.samplers[channel.sampler].output, &translations);
				}
				else if (channel.target_path == "rotation") {

				}
				else if (channel.target_path == "scale") {

				}
			}

			pMesh->addSkeletalAnimation(pAnim, false);
		}
	}
	
	void GLTFIO::readSkinningData() {
		for (Skin skin : model.skins) {
			std::vector<std::vector<float>> inverseBindMatrices;

			getAccessorData(skin.inverseBindMatrices, &inverseBindMatrices);

			std::vector<Eigen::Matrix4f> offsetMatrices;
			
			for (auto vec : inverseBindMatrices) {
				Eigen::Matrix4f offsetMatrix;
				
				for (int i = 0; i < vec.size(); i++) {
					offsetMatrix(i % 4, i / 4) = vec[i];
				}

				offsetMatrices.push_back(offsetMatrix);
			}

			int counter = 0;

			for (int i : skin.joints) {
				pMesh->getBone(i)->OffsetMatrix = offsetMatrices[counter];
				counter++;
			}
		}
	}

	void GLTFIO::readNodes() {
		std::vector<T3DMesh<float>::Bone*>* pBones = new std::vector<T3DMesh<float>::Bone*>;
		
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

			//Offset matrices will be set later in readSkinningData().

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

		pMesh->bones(pBones, false);
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
		this->pCMesh = pMesh;

		Model emptyModel;
		model = emptyModel;

		coord.clear();
		normal.clear();
		tangent.clear();
		texCoord.clear();
		color.clear();
		joint.clear();
		weight.clear();

		offsets.clear();

		filePath = Filepath;

		writeMeshes();
		
		TinyGLTF writer;

		writer.WriteGltfSceneToFile(&model, Filepath, false, false, true, false);
	}//store

	void GLTFIO::writeMeshes() {
		//Every texture will use this basic sampler.
		Sampler gltfSampler;

		gltfSampler.magFilter = TINYGLTF_TEXTURE_FILTER_LINEAR;
		gltfSampler.minFilter = TINYGLTF_TEXTURE_FILTER_LINEAR;
		gltfSampler.wrapS = TINYGLTF_TEXTURE_WRAP_REPEAT;
		gltfSampler.wrapT = TINYGLTF_TEXTURE_WRAP_REPEAT;

		model.samplers.push_back(gltfSampler);
		
		//Every mesh will hold a single primitive with the submesh data.
		for (int i = 0; i < pCMesh->submeshCount(); i++) {
			writePrimitive();
		}
	}//writeMeshes
	
	void GLTFIO::writePrimitive() {
		Buffer buffer;
		model.buffers.push_back(buffer);
		Mesh mesh;
		model.meshes.push_back(mesh);
		Primitive primitive;
		
		int meshIndex = model.meshes.size() - 1;

		model.meshes[meshIndex].primitives.push_back(primitive);
		
		prepareAttributeArrays();
		writeAttributes();
		writeMaterial();
	}//writeSubmesh
	
	void GLTFIO::prepareAttributeArrays() {
		int meshIndex = model.meshes.size() - 1;
		const T3DMesh<float>::Submesh* pSubmesh = pCMesh->getSubmesh(meshIndex);

		coord.clear();
		normal.clear();
		tangent.clear();
		texCoord.clear();
		color.clear();
		joint.clear();
		weight.clear();

		int32_t min = -1;
		int32_t max = -1;

		std::vector<int32_t> indices;

		for (int i = 0; i < pSubmesh->Faces.size(); i++) {
			auto face = pSubmesh->Faces[i];

			for (int j = 0; j < 3; j++) {
				int32_t index = face.Vertices[j];

				if (min == -1 || index < min) {
					min = index;
				}

				if (max == -1 || index > max) {
					max = index;
				}

				indices.push_back(index);
			}
		}

		for (int i = 0; i < indices.size(); i++) {
			// fill up to index
			for (int k = coord.size(); k <= indices[i] - min; k++) {
				Eigen::Vector3f vec3;
				coord.push_back(vec3);
			}
			auto pos = pCMesh->vertex(indices[i]);
			coord[indices[i] - min] = pos;

			if (pCMesh->normalCount() > 0) {
				// fill up to index
				for (int k = normal.size(); k <= indices[i] - min; k++) {
					Eigen::Vector3f vec3;
					normal.push_back(vec3);
				}
				auto norm = pCMesh->normal(indices[i]);
				normal[indices[i] - min] = norm;
			}

			if (pCMesh->tangentCount() > 0) {
				// fill up to index
				for (int k = tangent.size(); k <= indices[i] - min; k++) {
					Eigen::Vector3f vec3;
					tangent.push_back(vec3);
				}
				auto tan = pCMesh->tangent(indices[i]);
				tangent[indices[i] - min] = tan;
			}

			if (pCMesh->textureCoordinatesCount() > 0) {
				// fill up to index
				for (int k = texCoord.size(); k <= indices[i] - min; k++) {
					Eigen::Vector3f vec3;
					texCoord.push_back(vec3);
				}
				auto tex = pCMesh->textureCoordinate(indices[i]);
				tex(1) = 1.0f - tex(1);
				texCoord[indices[i] - min] = tex;
			}

			if (pCMesh->colorCount() > 0) {
				// fill up to index
				for (int k = color.size(); k <= indices[i] - min; k++) {
					Eigen::Vector4f vec4;
					color.push_back(vec4);
				}
				auto meshCol = pCMesh->color(indices[i]);
				Eigen::Vector4f col;
				col(0) = meshCol(0);
				col(1) = meshCol(1);
				col(2) = meshCol(2);
				col(3) = meshCol(3);
				color[indices[i] - min] = col;
			}

			indices[i] -= min;
		}

		writeAccessorDataScalar(model.buffers.size() - 1, TINYGLTF_COMPONENT_TYPE_INT, &indices);
		model.meshes[meshIndex].primitives[0].indices = model.accessors.size() - 1;

		std::cout << "coord " << coord.size() << std::endl;
		std::cout << "normal " << normal.size() << std::endl;
		std::cout << "tangent " << tangent.size() << std::endl;
		std::cout << "texCoord " << texCoord.size() << std::endl;
		std::cout << "color " << color.size() << std::endl;
		std::cout << "joint " << joint.size() << std::endl;
		std::cout << "weight " << weight.size() << std::endl;

	}

	void GLTFIO::writeAttributes() {
		int meshIndex = model.meshes.size() - 1;
		Primitive* pPrimitive = &(model.meshes[meshIndex].primitives[0]);
		
		int bufferIndex = model.buffers.size() - 1;
		int accessorIndex = model.accessors.size();

		std::vector<std::vector<float>> data;

		pPrimitive->attributes.emplace("POSITION", accessorIndex++);
		for (auto pos : coord) {
			std::vector<float> tmp;
			for (int i = 0; i < 3; i++) {
				tmp.push_back(pos(i));
			}
			data.push_back(tmp);
		}

		writeAccessorData(bufferIndex, TINYGLTF_TYPE_VEC3, &data);
		data.clear();
		
		if (normal.size() > 0) {
			pPrimitive->attributes.emplace("NORMAL", accessorIndex++);
			for (auto n : normal) {
				std::vector<float> tmp;
				for (int i = 0; i < 3; i++) {
					tmp.push_back(n(i));
				}
				data.push_back(tmp);
			}

			writeAccessorData(bufferIndex, TINYGLTF_TYPE_VEC3, &data);
			data.clear();
		}
		
		if (tangent.size() > 0) {
			pPrimitive->attributes.emplace("TANGENT", accessorIndex++);
			for (auto t: tangent) {
				std::vector<float> tmp;
				for (int i = 0; i < 3; i++) {
					tmp.push_back(t(i));
				}
				data.push_back(tmp);
			}

			writeAccessorData(bufferIndex, TINYGLTF_TYPE_VEC3, &data);
			data.clear();
		}
		
		if (texCoord.size() > 0) {
			int count = 0;
			pPrimitive->attributes.emplace("TEXCOORD_0", accessorIndex++);
			for (auto tex : texCoord) {
				std::vector<float> tmp;
				for (int i = 0; i < 2; i++) {
					tmp.push_back(tex(i));
					if (count == 0) std::cout << tex(i) << " ";
				}
				data.push_back(tmp);
				if (count == 0) std::cout << std::endl;
				count++;
			}

			writeAccessorData(bufferIndex, TINYGLTF_TYPE_VEC2, &data);
			data.clear();
		}

		if (color.size() > 0) {
			pPrimitive->attributes.emplace("COLOR_0", accessorIndex++);
			for (auto c : color) {
				std::vector<float> tmp;
				for (int i = 0; i < 4; i++) {
					tmp.push_back(c(i));
				}
				data.push_back(tmp);
			}

			writeAccessorData(bufferIndex, TINYGLTF_TYPE_VEC4, &data);
			data.clear();
		}
	}

	void GLTFIO::writeMaterial() {
		int meshIndex = model.meshes.size() - 1;

		T3DMesh<float>::Submesh* pSubmesh = pMesh->getSubmesh(meshIndex);
		
		T3DMesh<float>::Material* pMaterial = pMesh->getMaterial(pSubmesh->Material);

		Material gltfMaterial;

		gltfMaterial.pbrMetallicRoughness.metallicFactor = pMaterial->Metallic;
		gltfMaterial.pbrMetallicRoughness.roughnessFactor = pMaterial->Roughness;

		gltfMaterial.normalTexture.index = writeTexture(pMaterial->TexNormal);
		gltfMaterial.pbrMetallicRoughness.baseColorTexture.index = writeTexture(pMaterial->TexAlbedo);
		gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index = writeTexture(pMaterial->TexMetallicRoughness);
		gltfMaterial.emissiveTexture.index = writeTexture(pMaterial->TexEmissive);
		gltfMaterial.occlusionTexture.index = writeTexture(pMaterial->TexOcclusion);

		model.meshes[meshIndex].primitives[0].material = model.materials.size();

		model.materials.push_back(gltfMaterial);
	}

	int GLTFIO::writeTexture(const std::string path) {
		std::filesystem::path texPath(path);
		std::filesystem::path gltfFilePath(filePath);

		auto parent = gltfFilePath.parent_path();
		auto possibleTexPath = parent / (texPath.filename());

		if (!std::filesystem::exists(possibleTexPath)) {
			std::filesystem::copy(texPath, possibleTexPath);
		}

		std::string uri = possibleTexPath.filename().string();

		Texture gltfTexture;
		
		gltfTexture.source = model.images.size();
		
		Image gltfImage;
		
		gltfImage.uri = uri;

		model.images.push_back(gltfImage);
		
		gltfTexture.sampler = 0;

		model.textures.push_back(gltfTexture);

		return model.textures.size() - 1;
	}

	void GLTFIO::writeNodes() {
		
	}

	void GLTFIO::release(void) {
		delete this;
	}//release

	bool GLTFIO::accepted(const std::string Filepath, I3DMeshIO::Operation Op) {
		return (Filepath.find(".glb") != std::string::npos || Filepath.find(".gltf") != std::string::npos);
	}//accepted


	void GLTFIO::getAccessorData(const int accessor, std::vector<Eigen::Vector3f>* pData) {
		std::vector<std::vector<float>> vData;

		getAccessorData(accessor, &vData);

		for (std::vector<float> v : vData) {
			Eigen::Vector3f vec;

			vec(0) = v[0];
			vec(1) = v[1];
			vec(2) = v[2];

			pData->push_back(vec);
		}
	}

	void GLTFIO::getAccessorData(const int accessor, std::vector<Eigen::Vector4f>* pData) {
		std::vector<std::vector<float>> vData;

		getAccessorData(accessor, &vData);

		for (auto v : vData) {
			Eigen::Vector4f vec;

			vec(0) = v[0];
			vec(1) = v[1];
			vec(2) = v[2];
			vec(3) = v[3];

			pData->push_back(vec);
		}
	}
}//name space