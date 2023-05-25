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

#include "Prototypes/SkeletonConvertion.hpp"

#include <iostream>
#include <algorithm>
#include <fstream>

using namespace tinygltf;

namespace CForge {

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

				if (min == -1 || index < min)
					min = index;

				if (max == -1 || index > max)
					max = index;

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
			Eigen::Vector3f pos = m_pCMesh->vertex(current_index);

			m_coord[current_index - min] = pos;

			if (m_pCMesh->normalCount() > 0) {
				// fill up to index
				for (int k = m_normal.size(); k <= current_index - min; k++) {
					Eigen::Vector3f vec3(0, 0, 0);
					m_normal.push_back(vec3);
				}
				Eigen::Vector3f norm = m_pCMesh->normal(current_index).normalized();
				m_normal[current_index - min] = norm;
			}

			if (m_pCMesh->tangentCount() > 0) {
				// fill up to index
				for (int k = m_tangent.size(); k <= current_index - min; k++) {
					Eigen::Vector3f vec3(0, 0, 0);
					m_tangent.push_back(vec3);
				}
				Eigen::Vector3f tan = m_pCMesh->tangent(current_index);
				m_tangent[current_index - min] = tan;
			}

			if (m_pCMesh->textureCoordinatesCount() > 0) {
				// fill up to index
				for (int k = m_texCoord.size(); k <= current_index - min; k++) {
					Eigen::Vector2f vec2(0, 0);
					m_texCoord.push_back(vec2);
				}

				if (current_index >= m_pCMesh->textureCoordinatesCount()) continue;

				Eigen::Vector3f tex = m_pCMesh->textureCoordinate(current_index);
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
				Eigen::Vector3f meshCol = m_pCMesh->color(current_index);
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

		//TODO this is dirty
		T3DMesh<float>::Bone* p_cRoot = const_cast<T3DMesh<float>::Bone*>(m_pCMesh->rootBone());
		
		SkeletonConverter sc;
		sc.OMtoRH(p_cRoot);

		std::vector<T3DMesh<float>::Bone*> RHskeleton = sc.getSkeleton();

		for (int i = 0; i < m_pCMesh->boneCount(); i++) {
			auto pBone = m_pCMesh->getBone(i);

			inverseBindMatrices.push_back(pBone->OffsetMatrix);

			Node newNode;

			//TODO there seems to be a bug where to root node gets rotated randomly sometimes.
			// Use OffsetMatrix to calculate Bone position as a Node.
			std::vector<double> offsetMat;
			Eigen::Matrix4f eigMat = RHskeleton[i]->OffsetMatrix;

			// convert Eigen::Matrix4f to std::vector<double>
			for (uint32_t i=0;i<16;++i)
				offsetMat.push_back(eigMat.data()[i]);//4*i%16+i/4]);
			newNode.matrix = offsetMat;

			if (pBone->Name.compare("") == 0) // Bone has no name, take ID instead.
				newNode.name = "bone_" + std::to_string(i);
			else
				newNode.name = pBone->Name;

			m_model.nodes.push_back(newNode);

			if (pBone->VertexInfluences.size() == 0) continue;

			std::vector<int> meshesContainInfl = getMeshIndexByCrossForgeVertexIndex(pBone->VertexInfluences[0]);

			for (int j = 0; j < pBone->VertexInfluences.size(); j++) {
				int32_t vertex_index = pBone->VertexInfluences[j];
				float weight = pBone->VertexWeights[j];

				for (uint32_t l=0;l<meshesContainInfl.size();++l) {
					int mesh_index = meshesContainInfl[l];
					
					// Push back new slots.
					for (int k = mesh_influences[mesh_index].size(); k <= vertex_index; k++) {
						mesh_influences[mesh_index].push_back(std::vector<unsigned short>());
						mesh_weights[mesh_index].push_back(std::vector<float>());
					}

					bool found = false;

					for (int k = 0; k < mesh_influences[mesh_index][vertex_index].size(); k++) {
						if (mesh_influences[mesh_index][vertex_index][k] == i) {
							// Weight of vertex already exists.
							
							// Update Weight if current one is larger.
							float oldWeight = mesh_weights[mesh_index][vertex_index][k];
							if (oldWeight < weight) {
								mesh_influences[mesh_index][vertex_index][k] = i;
								mesh_weights[mesh_index][vertex_index][k] = weight;
							}
							
							found = true; // vertex already added.
							//break;
						}
					}

					// Weight of Vertex is new.
					if (!found && weight > 0.0) {
						mesh_influences[mesh_index][vertex_index].push_back(i);
						mesh_weights[mesh_index][vertex_index].push_back(weight);
					}
				}
			}
		}
		std::cout << "stop" << std::endl;
		//TODO might be a unwanted sideeffect? / Move into extra function?
		// Normalize weights.
		for (uint32_t i = 0; i < mesh_weights.size(); ++i) {
			for (uint32_t j = 0; j < mesh_weights[i].size(); ++j) {
				double total = 0.0;
				for (uint32_t k = 0; k < mesh_weights[i][j].size(); ++k) {
					total += (double) mesh_weights[i][j][k];
				}
				for (uint32_t k = 0; k < mesh_weights[i][j].size(); ++k) {
					mesh_weights[i][j][k] /= total;
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
							if ((*joints)[pos] > (*joints)[pos + 1]) {
								std::swap((*joints)[pos], (*joints)[pos + 1]);
								std::swap((*weights)[pos], (*weights)[pos + 1]);
								found = true;
							}
						}
					}
				}

				// Trim size to 4 as gltf only supports 4 weights per vertex.
				for (int k = joints->size(); k > 4; k--) {
					joints->pop_back();
					weights->pop_back();
				}
				while (joints->size() < 4) joints->push_back(0);
				while (weights->size() < 4) weights->push_back(0.0f);

				float sum = 0.0f;

				// normalize weights
				for (int k = 0; k < weights->size(); k++) {
					sum += (*weights)[k];
				}
				for (uint32_t k=0;k<weights->size();++k)
					(*weights)[k] /= sum;
				
				//if (sum < 1.0f) (*weights)[0] += 1.0f - sum;
			}

