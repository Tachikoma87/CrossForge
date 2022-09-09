#include "GLTFIO.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <tiny_gltf.h>
#include <iostream>
#include <filesystem>
#include <algorithm>

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

		readNodes();
		
		readMeshes();

		readSkinningData();

		readSkeletalAnimations();

		readMorphTargets();
		
		std::cout << "END" << std::endl;

	}//load

#pragma region read
	
	void GLTFIO::readMeshes() {
		for (int i = 0; i < model.meshes.size(); i++) {
			Mesh currentMesh = model.meshes[i];

			T3DMesh<float>::Submesh* pParentSubmesh;

			for (int j = 0; j < model.nodes.size(); j++) {
				if (model.nodes[j].mesh == i) {
					pParentSubmesh = pMesh->getSubmesh(j);
					break;
				}
			}

			for (int k = 0; k < currentMesh.primitives.size(); k++) {
				Primitive currentPrimitive = currentMesh.primitives[k];

				auto pSubmesh = readPrimitive(&currentPrimitive);

				pParentSubmesh->Children.push_back(pSubmesh);
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
		pMesh->textureCoordinates(pTexCoord);
	}

	T3DMesh<float>::Submesh* GLTFIO::readPrimitive(Primitive* pPrimitive) {
		readAttributes(pPrimitive);

		return readSubMeshes(pPrimitive);
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

				std::vector<Eigen::Vector4f> accJoints;
				getAccessorData(keyValuePair.second, &accJoints);

				for (auto j : accJoints) {
					joint.push_back(j);
				}

				continue;
			}

			if (keyValuePair.first.find("WEIGHTS") != std::string::npos) {
				int n = -1;

				auto s = keyValuePair.first.substr(8);

				sscanf(s.c_str(), "%d", &n);

				//TODO: support multiple weights
				if (n != 0) continue;

				std::vector<Eigen::Vector4f> accWeights;
				getAccessorData(keyValuePair.second, &accWeights);

				for (auto w : accWeights) {
					weight.push_back(w);
				}

				continue;
			}

			if (keyValuePair.first.find("COLOR") != std::string::npos) {
				int n = -1;

				auto s = keyValuePair.first.substr(8);

				sscanf(s.c_str(), "%d", &n);

				//TODO: support multiple colors
				if (n != 0) continue;

				std::vector<Eigen::Vector4f> accColors;
				getAccessorData(keyValuePair.second, &accColors);

				for (auto c : accColors) {
					color.push_back(c);
				}

				continue;
			}
		}//for attributes

		//fill up the rest of the attributes with default values
		for (int i = normal.size(); i < coord.size(); i++) normal.push_back(*(new Eigen::Vector3f));
		for (int i = tangent.size(); i < coord.size(); i++) tangent.push_back(*(new Eigen::Vector3f));
		for (int i = texCoord.size(); i < coord.size(); i++) texCoord.push_back(*(new Eigen::Vector3f));
		for (int i = joint.size(); i < coord.size(); i++) joint.push_back(*(new Eigen::Vector4f));
		for (int i = weight.size(); i < coord.size(); i++) weight.push_back(*(new Eigen::Vector4f));
		for (int i = color.size(); i < coord.size(); i++) color.push_back(*(new Eigen::Vector4f));
		
		//set vertex influences and weights for bones
		for (int i = 0; i < joint.size(); i++) {
			for (int j = 0; j < 4; j++) {
				int32_t index = (int32_t)joint[i](j);
				if (index < 0) continue;
				auto pBone = pMesh->getBone(index);
				pBone->VertexInfluences.push_back(i);
				pBone->VertexWeights.push_back(weight[i](j));
			}
		}

		offsets.push_back(counter);

		std::cout << "  " << counter << " vertices" << std::endl;
	}

	T3DMesh<float>::Submesh* GLTFIO::readSubMeshes(Primitive* pPrimitive) {
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

		return pSubMesh;
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
			std::vector<T3DMesh<float>::BoneKeyframes*> keyframes;
			std::vector<int> inputAccessors;

			float duration = -1;

			bool pure_morph_target_animation = true;

			for (AnimationChannel channel : animation.channels) {
				T3DMesh<float>::BoneKeyframes* pBoneKF = nullptr;
				int input = animation.samplers[channel.sampler].input;

				//A new BoneKeyFrames object is needed for every BoneID and Timestamps vector.

				for (int i = 0; i < keyframes.size(); i++) {
					if (keyframes[i]->BoneID == channel.target_node && inputAccessors[i] == input) {
						pBoneKF = keyframes[i];
						break;
					}
				}

				if (pBoneKF == nullptr) {
					pBoneKF = new T3DMesh<float>::BoneKeyframes;
					pBoneKF->BoneID = channel.target_node;
					pBoneKF->ID = id_counter++;
					keyframes.push_back(pBoneKF);
					inputAccessors.push_back(input);
				}

				std::vector<float> timeValues;
				getAccessorDataScalarFloat(input, &timeValues);
				pBoneKF->Timestamps = timeValues;

				for (auto t : timeValues) duration = std::max(duration, t);

				if (channel.target_path == "translation") {
					std::vector<Eigen::Vector3f> translations;
					int output = animation.samplers[channel.sampler].output;

					getAccessorData(output, &translations);
					
					pBoneKF->Positions = translations;

					pure_morph_target_animation = false;
				}
				else if (channel.target_path == "rotation") {
					std::vector<Eigen::Quaternionf> rotations;
					int output = animation.samplers[channel.sampler].output;

					getAccessorData(output, &rotations);

					pBoneKF->Rotations = rotations;

					pure_morph_target_animation = false;
				}
				else if (channel.target_path == "scale") {
					std::vector<Eigen::Vector3f> scalings;
					int output = animation.samplers[channel.sampler].output;

					getAccessorData(output, &scalings);

					pBoneKF->Scalings = scalings;

					pure_morph_target_animation = false;
				}
				/*
				"weights" as animation channel target is ignored,
				because cross forge only stores the morph targets without animated weights.
				*/
			}
			
			if (pure_morph_target_animation) continue;

			pAnim = new T3DMesh<float>::SkeletalAnimation;

			pAnim->Name = animation.name;
			pAnim->Keyframes = keyframes;
			pAnim->Duration = duration;

			pMesh->addSkeletalAnimation(pAnim, false);
		}
	}
	
	void GLTFIO::readSkinningData() {
		std::vector<T3DMesh<float>::Bone*>* pBones = new std::vector<T3DMesh<float>::Bone*>;
		std::map<int, T3DMesh<float>::Bone*> bones_by_indices;

		for (Skin skin : model.skins) {
			std::vector<std::vector<float>> inverseBindMatrices;

			getAccessorData(skin.inverseBindMatrices, &inverseBindMatrices);

			std::vector<Eigen::Matrix4f> offsetMatrices;

			toMat4f(&inverseBindMatrices, &offsetMatrices);

			int counter = 0;

			for (int i : skin.joints) {
				auto pBone = new T3DMesh<float>::Bone;
				pBone->OffsetMatrix = offsetMatrices[counter];
				pBone->ID = i;
				pBones->push_back(pBone);

				bones_by_indices.emplace(i, pBone);
				
				counter++;
			}

			//link bones together
			for (int i = 0; i < pBones->size(); i++) {
				auto pBone = (*pBones)[i];
				auto boneNode = model.nodes[pBone->ID];
				
				for (int32_t c : boneNode.children) {
					auto childBone = bones_by_indices[c];
					
					pBone->Children.push_back(childBone);

					childBone->pParent = pBone;
				}
			}
		}

		pMesh->bones(pBones, false);
	}

	void GLTFIO::readMorphTargets() {
		int offset_index = 0;
		
		for (auto m : model.meshes) {
			for (auto p : m.primitives) {
				for (auto t : p.targets) {
					T3DMesh<float>::MorphTarget* pMorphtTarget = new T3DMesh<float>::MorphTarget;

					pMorphtTarget->ID = offset_index;

					int32_t buff_view = -1;
					
					for (auto keyValuePair : t) {
						if (keyValuePair.first == "POSITION") {
							std::vector<std::vector<float>> data;
							std::vector<Eigen::Vector3f> position_offsets;
							std::vector<int32_t> indices;
							
							int32_t index_buff_view = getSparseAccessorData(keyValuePair.second, &indices, &data);
							toVec3f(&data, &position_offsets);
							
							pMorphtTarget->VertexOffsets = position_offsets;

							if (buff_view == -1) {
								buff_view = index_buff_view;
								
								int32_t offset = 0;
								for (int i = 0; i < offset_index; i++) offset += offsets[i];

								for (int i = 0; i < indices.size(); i++) indices[i] += offset;

								pMorphtTarget->VertexIDs = indices;
							}
							else if (buff_view != index_buff_view) {
								std::cout << "Morph target vertex indices are not in the same buffer view." << std::endl;
							}
						}
						else if (keyValuePair.first == "NORMAL") {
							std::vector<std::vector<float>> data;
							std::vector<Eigen::Vector3f> normal_offsets;
							std::vector<int32_t> indices;
							
							int32_t index_buff_view = getSparseAccessorData(keyValuePair.second, &indices, &data);
							toVec3f(&data, &normal_offsets);
							
							pMorphtTarget->NormalOffsets = normal_offsets;

							if (buff_view == -1) {
								buff_view = index_buff_view;

								int32_t offset = 0;
								for (int i = 0; i < offset_index; i++) offset += offsets[i];

								for (int i = 0; i < indices.size(); i++) indices[i] += offset;

								pMorphtTarget->VertexIDs = indices;
							}
							else if (buff_view != index_buff_view) {
								std::cout << "Morph target vertex indices are not in the same buffer view." << std::endl;
							}
						}
					}
					
					pMesh->addMorphTarget(pMorphtTarget, false);
				}
				
				offset_index += 1;
			}
		}
	}

	/*
	Every node in gltf has a reference as submesh and some nodes also as bone in CrossForge,
	because a gltf node can fullfill both rolls.
	Bones are read in readSkinningData.
	*/
	void GLTFIO::readNodes() {
		std::vector<T3DMesh<float>::Bone*>* pBones = new std::vector<T3DMesh<float>::Bone*>;
		
		for (int i = 0; i < model.nodes.size(); i++) {
			Node node = model.nodes[i];
			
			T3DMesh<float>::Submesh* pSubmesh = new T3DMesh<float>::Submesh;

			if (node.translation.size() > 0) {
				pSubmesh->TranslationOffset(0) = node.translation[0];
				pSubmesh->TranslationOffset(1) = node.translation[1];
				pSubmesh->TranslationOffset(2) = node.translation[2];
			}

			if (node.rotation.size() > 0) {
				Eigen::Quaternionf newRotation(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
				pSubmesh->RotationOffset = newRotation;
			}

			//TODO add Scale field to submesh struct
			
			pSubmesh->pParent = nullptr;
			
			pMesh->addSubmesh(pSubmesh, false);
		}


		//Do a second pass to link all submeshes together.

		for (int i = 0; i < model.nodes.size(); i++) {
			std::vector<T3DMesh<float>::Bone*> children;
			std::vector<T3DMesh<float>::Submesh*> submeshChildren;
			
			auto pSubmesh = pMesh->getSubmesh(i);

			for (auto c : model.nodes[i].children) {
				auto pSubChild = pMesh->getSubmesh(c);
				
				pSubChild->pParent = pSubmesh;
				
				submeshChildren.push_back(pSubChild);
			}
			pSubmesh->Children = submeshChildren;
		}
	}
#pragma endregion
	
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

		primitiveIndexRanges.clear();

		filePath = Filepath;
		
		Buffer buffer;
		model.buffers.push_back(buffer);

		//Every texture will use this basic sampler.
		Sampler gltfSampler;

		gltfSampler.magFilter = TINYGLTF_TEXTURE_FILTER_LINEAR;
		gltfSampler.minFilter = TINYGLTF_TEXTURE_FILTER_LINEAR;
		gltfSampler.wrapS = TINYGLTF_TEXTURE_WRAP_REPEAT;
		gltfSampler.wrapT = TINYGLTF_TEXTURE_WRAP_REPEAT;

		model.samplers.push_back(gltfSampler);

		//Every mesh will hold a single primitive with the submesh data.
		
		writeNodes();
		
		TinyGLTF writer;

		writer.WriteGltfSceneToFile(&model, Filepath, false, false, true, false);
	}//store

