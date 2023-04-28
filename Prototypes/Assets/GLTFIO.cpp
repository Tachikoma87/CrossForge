#include "GLTFIO.h"

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

#pragma region read

	void GLTFIO::readMeshes() {
		for (int i = 0; i < m_model.meshes.size(); i++) {
			Mesh currentMesh = m_model.meshes[i];

			T3DMesh<float>::Submesh* pParentSubmesh;

			for (int j = 0; j < m_model.nodes.size(); j++) {
				if (m_model.nodes[j].mesh == i) {
					pParentSubmesh = m_pMesh->getSubmesh(j);
					break;
				}
			}

			for (int k = 0; k < currentMesh.primitives.size(); k++) {
				Primitive currentPrimitive = currentMesh.primitives[k];

				auto pSubmesh = readPrimitive(&currentPrimitive);

				pParentSubmesh->Children.push_back(pSubmesh);
				pSubmesh->pParent = pParentSubmesh;
			}
		}

		auto pCoord = new std::vector<Eigen::Matrix<float, 3, 1>>;
		for (auto i : m_coord) pCoord->push_back(i);
		auto pNormal = new std::vector<Eigen::Matrix<float, 3, 1>>;
		for (auto i : m_normal) pNormal->push_back(i);
		auto pTangent = new std::vector<Eigen::Matrix<float, 3, 1>>;
		for (auto i : m_tangent) pTangent->push_back(i);
		auto pTexCoord = new std::vector<Eigen::Matrix<float, 3, 1>>;
		fromUVtoUVW(&m_texCoord, pTexCoord);

		m_pMesh->vertices(pCoord);
		m_pMesh->normals(pNormal);
		m_pMesh->tangents(pTangent);
		m_pMesh->textureCoordinates(pTexCoord);
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

					m_coord.push_back(mat);

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

					m_normal.push_back(mat);
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

					m_tangent.push_back(mat);
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
					Eigen::Matrix<float, 2, 1> mat;

					mat(0) = pos[0];
					mat(1) = 1.0f - pos[1];

					m_texCoord.push_back(mat);
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
					m_joint.push_back(j);
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
					m_weight.push_back(w);
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
					m_color.push_back(c);
				}

				continue;
			}
		}//for attributes

		//fill up the rest of the attributes with default values
		for (int i = m_normal.size(); i < m_coord.size(); i++) {
			Eigen::Vector3f vec(0, 0, 0);
			m_normal.push_back(vec);
		}
		for (int i = m_tangent.size(); i < m_coord.size(); i++) {
			Eigen::Vector3f vec(0, 0, 0);
			m_tangent.push_back(vec);
		}
		for (int i = m_texCoord.size(); i < m_coord.size(); i++) {
			Eigen::Vector2f vec(0, 0);
			m_texCoord.push_back(vec);
		}
		for (int i = m_joint.size(); i < m_coord.size(); i++) {
			Eigen::Vector4f vec(-1, -1, -1, -1);
			m_joint.push_back(vec);
		}
		for (int i = m_weight.size(); i < m_coord.size(); i++) {
			Eigen::Vector4f vec(0, 0, 0, 0);
			m_weight.push_back(vec);
		}
		for (int i = m_color.size(); i < m_coord.size(); i++) {
			Eigen::Vector4f vec(0, 0, 0, 0);
			m_color.push_back(vec);
		}

		//set vertex influences and weights for bones
		for (int i = 0; i < m_joint.size(); i++) {
			for (int j = 0; j < 4; j++) {
				int32_t index = (int32_t)m_joint[i](j);
				if (index < 0) continue;
				auto pBone = m_pMesh->getBone(index);
				pBone->VertexInfluences.push_back(i);
				pBone->VertexWeights.push_back(m_weight[i](j));
			}
		}

		m_offsets.push_back(counter);
	}

	T3DMesh<float>::Submesh* GLTFIO::readSubMeshes(Primitive* pPrimitive) {
		T3DMesh<float>::Submesh* pSubMesh = new T3DMesh<float>::Submesh;

		std::vector<T3DMesh<float>::Face> faces;
		readFaces(pPrimitive, &faces);
		pSubMesh->Faces = faces;

		T3DMesh<float>::Material* pMaterial = new T3DMesh<float>::Material;

		if (pPrimitive->material != -1) {
			readMaterial(pPrimitive->material, pMaterial);
		}

		m_pMesh->addMaterial(pMaterial, false);
		pSubMesh->Material = m_materialIndex;
		m_materialIndex++;

		m_pMesh->addSubmesh(pSubMesh, false);

		return pSubMesh;
	}

	void GLTFIO::readFaces(Primitive* pPrimitive, std::vector<T3DMesh<float>::Face>* faces) {
		int accessorIndex = pPrimitive->indices;

		std::vector<int> indices;
		getAccessorDataScalar(accessorIndex, &indices);

		int offset = 0;
		for (int i = 0; i < m_offsets.size() - 1; i++) offset += m_offsets[i];

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

	void GLTFIO::readMaterial(const int m_materialIndex, T3DMesh<float>::Material* pMaterial) {
		Material gltfMaterial = m_model.materials[m_materialIndex];

		// pMaterial->ID = m_materialIndex;

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
		if (textureIndex < 0 || textureIndex >= m_model.textures.size()) return "";

		int sourceIndex = m_model.textures[textureIndex].source;

		Image* pImage = &m_model.images[sourceIndex];

		STD_FS::path path = m_filePath;
		path = path.parent_path();

		if (pImage->uri.size()) {
			//combine texture uri with gltf file parent directory to get the full path
			path.append(m_model.images[sourceIndex].uri);

			return path.string();
		}

		//embedded texture

		if (pImage->mimeType == "image/jpeg") {
			path.append("image_" + std::to_string(textureIndex) + ".jpg");
		}
		else {
			path.append("image_" + std::to_string(textureIndex) + "." + pImage->mimeType.substr(6));
		}

		//no buffer view defined or buffer view is out of range
		if (pImage->bufferView < 0 || pImage->bufferView >= m_model.bufferViews.size()) return "";

		BufferView* pBufferView = &m_model.bufferViews[pImage->bufferView];
		Buffer* pBuffer = &m_model.buffers[pBufferView->buffer];

		std::ofstream fout(path.string(), std::ios::out | std::ios::binary);
		fout.write((const char*)pBuffer->data.data() + pBufferView->byteOffset, pBufferView->byteLength);
		fout.close();

		return path.string();
	}

	void GLTFIO::readSkeletalAnimations() {
		T3DMesh<float>::SkeletalAnimation* pAnim;

		int id_counter = 0;

		for (Animation animation : m_model.animations) {
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
			pAnim->Speed = 1.0;

			m_pMesh->addSkeletalAnimation(pAnim, false);
		}
	}

	void GLTFIO::readSkinningData() {
		std::vector<T3DMesh<float>::Bone*>* pBones = new std::vector<T3DMesh<float>::Bone*>;
		std::map<int, T3DMesh<float>::Bone*> bones_by_indices;

		for (Skin skin : m_model.skins) {
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
				auto boneNode = m_model.nodes[pBone->ID];

				for (int32_t c : boneNode.children) {
					auto childBone = bones_by_indices[c];

					pBone->Children.push_back(childBone);

					childBone->pParent = pBone;
				}
			}
		}

		m_pMesh->bones(pBones, false);
	}

	void GLTFIO::readMorphTargets() {
		int offset_index = 0;

		for (auto m : m_model.meshes) {
			for (auto p : m.primitives) {
				for (auto t : p.targets) {
					T3DMesh<float>::MorphTarget* pMorphTarget = new T3DMesh<float>::MorphTarget;

					pMorphTarget->ID = offset_index;

					int32_t last_index_buff_view = -1;

					int32_t index_offset = 0;
					for (int i = 0; i < offset_index; i++) index_offset += m_offsets[i];

					for (auto keyValuePair : t) {
						std::vector<std::vector<float>> data;
						std::vector<Eigen::Vector3f> attribute_offsets;
						std::vector<int32_t> indices;

						int32_t index_buff_view = getSparseAccessorData(keyValuePair.second, &indices, &data);

						//Accessor is not sparse
						if (index_buff_view == -1) {
							getAccessorData(keyValuePair.second, &data);
							for (int i = 0; i < data.size(); i++) indices.push_back(i);
						}

						for (int i = 0; i < indices.size(); i++) indices[i] += index_offset;

						toVec3f(&data, &attribute_offsets);

						if (keyValuePair.first == "POSITION") {
							pMorphTarget->VertexOffsets = attribute_offsets;
							pMorphTarget->VertexIDs = indices;
						}
						else if (keyValuePair.first == "NORMAL") {
							pMorphTarget->NormalOffsets = attribute_offsets;
							pMorphTarget->VertexIDs = indices;

						}
					}

					if (pMorphTarget->VertexIDs.size()) m_pMesh->addMorphTarget(pMorphTarget, false);
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

		for (int i = 0; i < m_model.nodes.size(); i++) {
			Node node = m_model.nodes[i];

			T3DMesh<float>::Submesh* pSubmesh = new T3DMesh<float>::Submesh;

			if (node.matrix.size()) {
				auto mat = toSingleMat4(&node.matrix);

				pSubmesh->TranslationOffset = getTranslation(mat);
				pSubmesh->RotationOffset = getRotation(mat);

				auto scale = getScale(mat);

				if (std::abs(scale(0) - 1.0) > 0.001 || std::abs(scale(1) - 1.0) > 0.001 || std::abs(scale(2) - 1.0) > 0.001) {
					std::cout << "SCALE FOUND!!!" << std::endl;
				}
			}
			else {
				if (node.translation.size()) {
					pSubmesh->TranslationOffset(0) = node.translation[0];
					pSubmesh->TranslationOffset(1) = node.translation[1];
					pSubmesh->TranslationOffset(2) = node.translation[2];
				}

				if (node.rotation.size()) {
					Eigen::Quaternionf newRotation(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
					pSubmesh->RotationOffset = newRotation;
				}
			}

			//TODO add Scale field to submesh struct

			pSubmesh->pParent = nullptr;

			m_pMesh->addSubmesh(pSubmesh, false);
		}


		//Do a second pass to link all submeshes together.

		for (int i = 0; i < m_model.nodes.size(); i++) {
			std::vector<T3DMesh<float>::Bone*> children;
			std::vector<T3DMesh<float>::Submesh*> submeshChildren;

			auto pSubmesh = m_pMesh->getSubmesh(i);

			for (auto c : m_model.nodes[i].children) {
				auto pSubChild = m_pMesh->getSubmesh(c);

				pSubChild->pParent = pSubmesh;

				submeshChildren.push_back(pSubChild);
			}
			pSubmesh->Children = submeshChildren;
		}
	}
#pragma endregion

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

#pragma region write

	int GLTFIO::writePrimitive(const T3DMesh<float>::Submesh* pSubmesh) {
		Mesh mesh;
		m_model.meshes.push_back(mesh);
		Primitive primitive;

		primitive.mode = TINYGLTF_MODE_TRIANGLES;

		int meshIndex = m_model.meshes.size() - 1;

		m_model.meshes[meshIndex].primitives.push_back(primitive);

		std::pair<int, int> minmax = prepareAttributeArrays(pSubmesh);
		writeAttributes();
		writeMaterial(pSubmesh);
		writeMorphTargets(minmax);

		m_primitiveIndexRanges.push_back(minmax);

		return meshIndex;
	}//writePrimitive

	std::pair<int, int> GLTFIO::prepareAttributeArrays(const T3DMesh<float>::Submesh* pSubmesh) {
		m_coord.clear();
		m_normal.clear();
		m_tangent.clear();
		m_texCoord.clear();
		m_color.clear();
		m_joint.clear();
		m_weight.clear();

		int32_t min = -1;
		int32_t max = -1;

		std::vector<int32_t> indices;

		bool values_found = false;

		for (int i = 0; i < pSubmesh->Faces.size(); i++) {
			auto face = pSubmesh->Faces[i];

			for (int j = 0; j < 3; j++) {
				int32_t index = face.Vertices[j];

				if (index < 0) index = 0;

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

			auto current_index = indices[i];

			// fill up to index
			for (int k = m_coord.size(); k <= current_index - min; k++) {
				Eigen::Vector3f vec3(0, 0, 0);
				m_coord.push_back(vec3);
			}
			auto pos = m_pCMesh->vertex(current_index);
			m_coord[current_index - min] = pos;

			if (m_pCMesh->normalCount() > 0) {
				// fill up to index
				for (int k = m_normal.size(); k <= current_index - min; k++) {
					Eigen::Vector3f vec3(0, 0, 0);
					m_normal.push_back(vec3);
				}
				auto norm = m_pCMesh->normal(current_index).normalized();
				m_normal[current_index - min] = norm;
			}

			if (m_pCMesh->tangentCount() > 0) {
				// fill up to index
				for (int k = m_tangent.size(); k <= current_index - min; k++) {
					Eigen::Vector3f vec3(0, 0, 0);
					m_tangent.push_back(vec3);
				}
				auto tan = m_pCMesh->tangent(current_index);
				m_tangent[current_index - min] = tan;
			}

			if (m_pCMesh->textureCoordinatesCount() > 0) {
				// fill up to index
				for (int k = m_texCoord.size(); k <= current_index - min; k++) {
					Eigen::Vector2f vec2(0, 0);
					m_texCoord.push_back(vec2);
				}

				if (current_index >= m_pCMesh->textureCoordinatesCount()) continue;

				auto tex = m_pCMesh->textureCoordinate(current_index);
				tex(1) = 1.0f - tex(1);
				m_texCoord[current_index - min](0) = tex(0);
				m_texCoord[current_index - min](1) = tex(1);
			}

			if (m_pCMesh->colorCount() > 0) {
				// fill up to index
				for (int k = m_color.size(); k <= current_index - min; k++) {
					Eigen::Vector4f vec4(0, 0, 0, 0);
					m_color.push_back(vec4);
				}
				auto meshCol = m_pCMesh->color(current_index);
				Eigen::Vector4f col;
				col(0) = meshCol(0);
				col(1) = meshCol(1);
				col(2) = meshCol(2);
				col(3) = meshCol(3);
				m_color[current_index - min] = col;
			}

			indices[i] -= min;
		}

		writeAccessorDataScalar(m_model.buffers.size() - 1, &indices);
		int meshIndex = m_model.meshes.size() - 1;
		m_model.meshes[meshIndex].primitives[0].indices = m_model.accessors.size() - 1;

		std::cout << "m_coord " << m_coord.size() << std::endl;
		std::cout << "m_normal " << m_normal.size() << std::endl;
		std::cout << "m_tangent " << m_tangent.size() << std::endl;
		std::cout << "m_texCoord " << m_texCoord.size() << std::endl;
		std::cout << "m_color " << m_color.size() << std::endl;
		std::cout << "m_joint " << m_joint.size() << std::endl;
		std::cout << "m_weight " << m_weight.size() << std::endl;

		return minmax;
	}

	void GLTFIO::writeAttributes() {
		int meshIndex = m_model.meshes.size() - 1;
		Primitive* pPrimitive = &(m_model.meshes[meshIndex].primitives[0]);

		int bufferIndex = 0;
		int accessorIndex = m_model.accessors.size();

		std::vector<std::vector<float>> data;

		pPrimitive->attributes.emplace("POSITION", accessorIndex++);

		fromVec3f(&m_coord, &data);

		writeAccessorData(bufferIndex, TINYGLTF_TYPE_VEC3, &data);
		data.clear();

		if (m_normal.size() > 0) {
			pPrimitive->attributes.emplace("NORMAL", accessorIndex++);

			for (int i = 0; i < m_normal.size(); i++) {
				if (m_normal[i].norm() != 1.0) {
					m_normal[i](0) = 1.0;
					m_normal[i](1) = 0.0;
					m_normal[i](2) = 0.0;
				}
			}

			fromVec3f(&m_normal, &data);

			writeAccessorData(bufferIndex, TINYGLTF_TYPE_VEC3, &data);
			data.clear();
		}

		if (m_tangent.size() > 0) {
			bool skip_tangents = false;

			//normalize tangents
			for (int i = 0; i < m_tangent.size(); i++) {
				m_tangent[i].normalize();
				if (m_tangent[i].norm() == 0) {
					skip_tangents = true;
					break;
				}
			}

			if (!skip_tangents) {
				pPrimitive->attributes.emplace("TANGENT", accessorIndex++);

				fromVec3f(&m_tangent, &data);

				//add w component for compatibility
				for (int i = 0; i < data.size(); i++) {
					data[i].push_back(-1.0f);
				}


				writeAccessorData(bufferIndex, TINYGLTF_TYPE_VEC4, &data);
				data.clear();
			}
		}

		if (m_texCoord.size() > 0) {
			pPrimitive->attributes.emplace("TEXCOORD_0", accessorIndex++);

			fromVec2f(&m_texCoord, &data);

			writeAccessorData(bufferIndex, TINYGLTF_TYPE_VEC2, &data);
			data.clear();
		}

		if (m_color.size() > 0) {
			pPrimitive->attributes.emplace("COLOR_0", accessorIndex++);

			fromVec4f(&m_color, &data);

			writeAccessorData(bufferIndex, TINYGLTF_TYPE_VEC4, &data);
			data.clear();
		}
	}

	void GLTFIO::writeMaterial(const T3DMesh<float>::Submesh* pSubmesh) {
		int meshIndex = m_model.meshes.size() - 1;

		if (pSubmesh->Material < 0) {
			m_model.meshes[meshIndex].primitives[0].material = -1;
			return;
		}

		const T3DMesh<float>::Material* pMaterial = m_pCMesh->getMaterial(pSubmesh->Material);

		Material gltfMaterial;

		gltfMaterial.pbrMetallicRoughness.metallicFactor = pMaterial->Metallic;
		gltfMaterial.pbrMetallicRoughness.roughnessFactor = pMaterial->Roughness;

		gltfMaterial.normalTexture.index = writeTexture(pMaterial->TexNormal);
		gltfMaterial.pbrMetallicRoughness.baseColorTexture.index = writeTexture(pMaterial->TexAlbedo);
		gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index = writeTexture(pMaterial->TexMetallicRoughness);
		gltfMaterial.emissiveTexture.index = writeTexture(pMaterial->TexEmissive);
		gltfMaterial.occlusionTexture.index = writeTexture(pMaterial->TexOcclusion);

		gltfMaterial.pbrMetallicRoughness.baseColorFactor[0] = std::max(pMaterial->Color(0), 0.0f);
		gltfMaterial.pbrMetallicRoughness.baseColorFactor[1] = std::max(pMaterial->Color(1), 0.0f);
		gltfMaterial.pbrMetallicRoughness.baseColorFactor[2] = std::max(pMaterial->Color(2), 0.0f);
		gltfMaterial.pbrMetallicRoughness.baseColorFactor[3] = std::max(pMaterial->Color(3), 0.0f);

		m_model.meshes[meshIndex].primitives[0].material = m_model.materials.size();

		m_model.materials.push_back(gltfMaterial);
	}

	int GLTFIO::writeTexture(const std::string path) {
		if (path.empty() || !STD_FS::exists(path)) return -1;

		STD_FS::path texPath(path);

		std::string extension = texPath.extension().string();
		if (extension == ".jpg") extension = ".jpeg";

		Texture gltfTexture;
		gltfTexture.source = m_model.images.size();

		Image gltfImage;
		gltfImage.mimeType = "image/" + extension.substr(1, extension.length() - 1);
		gltfImage.bufferView = m_model.bufferViews.size();


		BufferView imageBufferView;
		imageBufferView.buffer = 1;
		Buffer* pBuffer = &m_model.buffers[imageBufferView.buffer];
		imageBufferView.byteOffset = pBuffer->data.size();
		imageBufferView.byteLength = STD_FS::file_size(texPath);

		std::ifstream infile(texPath, std::ios_base::binary);

		pBuffer->data.reserve(pBuffer->data.size() + imageBufferView.byteLength);

		std::copy(std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>(), std::back_inserter(pBuffer->data));

		m_model.bufferViews.push_back(imageBufferView);
		m_model.images.push_back(gltfImage);

		gltfTexture.sampler = 0;

		m_model.textures.push_back(gltfTexture);

		return m_model.textures.size() - 1;
	}

	void GLTFIO::writeNodes() {
		Scene scene;
		std::map<const T3DMesh<float>::Submesh*, int> submeshMap;

		for (int i = 0; i < m_pCMesh->submeshCount(); i++) {
			Node newNode;

			newNode.name = std::to_string(i);

			const T3DMesh<float>::Submesh* pSubmesh = m_pCMesh->getSubmesh(i);

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

			m_model.nodes.push_back(newNode);
		}

		//Do a second pass to set node children.
		std::vector<int> allChildren;

		for (int i = 0; i < m_model.nodes.size(); i++) {
			auto pSubmesh = m_pCMesh->getSubmesh(i);

			for (auto c : pSubmesh->Children) {
				int childNode = submeshMap[c];

				allChildren.push_back(childNode);
				m_model.nodes[i].children.push_back(childNode);
			}
		}

		//Find root nodes.
		for (int i = 0; i < m_model.nodes.size(); i++) {
			if (std::find(allChildren.begin(), allChildren.end(), i) == allChildren.end()) {
				scene.nodes.push_back(i);
			}
		}

		m_model.scenes.push_back(scene);
	}

	void GLTFIO::writeMorphTargets(std::pair<int, int> minmax) {
		for (int i = 0; i < m_pCMesh->morphTargetCount(); i++) {
			auto pTarget = m_pCMesh->getMorphTarget(i);

			int min = minmax.first;
			int max = minmax.second;

			int first_index = pTarget->VertexIDs[0];

			if (first_index >= min && first_index <= max) {
				//Morph target affects current primitve

				int mesh_index = m_model.meshes.size() - 1;
				int primitve_index = m_model.meshes[mesh_index].primitives.size() - 1;

				Primitive* pCurrentPrimitive = &m_model.meshes[mesh_index].primitives[primitve_index];

				std::map<std::string, int> targetMap;
				std::vector<std::vector<float>> data;

				if (pTarget->VertexIDs.size() < m_coord.size()) {
					std::vector<int32_t>* pIndices;

					if (pTarget->VertexOffsets.size()) {
						fromVec3f(&pTarget->VertexOffsets, &data);

						int accessor_index = writeSparseAccessorData(0, TINYGLTF_TYPE_VEC3,
							&pTarget->VertexIDs, &data);
						targetMap.emplace("POSITION", accessor_index);

						data.clear();
					}

					if (pTarget->NormalOffsets.size()) {
						fromVec3f(&pTarget->NormalOffsets, &data);

						int accessor_index = writeSparseAccessorData(0, TINYGLTF_TYPE_VEC3,
							nullptr, &data);
						targetMap.emplace("NORMAL", accessor_index);
					}
				}
				else {
					if (pTarget->VertexOffsets.size()) {
						fromVec3f(&pTarget->VertexOffsets, &data);

						writeAccessorData(0, TINYGLTF_TYPE_VEC3, &data);
						int accessor_index = m_model.accessors.size() - 1;

						targetMap.emplace("POSITION", accessor_index);

						data.clear();
					}

					if (pTarget->NormalOffsets.size()) {
						fromVec3f(&pTarget->NormalOffsets, &data);

						writeAccessorData(0, TINYGLTF_TYPE_VEC3, &data);
						int accessor_index = m_model.accessors.size() - 1;

						targetMap.emplace("NORMAL", accessor_index);
					}
				}

				if (targetMap.size()) {
					pCurrentPrimitive->targets.push_back(targetMap);
				}

				return;
			}
		}
	}

	void GLTFIO::writeSkinningData() {
		if (m_pCMesh->boneCount() == 0) return;

		/*
		* Vorgehensweise:
		* Alle Bones als Nodes in gltf einfügen.
		* Eine Umrechnung oder Datenstruktur bereithalten, um die Bone-Indices auf die Node-Indices umzurechnen, da ja schon Nodes existieren.
		* Alle Bones durchgehen und das Primitiv ermitteln was sie beeinflussen.
		* Nun muss eine Umwandlung gefunden werden, sodass nicht die Vertices pro Knochen aufgelistet werden,
		* sondern die 4 beeinflussenden Knochen für jeden Vertex.
		* Dafür pro Bone alle influences (indices) und weights durchgehen und in zwei Datenstrukturen (vector<vector<float>>) sammeln.
		* Die Datenstrukturen sollten die selbe Indexbasis haben wie die Attribute des Primitivs.
		* Dadurch entstehen pro Vertex eine Liste mit Knochen und Gewichten, welche Schritt für Schritt mit den Werten ausgefüllt werden.
		* Pro Vertex gibt es also eine Liste mit Node-Indices die ihn beeinflussen und eine Liste mit den Gewichtungen für diese Nodes.
		* Diese Listen werden dann gekürzt so dass nur die stärksten 4 Einflüsse erhalten bleiben.
		* Die Listen werden dann als Joints und Weights in Accessoren geschrieben und als Attribute dem Primitiv zugefügt.
		*/

		//mesh index	vertex index	joints/weights
		std::vector<std::vector<std::vector<unsigned short>>> mesh_influences;
		std::vector<std::vector<std::vector<float>>> mesh_weights;

		for (int i = 0; i < m_model.meshes.size(); i++) {
			std::vector<std::vector<unsigned short>> influences;
			std::vector<std::vector<float>> weights;

			mesh_influences.push_back(influences);
			mesh_weights.push_back(weights);
		}

		int node_offset = m_model.nodes.size();

		std::vector<Eigen::Matrix4f> inverseBindMatrices;

		for (int i = 0; i < m_pCMesh->boneCount(); i++) {
			auto pBone = m_pCMesh->getBone(i);

			inverseBindMatrices.push_back(pBone->OffsetMatrix);

			Node newNode;
			newNode.name = "bone_" + std::to_string(i);

			m_model.nodes.push_back(newNode);

			if (pBone->VertexInfluences.size() == 0) continue;

			int mesh_index = getMeshIndexByCrossForgeVertexIndex(pBone->VertexInfluences[0]);

			for (int j = 0; j < pBone->VertexInfluences.size(); j++) {
				int32_t vertex_index = pBone->VertexInfluences[j];
				float m_weight = pBone->VertexWeights[j];

				for (int k = mesh_influences[mesh_index].size(); k <= vertex_index; k++) {
					std::vector<unsigned short> joints;
					std::vector<float> weights;

					mesh_influences[mesh_index].push_back(joints);
					mesh_weights[mesh_index].push_back(weights);
				}

				bool found = false;

				for (int k = 0; k < mesh_influences[mesh_index][vertex_index].size(); k++) {
					if (mesh_influences[mesh_index][vertex_index][k] == i) {
						found = true;
						break;
					}
				}

				if (!found && m_weight > 0.0) {
					mesh_influences[mesh_index][vertex_index].push_back(i);
					mesh_weights[mesh_index][vertex_index].push_back(m_weight);
				}
			}
		}

		std::vector<bool> has_parent;
		for (int i = 0; i < m_pCMesh->boneCount(); i++) has_parent.push_back(false);

		// set children of nodes
		for (int i = node_offset; i < m_model.nodes.size(); i++) {
			auto pBone = m_pCMesh->getBone(i - node_offset);

			for (int j = 0; j < pBone->Children.size(); j++) {
				m_model.nodes[i].children.push_back(node_offset + pBone->Children[j]->ID);
				has_parent[pBone->Children[j]->ID] = true;
			}
		}

		// sort and write the 4 strongest weights per vertex
		for (int i = 0; i < mesh_influences.size(); i++) {
			for (int j = 0; j < mesh_influences[i].size(); j++) {
				auto joints = &mesh_influences[i][j];
				auto weights = &mesh_weights[i][j];

				if (joints->size() > 0 && weights->size() > 0) {
					// bubble sort
					bool found = true;
					while (found) {
						found = false;
						for (int pos = 0; pos < joints->size() - 1; pos++) {
							if ((*joints)[pos] < (*joints)[pos + 1]) {
								std::swap((*joints)[pos], (*joints)[pos + 1]);
								std::swap((*weights)[pos], (*weights)[pos + 1]);
								found = true;
							}
						}
					}
				}

				for (int k = joints->size(); k > 4; k--) {
					joints->pop_back();
					weights->pop_back();
				}

				while (joints->size() < 4) joints->push_back(0);
				while (weights->size() < 4) weights->push_back(0.0f);

				float sum = 0.0f;

				for (int k = 0; k < weights->size(); k++) {
					sum += (*weights)[k];
				}
				if (sum < 1.0f) (*weights)[0] += 1.0f - sum;
			}

			if (mesh_weights[i].size() == 0 || mesh_influences[i].size() == 0) continue;

			// write primitive attributes
			Primitive* pPrimitive = &m_model.meshes[i].primitives[0];

			int count = m_model.accessors[pPrimitive->attributes["POSITION"]].count;

			while (mesh_weights[i].size() > count) mesh_weights[i].pop_back();
			while (mesh_influences[i].size() > count) mesh_influences[i].pop_back();

			writeAccessorData(0, TINYGLTF_TYPE_VEC4, &mesh_weights[i]);
			int accessor = m_model.accessors.size() - 1;
			pPrimitive->attributes.emplace("WEIGHTS_0", accessor);

			writeAccessorData(0, TINYGLTF_TYPE_VEC4, &mesh_influences[i]);
			accessor = m_model.accessors.size() - 1;
			pPrimitive->attributes.emplace("JOINTS_0", accessor);
		}

		// write inverse bind matrices
		std::vector<std::vector<float>> data;
		fromMat4f(&inverseBindMatrices, &data);

		// Validator complains if last value of mat4 is not 1.
		for (int i = 0; i < data.size(); i++) {
			data[i][15] = 1.0f;
		}

		writeAccessorData(0, TINYGLTF_TYPE_MAT4, &data);
		int accessor = m_model.accessors.size() - 1;

		Skin skin;
		for (int i = node_offset; i < m_model.nodes.size(); i++) {
			skin.joints.push_back(i);
		}
		skin.inverseBindMatrices = accessor;
		skin.skeleton = -1;

		// find skeleton root, will be set to a node with mesh later and this root will be a child of that mesh.
		int skeleton_root = 0;

		for (int i = has_parent.size() - 1; i >= 0; i--) {
			if (!has_parent[i]) {
				skeleton_root = node_offset + i;
				break;
			}
		}

		// find node with mesh 0 and root bone as child and set skin.
		for (int i = 0; i < m_model.nodes.size(); i++) {
			auto pNode = &m_model.nodes[i];

			if (pNode->mesh == 0) {
				pNode->children.push_back(skeleton_root);
				pNode->skin = m_model.skins.size();
				skin.skeleton = i;
				break;
			}
		}

		m_model.skins.push_back(skin);
	}

	void GLTFIO::writeSkeletalAnimations() {
		for (int i = 0; i < m_pCMesh->skeletalAnimationCount(); i++) {
			auto pAnim = m_pCMesh->getSkeletalAnimation(i);

			Animation newGltfAnim;

			newGltfAnim.name = pAnim->Name;

			for (int j = 0; j < pAnim->Keyframes.size(); j++) {
				auto pBoneKf = pAnim->Keyframes[j];

				int target_node = getNodeIndexByName("bone_" + std::to_string(pBoneKf->BoneID));

				if (target_node == -1) continue;

				if (pBoneKf->BoneID == -1 || pBoneKf->Timestamps.size() == 0) {
					continue;
				}

				for (int k = 0; k < pBoneKf->Timestamps.size(); k++) {
					pBoneKf->Timestamps[k] /= pAnim->Speed;
				}

				writeAccessorDataScalar(0, &pBoneKf->Timestamps);
				int indexAccessor = m_model.accessors.size() - 1;

				if (pBoneKf->Positions.size()) {
					std::vector<std::vector<float>> data;

					fromVec3f(&pBoneKf->Positions, &data);

					writeAccessorData(0, TINYGLTF_TYPE_VEC3, &data);
					int positionAccessor = m_model.accessors.size() - 1;

					AnimationSampler sampler;
					sampler.input = indexAccessor;
					sampler.output = positionAccessor;
					sampler.interpolation = "LINEAR";

					newGltfAnim.samplers.push_back(sampler);

					AnimationChannel channel;
					channel.sampler = newGltfAnim.samplers.size() - 1;
					channel.target_node = target_node;
					assert(channel.target_node >= 0);
					channel.target_path = "translation";

					newGltfAnim.channels.push_back(channel);
				}

				if (pBoneKf->Rotations.size()) {
					std::vector<std::vector<float>> data;

					fromQuatf(&pBoneKf->Rotations, &data);

					writeAccessorData(0, TINYGLTF_TYPE_VEC4, &data);
					int rotationAccessor = m_model.accessors.size() - 1;

					AnimationSampler sampler;
					sampler.input = indexAccessor;
					sampler.output = rotationAccessor;
					sampler.interpolation = "LINEAR";

					newGltfAnim.samplers.push_back(sampler);

					AnimationChannel channel;
					channel.sampler = newGltfAnim.samplers.size() - 1;
					channel.target_node = target_node;
					channel.target_path = "rotation";

					newGltfAnim.channels.push_back(channel);
				}

				if (pBoneKf->Scalings.size()) {
					std::vector<std::vector<float>> data;

					fromVec3f(&pBoneKf->Scalings, &data);

					writeAccessorData(0, TINYGLTF_TYPE_VEC3, &data);
					int scaleAccessor = m_model.accessors.size() - 1;

					AnimationSampler sampler;
					sampler.input = indexAccessor;
					sampler.output = scaleAccessor;
					sampler.interpolation = "LINEAR";

					newGltfAnim.samplers.push_back(sampler);

					AnimationChannel channel;
					channel.sampler = newGltfAnim.samplers.size() - 1;
					channel.target_node = target_node;
					channel.target_path = "scale";

					newGltfAnim.channels.push_back(channel);
				}
			}

			if (newGltfAnim.channels.size()) m_model.animations.push_back(newGltfAnim);
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
		Accessor acc = m_model.accessors[accessor];

		if (!acc.sparse.isSparse) {
			std::cout << "Accessor is not sparse" << std::endl;

			return -1;
		}

		int component_count = componentCount(acc.type);

		int index_type = acc.sparse.indices.componentType;
		int index_type_size = sizeOfGltfComponentType(index_type);
		int index_byte_offset = acc.sparse.indices.byteOffset;
		BufferView index_buff_view = m_model.bufferViews[acc.sparse.indices.bufferView];
		Buffer index_buff = m_model.buffers[index_buff_view.buffer];

		int value_type = acc.componentType;
		int value_type_size = sizeOfGltfComponentType(acc.componentType);
		int value_byte_offset = acc.sparse.values.byteOffset;
		BufferView value_buff_view = m_model.bufferViews[acc.sparse.values.bufferView];
		Buffer value_buff = m_model.buffers[value_buff_view.buffer];



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
		Accessor acc = m_model.accessors[accessor];

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
		Accessor acc = m_model.accessors[accessor];

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
		Buffer* pBuffer = &m_model.buffers[buffer_index];

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

			accessor.sparse.indices.bufferView = m_model.bufferViews.size();
			accessor.sparse.indices.componentType = TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT;

			m_model.bufferViews.push_back(index_buffer_view);

			writeBuffer(&pBuffer->data, index_buffer_view.byteOffset + accessor.byteOffset,
				accessor.sparse.count, false, index_buffer_view.byteStride, pIndices);
		}

		BufferView value_buffer_view;

		value_buffer_view.byteOffset = pBuffer->data.size();
		value_buffer_view.buffer = buffer_index;
		value_buffer_view.byteLength = pData->size() * sizeof(float) * componentCount(type);
		value_buffer_view.byteStride = 0;

		accessor.sparse.values.bufferView = m_model.bufferViews.size();

		m_model.bufferViews.push_back(value_buffer_view);

		bool is_matrix = componentIsMatrix(type);


		writeBuffer(&pBuffer->data, value_buffer_view.byteOffset + accessor.byteOffset,
			accessor.sparse.count, is_matrix, value_buffer_view.byteStride, pData);


		m_model.accessors.push_back(accessor);

		return m_model.accessors.size() - 1;
	}

	void GLTFIO::toVec2f(const std::vector<std::vector<float>>* pIn, std::vector<Eigen::Vector2f>* pOut) {
		for (auto e : *pIn) {
			Eigen::Vector2f vec;

			vec(0) = e[0];
			vec(1) = e[1];
			pOut->push_back(vec);
		}
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

	Eigen::Matrix4f GLTFIO::toSingleMat4(const std::vector<double>* pIn) {
		Eigen::Matrix4f mat;

		for (int i = 0; i < pIn->size(); i++) {
			mat(i % 4, i / 4) = (float)(*pIn)[i];
		}

		return mat;
	}

	void GLTFIO::fromVec2f(const std::vector<Eigen::Vector2f>* pIn, std::vector<std::vector<float>>* pOut) {
		for (auto e : *pIn) {
			std::vector<float> toAdd;
			toAdd.push_back(e(0));
			toAdd.push_back(e(1));
			pOut->push_back(toAdd);
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

	void GLTFIO::fromUVtoUVW(const std::vector<Eigen::Vector2f>* pIn, std::vector<Eigen::Vector3f>* pOut) {
		for (auto e : *pIn) {
			Eigen::Vector3f vec;

			vec(0) = e(0);
			vec(1) = e(1);
			vec(2) = 0.0f;
			pOut->push_back(vec);
		}
	}

	void GLTFIO::fromUVWtoUV(const std::vector<Eigen::Vector3f>* pIn, std::vector<Eigen::Vector2f>* pOut) {
		for (auto e : *pIn) {
			Eigen::Vector2f vec;

			vec(0) = e(0);
			vec(1) = e(1);
			pOut->push_back(vec);
		}
	}

	/*
	* Returns the gltf mesh index for a given index of a vertex in the CrossForge Mesh.
	* Only works if the submeshes have already been processed into primitives during writing.
	*/
	int GLTFIO::getMeshIndexByCrossForgeVertexIndex(int index) {
		for (int i = 0; i < m_primitiveIndexRanges.size(); i++) {
			auto minmax = m_primitiveIndexRanges[i];

			if (index >= minmax.first && index <= minmax.second) return i;
		}

		return -1;
	}

	Eigen::Vector3f GLTFIO::getTranslation(const Eigen::Matrix4f& mat) {
		return Eigen::Vector3f(mat(0, 3), mat(1, 3), mat(2, 3));
	}

	Eigen::Quaternionf GLTFIO::getRotation(const Eigen::Matrix4f& mat) {
		auto scale = getScale(mat);

		Eigen::Matrix4f rotation(mat);

		rotation(0, 0) /= scale.x();
		rotation(1, 0) /= scale.x();
		rotation(2, 0) /= scale.x();

		rotation(0, 1) /= scale.y();
		rotation(1, 1) /= scale.y();
		rotation(2, 1) /= scale.y();

		rotation(0, 2) /= scale.z();
		rotation(1, 2) /= scale.z();
		rotation(2, 2) /= scale.z();

		float t;
		Eigen::Vector4f q;

		if (rotation(2, 2) < 0) {
			if (rotation(0, 0) > rotation(1, 1)) {
				t = 1.0 + rotation(0, 0) - rotation(1, 1) - rotation(2, 2);
				q(0) = t;
				q(1) = rotation(0, 1) + rotation(1, 0);
				q(2) = rotation(2, 0) + rotation(0, 2);
				q(3) = rotation(1, 2) + rotation(2, 1);
			}
			else {
				t = 1.0 - rotation(0, 0) + rotation(1, 1) - rotation(2, 2);
				q(0) = rotation(0, 1) + rotation(1, 0);
				q(1) = t;
				q(2) = rotation(1, 2) + rotation(2, 1);
				q(3) = rotation(2, 0) + rotation(0, 2);
			}
		}
		else {
			if (rotation(0, 0) < -rotation(1, 1)) {
				t = 1.0 - rotation(0, 0) - rotation(1, 1) + rotation(2, 2);
				q(0) = rotation(2, 0) + rotation(0, 2);
				q(1) = rotation(1, 2) + rotation(2, 1);
				q(2) = t;
				q(3) = rotation(0, 1) - rotation(1, 0);
			}
			else {
				t = 1.0 + rotation(0, 0) + rotation(1, 1) + rotation(2, 2);
				q(0) = rotation(1, 2) - rotation(2, 1);
				q(1) = rotation(2, 0) - rotation(0, 2);
				q(2) = rotation(0, 1) - rotation(1, 0);
				q(3) = t;
			}
		}

		q *= 0.5 / sqrt(t);

		Eigen::Quaternionf quat(q(3), q(0), q(1), q(2));

		quat.normalize();

		return quat;
	}

	Eigen::Vector3f GLTFIO::getScale(const Eigen::Matrix4f& mat) {
		Eigen::Vector3f sx(mat(0, 0), mat(0, 1), mat(0, 2));
		Eigen::Vector3f sy(mat(1, 0), mat(1, 1), mat(1, 2));
		Eigen::Vector3f sz(mat(2, 0), mat(2, 1), mat(2, 2));

		Eigen::Vector3f scale(sx.norm(), sy.norm(), sz.norm());

		return scale;
	}

	int GLTFIO::getNodeIndexByName(const std::string& name) {
		for (int i = 0; i < m_model.nodes.size(); i++) {
			Node* pNode = &m_model.nodes[i];

			if (pNode->name == name) {
				return i;
			}
		}

		return -1;
	}

#pragma endregion
}//name space

//TODO
/*
* Was passiert mit Skelettanimationen mit unterschiedlichen Keyframes? -> ggf. Umrechnen auf gleiche Keyframes
* Die Berechnung der per face normals schlägt fehl, weil zu große Indices verwendet werden.
* Skelettanimationen speichern.
* Standardanimation der Morphtargets für den Export generieren.
*/