			if (mesh_weights[i].size() == 0 || mesh_influences[i].size() == 0)
				continue;

			// write primitive attributes
			Primitive* pPrimitive = &m_model.meshes[i].primitives[0];

			int count = m_model.accessors[pPrimitive->attributes["POSITION"]].count;

			while (mesh_weights[i].size() > count)
				mesh_weights[i].pop_back();
			
			while (mesh_influences[i].size() > count)
				mesh_influences[i].pop_back();

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

				int target_node = 0;
				std::string boneName;
				if (pAnim->Keyframes[j]->BoneName.compare("") == 0)
					boneName = "bone_" + std::to_string(pBoneKf->BoneID);
				else
					boneName = pAnim->Keyframes[j]->BoneName;
				target_node = getNodeIndexByName(boneName);

				if (target_node == -1) continue;

				if (pBoneKf->BoneID == -1 || pBoneKf->Timestamps.size() == 0) {
					continue;
				}

				std::vector<float> Timestamps;
				for (int k = 0; k < pBoneKf->Timestamps.size(); k++) {
					if (pAnim->Speed != 0.0)
						Timestamps.push_back(pBoneKf->Timestamps[k] / pAnim->Speed);
					else
						Timestamps.push_back(pBoneKf->Timestamps[k]);
				}

				writeAccessorDataScalar(0, &Timestamps);
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

	/*
	* Returns the gltf mesh index for a given index of a vertex in the CrossForge Mesh.
	* Only works if the submeshes have already been processed into primitives during writing.
	*/
	std::vector<int> GLTFIO::getMeshIndexByCrossForgeVertexIndex(int index) {
		std::vector<int> ret;
		for (int i = 0; i < m_primitiveIndexRanges.size(); i++) {
			auto minmax = m_primitiveIndexRanges[i];

			if (index >= minmax.first && index <= minmax.second) // return i;
				ret.push_back(i);
		}

		return ret;
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

}//CForge