#pragma region write
	
	int GLTFIO::writePrimitive(const T3DMesh<float>::Submesh* pSubmesh) {
		Mesh mesh;
		model.meshes.push_back(mesh);
		Primitive primitive;
		
		int meshIndex = model.meshes.size() - 1;

		model.meshes[meshIndex].primitives.push_back(primitive);
		
		std::pair<int, int> minmax = prepareAttributeArrays(pSubmesh);
		writeAttributes();
		writeMaterial(pSubmesh);
		writeMorphTargets(minmax);

		primitiveIndexRanges.push_back(minmax);

		return meshIndex;
	}//writePrimitive
	
	std::pair<int, int> GLTFIO::prepareAttributeArrays(const T3DMesh<float>::Submesh* pSubmesh) {
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

		std::pair<int, int> minmax(min, max);

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

		writeAccessorDataScalar(model.buffers.size() - 1, &indices);
		int meshIndex = model.meshes.size() - 1;
		model.meshes[meshIndex].primitives[0].indices = model.accessors.size() - 1;

		std::cout << "coord " << coord.size() << std::endl;
		std::cout << "normal " << normal.size() << std::endl;
		std::cout << "tangent " << tangent.size() << std::endl;
		std::cout << "texCoord " << texCoord.size() << std::endl;
		std::cout << "color " << color.size() << std::endl;
		std::cout << "joint " << joint.size() << std::endl;
		std::cout << "weight " << weight.size() << std::endl;
		
		return minmax;
	}

	void GLTFIO::writeAttributes() {
		int meshIndex = model.meshes.size() - 1;
		Primitive* pPrimitive = &(model.meshes[meshIndex].primitives[0]);
		
		int bufferIndex = model.buffers.size() - 1;
		int accessorIndex = model.accessors.size();

		std::vector<std::vector<float>> data;

		pPrimitive->attributes.emplace("POSITION", accessorIndex++);

		fromVec3f(&coord, &data);

		writeAccessorData(bufferIndex, TINYGLTF_TYPE_VEC3, &data);
		data.clear();
		
		if (normal.size() > 0) {
			pPrimitive->attributes.emplace("NORMAL", accessorIndex++);
			
			fromVec3f(&normal, &data);

			writeAccessorData(bufferIndex, TINYGLTF_TYPE_VEC3, &data);
			data.clear();
		}
		
		if (tangent.size() > 0) {
			pPrimitive->attributes.emplace("TANGENT", accessorIndex++);
			
			fromVec3f(&tangent, &data);

			writeAccessorData(bufferIndex, TINYGLTF_TYPE_VEC3, &data);
			data.clear();
		}
		
		if (texCoord.size() > 0) {
			pPrimitive->attributes.emplace("TEXCOORD_0", accessorIndex++);
			
			fromVec3f(&texCoord, &data);

			writeAccessorData(bufferIndex, TINYGLTF_TYPE_VEC2, &data);
			data.clear();
		}

		if (color.size() > 0) {
			pPrimitive->attributes.emplace("COLOR_0", accessorIndex++);
			
			fromVec4f(&color, &data);

			writeAccessorData(bufferIndex, TINYGLTF_TYPE_VEC4, &data);
			data.clear();
		}
	}

	void GLTFIO::writeMaterial(const T3DMesh<float>::Submesh* pSubmesh) {
		int meshIndex = model.meshes.size() - 1;
		
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
		Scene scene;
		std::map<const T3DMesh<float>::Submesh*, int> submeshMap;
		
		for (int i = 0; i < pCMesh->submeshCount(); i++) {
			Node newNode;
			
			const T3DMesh<float>::Submesh* pSubmesh = pCMesh->getSubmesh(i);

			submeshMap.emplace(std::make_pair(pSubmesh, i));

			if (pSubmesh->TranslationOffset(0) > -431602080.0) {
				newNode.translation.push_back(pSubmesh->TranslationOffset(0));
				newNode.translation.push_back(pSubmesh->TranslationOffset(1));
				newNode.translation.push_back(pSubmesh->TranslationOffset(2));
			}

			if (pSubmesh->RotationOffset.x() > -431602080.0) {
				newNode.rotation.push_back(pSubmesh->RotationOffset.x());
				newNode.rotation.push_back(pSubmesh->RotationOffset.y());
				newNode.rotation.push_back(pSubmesh->RotationOffset.z());
				newNode.rotation.push_back(pSubmesh->RotationOffset.w());
			}

			if (pSubmesh->Faces.size() > 0) {
				newNode.mesh = writePrimitive(pSubmesh);
			}

			model.nodes.push_back(newNode);
			scene.nodes.push_back(i);
		}

		//Do a second pass to set node children.

		for (int i = 0; i < model.nodes.size(); i++) {
			auto pSubmesh = pCMesh->getSubmesh(i);

			for (auto c : pSubmesh->Children) {
				model.nodes[i].children.push_back(submeshMap[c]);
			}
		}

		model.scenes.push_back(scene);
	}

	void GLTFIO::writeMorphTargets(std::pair<int, int> minmax) {
		for (int i = 0; i < pCMesh->morphTargetCount(); i++) {
			auto pTarget = pCMesh->getMorphTarget(i);

			int min = minmax.first;
			int max = minmax.second;

			int first_index = pTarget->VertexIDs[0];

			if (first_index >= min && first_index <= max) {
				//Morph target affects current primitve
				
				int mesh_index = model.meshes.size() - 1;
				int primitve_index = model.meshes[mesh_index].primitives.size() - 1;

				Primitive* pCurrentPrimitive = &model.meshes[mesh_index].primitives[primitve_index];

				std::map<std::string, int> targetMap;

				std::vector<int32_t>* pIndices;
				std::vector<std::vector<float>> data;

				fromVec3f(&pTarget->VertexOffsets, &data);

				int accessor_index = writeSparseAccessorData(0, TINYGLTF_TYPE_VEC3, 
					&pTarget->VertexIDs, &data);
				targetMap.emplace("POSITION", accessor_index);

				data.clear();
				fromVec3f(&pTarget->NormalOffsets, &data);

				accessor_index = writeSparseAccessorData(0, TINYGLTF_TYPE_VEC3,
					nullptr, &data);
				targetMap.emplace("NORMAL", accessor_index);

				pCurrentPrimitive->targets.push_back(targetMap);

				return;
			}
		}
	}

	void GLTFIO::writeSkinningData() {
		/*
		* TODO
		* Alle Bones als Nodes in gltf einfügen.
		* Eine Umrechnung oder Datenstruktur bereithalten um die Bone-Indices auf die Node-Indices umzurechnen, da ja schon Nodes existieren.
		* Alle Bones durchgehen und das Primitiv ermitteln was sie beeinflussen.
		* Für den Bone alle influences (indices) und weights durchgehen und in zwei Datenstrukturen (vector<vector<float>>) sammeln.
		* Die Datenstrukturen sollten die selbe Indexbasis haben wie die Attribute des Primitivs.
		* Pro Vertex gibt es also eine Liste mit Node-Indices die ihn beeinflussen und eine Liste mit den Gewichtungen für diese Nodes.
		* Diese Listen werden dann gekürzt so dass nur die stärksten 4 Einflüsse erhalten bleiben.
		* Die Listen werden dann als Joints und Weights in Accessoren geschrieben und als Attribute dem Primitiv zugefügt.
		*/

		//mesh index	vertex index	joints/weights
		std::vector<std::vector<std::vector<unsigned short>>> mesh_influences;
		std::vector<std::vector<std::vector<float>>> mesh_weights;

		for (int i = 0; i < model.meshes.size(); i++) {
			std::vector<std::vector<unsigned short>> influences;
			std::vector<std::vector<float>> weights;
			
			mesh_influences.push_back(influences);
			mesh_weights.push_back(weights);
		}

		int node_offset = model.nodes.size();

		for (int i = 0; i < pCMesh->boneCount(); i++) {
			auto pBone = pCMesh->getBone(i);
			
			Node newNode;
			newNode.name = "bone_" + std::to_string(i);

			if (pBone->Position(0) > -431602080.0) {
				newNode.translation.push_back(pBone->Position(0));
				newNode.translation.push_back(pBone->Position(1));
				newNode.translation.push_back(pBone->Position(2));
			}

			model.nodes.push_back(newNode);

			int mesh_index = getMeshIndexByCrossForgeVertexIndex(pBone->VertexInfluences[0]);
			int node_index = node_offset + i;

			for (int j = 0; j < pBone->VertexInfluences.size(); j++) {
				int32_t vertex_index = pBone->VertexInfluences[j];
				float weight = pBone->VertexWeights[j];
				
				for (int k = mesh_influences[mesh_index].size(); k <= vertex_index; k++) {
					std::vector<unsigned short> joints;
					std::vector<float> weights;
					
					mesh_influences[mesh_index].push_back(joints);
					mesh_weights[mesh_index].push_back(weights);
				}

				bool found = false;

				for (int k = 0; k < mesh_influences[mesh_index][vertex_index].size(); k++) {
					if (mesh_influences[mesh_index][vertex_index][k] == node_index) {
						found = true;
						break;
					}
				}

				if (!found) {
					mesh_influences[mesh_index][vertex_index].push_back(node_index);
					mesh_weights[mesh_index][vertex_index].push_back(weight);
				}
			}
		}

		//sort and write the 4 strongest weights per vertex
		for (int i = 0; i < mesh_influences.size(); i++) {
			for (int j = 0; j < mesh_influences[i].size(); j++) {
				auto joints = &mesh_influences[i][j];
				auto weights = &mesh_weights[i][j];
				

				//bubble sort
				bool found = true;
				while (found) {
					for (int pos = 0; pos < joints->size() - 1; pos++) {
						if ((*joints)[pos] < (*joints)[pos + 1]) {
							std::swap((*joints)[pos], (*joints)[pos + 1]);
							std::swap((*weights)[pos], (*weights)[pos + 1]);
						}
					}
				}
				
				for (int k = joints->size(); k > 4; k--) {
					joints->pop_back();
					weights->pop_back();
				}

				Primitive* pPrimitive = &model.meshes[i].primitives[0];
				
				
			}
		}
	}

#pragma endregion

	void GLTFIO::release(void) {
		delete this;
	}//release

	bool GLTFIO::accepted(const std::string Filepath, I3DMeshIO::Operation Op) {
		return (Filepath.find(".glb") != std::string::npos || Filepath.find(".gltf") != std::string::npos);
	}//accepted
	
#pragma region Util

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

	bool GLTFIO::componentIsMatrix(const int type) {
		switch (type) {
		case TINYGLTF_TYPE_MAT2:
		case TINYGLTF_TYPE_MAT3:
		case TINYGLTF_TYPE_MAT4:
			return true;
		}

		return false;
	}
	
	//returns indices buffer view to identify index data
	int32_t GLTFIO::getSparseAccessorData(const int accessor, std::vector<int32_t>* pIndices, std::vector<std::vector<float>>* pData) {
		Accessor acc = model.accessors[accessor];
		
		if (!acc.sparse.isSparse) {
			std::cout << "Accessor is not sparse" << std::endl;
			
			return -1;
		}

		int component_count = componentCount(acc.type);
		
		int index_type = acc.sparse.indices.componentType;
		int index_type_size = sizeOfGltfComponentType(index_type);
		int index_byte_offset = acc.sparse.indices.byteOffset;
		BufferView index_buff_view = model.bufferViews[acc.sparse.indices.bufferView];
		Buffer index_buff = model.buffers[index_buff_view.buffer];

		int value_type = acc.componentType;
		int value_type_size = sizeOfGltfComponentType(acc.componentType);
		int value_byte_offset = acc.sparse.values.byteOffset;
		BufferView value_buff_view = model.bufferViews[acc.sparse.values.bufferView];
		Buffer value_buff = model.buffers[value_buff_view.buffer];
		
		

		//read index data

		if (index_type == TINYGLTF_COMPONENT_TYPE_BYTE) {
			std::vector<char> indices;
			readBuffer(index_buff.data.data(), acc.sparse.count, acc.byteOffset + index_byte_offset, 1, false, 0, &indices);
			for (int i = 0; i < acc.sparse.count; i++) {
				pIndices->push_back((int32_t)(indices[i]));
			}
		}
		else if (index_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
			std::vector<unsigned char> indices;
			readBuffer(index_buff.data.data(), acc.sparse.count, acc.byteOffset + index_byte_offset, 1, false, 0, &indices);
			for (int i = 0; i < acc.sparse.count; i++) {
				pIndices->push_back((int32_t)(indices[i]));
			}
		}
		else if (index_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
			std::vector<unsigned short> indices;
			readBuffer(index_buff.data.data(), acc.sparse.count, acc.byteOffset + index_byte_offset, 1, false, 0, &indices);
			for (int i = 0; i < acc.sparse.count; i++) {
				pIndices->push_back((int32_t)(indices[i]));
			}
		}
		else if (index_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
			std::vector<uint32_t> indices;
			readBuffer(index_buff.data.data(), acc.sparse.count, acc.byteOffset + index_byte_offset, 1, false, 0, &indices);
			for (int i = 0; i < acc.sparse.count; i++) {
				pIndices->push_back((int32_t)(indices[i]));
			}
		}
		else if (index_type == TINYGLTF_COMPONENT_TYPE_INT) {
			readBuffer(index_buff.data.data(), acc.sparse.count, acc.byteOffset + index_byte_offset, 1, false, 0, pIndices);
		}
		else {
			std::cout << "Unsupported index type" << std::endl;
		}

		
		//read value data

		readBuffer(value_buff.data.data(), acc.sparse.count, acc.byteOffset + value_byte_offset, component_count, componentIsMatrix(acc.type), 0, pData);

		return acc.sparse.indices.bufferView;
	}
	
	//reads normalized integers and returns floats
	void GLTFIO::getAccessorDataScalarFloat(const int accessor, std::vector<float>* pData) {
		Accessor acc = model.accessors[accessor];
		
		if (acc.componentType == TINYGLTF_COMPONENT_TYPE_BYTE) {
			std::vector<char> normalizedData;
			getAccessorDataScalar(accessor, &normalizedData);
			for (auto d : normalizedData) {
				pData->push_back(std::max(d / 127.0f, -1.0f));
			}
		}
		else if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
			std::vector<unsigned char> normalizedData;
			getAccessorDataScalar(accessor, &normalizedData);
			for (auto d : normalizedData) {
				pData->push_back(d / 255.0f);
			}
		}
		else if (acc.componentType == TINYGLTF_COMPONENT_TYPE_SHORT) {
			std::vector<short> normalizedData;
			getAccessorDataScalar(accessor, &normalizedData);
			for (auto d : normalizedData) {
				pData->push_back(std::max(d / 32767.0f, -1.0f));
			}
		}
		else if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
			std::vector<unsigned short> normalizedData;
			getAccessorDataScalar(accessor, &normalizedData);
			for (auto d : normalizedData) {
				pData->push_back(d / 65535.0f);
			}
		}
		else if (acc.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
			getAccessorDataScalar(accessor, pData);
		}
		else {
			std::cout << "Unsupported component type for normalized integer conversion: " << acc.componentType << std::endl;
		}
	}

	void GLTFIO::getAccessorData(const int accessor, std::vector<Eigen::Vector3f>* pData) {
		std::vector<std::vector<float>> vData;

		getAccessorDataFloat(accessor, &vData);

		toVec3f(&vData, pData);
	}

	void GLTFIO::getAccessorData(const int accessor, std::vector<Eigen::Vector4f>* pData) {
		std::vector<std::vector<float>> vData;

		getAccessorDataFloat(accessor, &vData);

		toVec4f(&vData, pData);
	}

	void GLTFIO::getAccessorData(const int accessor, std::vector<Eigen::Quaternionf>* pData) {
		std::vector<std::vector<float>> vData;

		getAccessorDataFloat(accessor, &vData);

		toQuatf(&vData, pData);
	}

	//Construct floats from normalized integers.
	void GLTFIO::getAccessorDataFloat(const int accessor, std::vector<std::vector<float>>* pData) {
		Accessor acc = model.accessors[accessor];

		if (acc.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) getAccessorData(accessor, pData);
		else if (acc.componentType == TINYGLTF_COMPONENT_TYPE_BYTE) {
			std::vector<std::vector<char>> normalizedData;
			getAccessorData(accessor, &normalizedData);
			for (auto e : normalizedData) {
				std::vector<float> toAdd;
				for (auto d : e) toAdd.push_back(std::max(d / 127.0f, -1.0f));
				pData->push_back(toAdd);
			}
		}
		else if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
			std::vector<std::vector<unsigned char>> normalizedData;
			getAccessorData(accessor, &normalizedData);
			for (auto e : normalizedData) {
				std::vector<float> toAdd;
				for (auto d : e) toAdd.push_back(d / 255.0f);
				pData->push_back(toAdd);
			}
		}
		else if (acc.componentType == TINYGLTF_COMPONENT_TYPE_SHORT) {
			std::vector<std::vector<short>> normalizedData;
			getAccessorData(accessor, &normalizedData);
			for (auto e : normalizedData) {
				std::vector<float> toAdd;
				for (auto d : e) toAdd.push_back(std::max(d / 32767.0f, -1.0f));
				pData->push_back(toAdd);
			}
		}
		else if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
			std::vector<std::vector<unsigned short>> normalizedData;
			getAccessorData(accessor, &normalizedData);
			for (auto e : normalizedData) {
				std::vector<float> toAdd;
				for (auto d : e) toAdd.push_back(d / 65535.0f);
				pData->push_back(toAdd);
			}
		}
		else std::cout << "Unsupported component type: " << acc.componentType << " for normalized integer conversion!" << std::endl;
	}

	int GLTFIO::writeSparseAccessorData(const int buffer_index, const int type, const std::vector<int32_t>* pIndices, const std::vector<std::vector<float>>* pData) {
		Buffer* pBuffer = &model.buffers[buffer_index];

		Accessor accessor;

		accessor.bufferView = 0;
		accessor.byteOffset = 0;
		accessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
		accessor.type = type;
		accessor.count = pData->size();
		accessor.sparse.isSparse = true;
		accessor.sparse.count = pData->size();


		if (pIndices != nullptr) {
			BufferView index_buffer_view;

			index_buffer_view.byteOffset = pBuffer->data.size();
			index_buffer_view.buffer = buffer_index;
			index_buffer_view.byteLength = pIndices->size() * sizeof(int32_t);
			index_buffer_view.byteStride = 0;

			accessor.sparse.indices.bufferView = model.bufferViews.size();
			accessor.sparse.indices.componentType = TINYGLTF_COMPONENT_TYPE_INT;

			model.bufferViews.push_back(index_buffer_view);

			writeBuffer(&pBuffer->data, index_buffer_view.byteOffset + accessor.byteOffset,
				accessor.sparse.count, false, index_buffer_view.byteStride, pIndices);
		}

		BufferView value_buffer_view;

		value_buffer_view.byteOffset = pBuffer->data.size();
		value_buffer_view.buffer = buffer_index;
		value_buffer_view.byteLength = pData->size() * sizeof(float) * componentCount(type);
		value_buffer_view.byteStride = 0;

		accessor.sparse.values.bufferView = model.bufferViews.size();

		model.bufferViews.push_back(value_buffer_view);

		bool is_matrix = componentIsMatrix(type);


		writeBuffer(&pBuffer->data, value_buffer_view.byteOffset + accessor.byteOffset, 
			accessor.sparse.count, is_matrix, value_buffer_view.byteStride, pData);


		model.accessors.push_back(accessor);

		return model.accessors.size() - 1;
	}

	void GLTFIO::toVec3f(const std::vector<std::vector<float>>* pIn, std::vector<Eigen::Vector3f>* pOut) {
		for (auto e : *pIn) {
			Eigen::Vector3f vec;

			vec(0) = e[0];
			vec(1) = e[1];
			vec(2) = e[2];
			pOut->push_back(vec);
		}
	}

	void GLTFIO::toVec4f(const std::vector<std::vector<float>>* pIn, std::vector<Eigen::Vector4f>* pOut) {
		for (auto e : *pIn) {
			Eigen::Vector4f vec;

			vec(0) = e[0];
			vec(1) = e[1];
			vec(2) = e[2];
			vec(3) = e[3];
			pOut->push_back(vec);
		}
	}

	void GLTFIO::toQuatf(const std::vector<std::vector<float>>* pIn, std::vector<Eigen::Quaternionf>* pOut) {
		for (auto e : *pIn) {
			Eigen::Quaternionf quat(e[3], e[0], e[1], e[2]);

			pOut->push_back(quat);
		}
	}

	void GLTFIO::toMat4f(const std::vector<std::vector<float>>* pIn, std::vector<Eigen::Matrix4f>* pOut) {
		for (auto vec : *pIn) {
			Eigen::Matrix4f mat;

			for (int i = 0; i < vec.size(); i++) {
				mat(i % 4, i / 4) = vec[i];
			}

			pOut->push_back(mat);
		}
	}

	void GLTFIO::fromVec3f(const std::vector<Eigen::Vector3f>* pIn, std::vector<std::vector<float>>* pOut) {
		for (auto e : *pIn) {
			std::vector<float> toAdd;
			toAdd.push_back(e(0));
			toAdd.push_back(e(1));
			toAdd.push_back(e(2));
			pOut->push_back(toAdd);
		}
	}

	void GLTFIO::fromVec4f(const std::vector<Eigen::Vector4f>* pIn, std::vector<std::vector<float>>* pOut) {
		for (auto e : *pIn) {
			std::vector<float> toAdd;
			toAdd.push_back(e(0));
			toAdd.push_back(e(1));
			toAdd.push_back(e(2));
			toAdd.push_back(e(3));
			pOut->push_back(toAdd);
		}
	}

	void GLTFIO::fromQuatf(const std::vector<Eigen::Quaternionf>* pIn, std::vector<std::vector<float>>* pOut) {
		for (auto e : *pIn) {
			std::vector<float> toAdd;
			toAdd.push_back(e.x());
			toAdd.push_back(e.y());
			toAdd.push_back(e.z());
			toAdd.push_back(e.w());
			pOut->push_back(toAdd);
		}
	}

	void GLTFIO::fromMat4f(const std::vector<Eigen::Matrix4f>* pIn, std::vector<std::vector<float>>* pOut) {
		for (auto mat : *pIn) {
			std::vector<float> toAdd;
			for (int i = 0; i < 16; i++) {
				toAdd.push_back(mat(i % 4, i / 4));
			}
			pOut->push_back(toAdd);
		}
	}

	/*
	* Returns the gltf mesh index for a given index of a vertex in the CrossForge Mesh.
	* Only works if the submeshes have already been processed into primitives during writing.
	*/
	int GLTFIO::getMeshIndexByCrossForgeVertexIndex(int index) {
		for (int i = 0; i < primitiveIndexRanges.size(); i++) {
			auto minmax = primitiveIndexRanges[i];
			
			if (index >= minmax.first && index <= minmax.second) return i;
		}

		return -1;
	}

#pragma endregion
}//name space
 
//TODO
/*
* Wo muss Rotation und Scale pro Node hin? Bone hat nur Position und Offsetmatrix. -> Submeshs speichern sowas.
* - Alle Nodes die an Skelettanimationen beteiligt sind werden auch als Bones gespeichert. WICHTIG.
* 
* Skelettanimationen schreiben.
* Eingebettete Texturen unterstützen. -> Ja beim einlesen mit AssetIO
* Was passiert mit Skelettanimationen mit unterschiedlichen Keyframes? -> ggf. Umrechnen
* Node Matritzen in rotation, translation und ggf. scale zerlegen.
*/