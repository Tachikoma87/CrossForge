#include "LODHandler.h"
#include <CForge/Core/SLogger.h>
#include <tinyxml2.h>
#include "../CForge/AssetIO/SAssetIO.h"
#include "MeshDecimate.h"
//#include "Examples/SceneUtilities.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace CForge {

	LODHandler::LODHandler(void){
		pSLOD = SLOD::instance();
	}//Constructor

	LODHandler::~LODHandler(void) {
		pSLOD->release();
		//clear();
	}//Destructor
	
	std::string LODHandler::assembleFolderPath(const std::string Filepath) {
		std::string base_filename = Filepath.substr(Filepath.find_last_of("/\\") + 1);
		// get rid of filetype
		base_filename = base_filename.substr(0, base_filename.find_first_of("."));
		std::string folderpath = Filepath.substr(0, Filepath.find_last_of("/\\") + 1);

		return folderpath + base_filename + "/";
	}
	
	void LODHandler::loadLODmeshes(LODActor* pActor, const std::string Filepath, const std::vector<float>& decimateAmount, const std::vector<float>& triBBratios) {
		if (Filepath.empty()) throw CForgeExcept("Empty filepath specified");
		
		std::vector<float> decAmount = decimateAmount;
		
		std::string lodFolderpath = assembleFolderPath(Filepath);
		std::string metaFilePath = lodFolderpath + "meta.xml";
		
		// create Folder if non existent
		if (!std::filesystem::is_directory(lodFolderpath)) {
			std::filesystem::create_directory(lodFolderpath);
		}
		
		std::vector<T3DMesh<float>*> LODMeshes;
		LODMeshes.push_back(pActor->getLODMeshes()[0]);
		
		if (pSLOD->skipMeshLoader) {
			generateLODMeshes(&LODMeshes,&decAmount);
			pActor->setLODmeshes(LODMeshes);
			return;
		}
		
		bool forceCreate = pSLOD->forceLODregeneration;
		// create Metafile
		if (!std::filesystem::exists(metaFilePath) || forceCreate) { // metafile does not exist
			generateAndStoreLOD(&LODMeshes, &decAmount, Filepath);
		}
		else { // metafile already exists
			std::vector<float> values;
			std::vector<float> triBBratios; // TODO unused remove
			bool suc = readMetaFile(metaFilePath, &values,&triBBratios);

			bool stagesEqual = std::equal(values.begin(), values.end(), decimateAmount.begin());
			if (!stagesEqual || suc) { // decimation values unequel, regenerate LOD
				generateAndStoreLOD(&LODMeshes, &decAmount, Filepath);
			} else {
				for (uint32_t i = 1; i < values.size(); i++) {
					std::string modelPath = getModelPath(Filepath, i);
					//std::cout << modelPath << "\n";
				
					if (!std::filesystem::exists(modelPath)) { // mesh does not exist, regenerate LOD
						LODMeshes.erase(LODMeshes.begin()+1, LODMeshes.end()); // remove existing models except the original
						generateAndStoreLOD(&LODMeshes, &decAmount, Filepath);
						break;
					}
				
					T3DMesh<float>* M = new T3DMesh<float>();
					SAssetIO::load(modelPath, M);
					//SceneUtilities::setMeshShader(M, pActor->getLODMeshes()[0]->getMaterial(0)->Roughness, pActor->getLODMeshes()[0]->getMaterial(0)->Metallic);
					M->computePerVertexNormals(); // TODO check?
					LODMeshes.push_back(M);
				} // for stages
				pActor->setLODStages(values);
			} // if models can be loaded
		}
		
		pActor->setLODmeshes(LODMeshes); // TODO
	}

	void LODHandler::generateAndStoreLOD(std::vector<T3DMesh<float>*>* LODmeshes, std::vector<float>* stages, std::string Filepath) {
		std::string lodFolderpath = assembleFolderPath(Filepath);
		std::string metaFilePath = lodFolderpath + "meta.xml";
		generateLODMeshes(LODmeshes, stages);
		storeLODMeshes(*LODmeshes, Filepath);
		writeMetaFile(metaFilePath, *stages, std::vector<float>());
	}

	void LODHandler::storeLODMeshes(std::vector<T3DMesh<float>*> meshes, std::string Filepath) {
		for (uint32_t i = 1; i < meshes.size(); i++) {
			std::string path = getModelPath(Filepath, i);
			//std::cout << "storeLODMeshes: " << path << "\n"; //TODO
			SAssetIO::store(path, (meshes[i]));
		}
	}
	
	std::string LODHandler::getModelPath(const std::string Filepath, uint32_t index) {
		std::string lodFolderpath = assembleFolderPath(Filepath);
		std::string base_filename = Filepath.substr(Filepath.find_last_of("/\\") + 1);
		base_filename = base_filename.substr(0, base_filename.find_first_of("."));
		return lodFolderpath + base_filename + std::to_string(index) + ".obj";
	}
	
	// TODO remove?
	void LODHandler::loadLODmeshes(LODActor* pActor, const std::string Filepath) {
		
		std::vector<float> pDecAmount = *pSLOD->getLevels();
		this->loadLODmeshes(pActor, Filepath, pDecAmount, std::vector<float>()); // TODO
	}
	
	void LODHandler::generateLODMeshes(std::vector<T3DMesh<float>*>* LODmeshes, std::vector<float>* stages) {
		// TODO parallelise generation
		for (uint32_t i = 1; i < stages->size(); i++) {
			T3DMesh<float>* pLODMesh = new T3DMesh<float>();
			float amount = float((*stages)[i]) / (*stages)[i-1];
			if (amount > 1.0)
				throw CForgeExcept("decimation stages are in wrong order");

			bool succ = MeshDecimator::decimateMesh((*LODmeshes)[i-1], pLODMesh, amount);
			bool faceless = false;
			for (uint32_t j = 0; j < pLODMesh->submeshCount(); j++) {
				if (pLODMesh->getSubmesh(j)->Faces.size() < 1)
					faceless = true;
			}
			
			if (!succ || pLODMesh->vertexCount() < 3 || faceless) { // decimation failed due to to few triangles to decimate
				// delete unavailable stages
				stages->erase((*stages).begin()+i, stages->end());
				delete pLODMesh;
				//LODmeshes->push_back(nullptr); // TODO ?
				break;
			}
			LODmeshes->push_back(pLODMesh);
		}
	}
	
	
	bool LODHandler::checkLODmodel(const std::string metaFilePath)
	{
		// TODO open  Metafile and check valid object
		if (!std::filesystem::exists(metaFilePath)) {
			return false;
		}
		else {
			std::vector<float> values;
			readMetaFile(metaFilePath, &values, nullptr);
			
			// check if model files for lod exist
			for (uint32_t i = 0; i < values.size(); i++) {
				
				std::filesystem::exists(metaFilePath);
			}
		}
	}

	void LODHandler::writeMetaFile(const std::string metaFilePath, const std::vector<float>& decimateAmount, const std::vector<float>& triBBratios) {
		
		tinyxml2::XMLDocument doc;
		
		tinyxml2::XMLElement* head = doc.NewElement("lod");
		doc.InsertEndChild(head);
		
		tinyxml2::XMLElement* levels = doc.NewElement("levels");
		levels->SetText(decimateAmount.size());
		head->InsertEndChild(levels);
		
		tinyxml2::XMLElement* reductionAmounts = doc.NewElement("reduction");
		head->InsertEndChild(reductionAmounts);
		// append reduction value for each level
		for (uint8_t i = 0; i < decimateAmount.size(); i++) {
			tinyxml2::XMLElement* redAmount = doc.NewElement("level");
			redAmount->SetAttribute("stage", i+1);
			redAmount->SetText(decimateAmount.at(i));
			reductionAmounts->InsertEndChild(redAmount);
		}
		tinyxml2::XMLElement* percAmounts = doc.NewElement("triBBratio");
		head->InsertEndChild(percAmounts);
		// append reduction value for each level
		for (uint8_t i = 0; i < triBBratios.size(); i++) {
			tinyxml2::XMLElement* percAmount = doc.NewElement("level");
			percAmount->SetAttribute("stage", i+1);
			percAmount->SetText(triBBratios.at(i));
			percAmounts->InsertEndChild(percAmount);
		}
		
		doc.SaveFile(metaFilePath.c_str(), false);
	}

	bool LODHandler::readMetaFile(const std::string metaFilePath, std::vector<float>* decAmount, std::vector<float>* triBBratios) {
		
		decAmount->clear();
		triBBratios->clear();
		
		tinyxml2::XMLDocument doc;
		if(doc.LoadFile(metaFilePath.c_str()) != 0)
			return false;
		tinyxml2::XMLElement* lodElem = doc.FirstChildElement("lod");
		tinyxml2::XMLElement* levels = lodElem->FirstChildElement("levels");
		uint32_t stages = atoi(levels->GetText());
		
		tinyxml2::XMLElement* reduction = lodElem->FirstChildElement("reduction");
		tinyxml2::XMLElement* redAmount = reduction->FirstChildElement("level");
		if (!redAmount)
			return false;
		for (uint32_t i = 0; i < stages; i++) {
			decAmount->push_back(atof(redAmount->GetText()));
			redAmount = redAmount->NextSiblingElement();
			if (!redAmount)
				return false;
		}
		
		reduction = lodElem->FirstChildElement("triBBratio");
		if (reduction && triBBratios) {
			redAmount = reduction->FirstChildElement("level");
			for (uint32_t i = 0; i < stages; i++) {
				triBBratios->push_back(atof(redAmount->GetText()));
				redAmount = redAmount->NextSiblingElement();
			}
		}

		return true;
	}

	float LODHandler::getTriSizeInfo(CForge::T3DMesh<float>& pMesh, TriSizeInfo type) {
		
		float ret = 0.0;
		uint32_t triangleAmount = 0;

		for (uint32_t i = 0; i < pMesh.submeshCount(); i++) {
			triangleAmount += (pMesh.getSubmesh(i)->Faces).size();
		}
		
		for (uint32_t i = 0; i < pMesh.submeshCount(); i++) {
			
			auto* triangles = &(pMesh.getSubmesh(i)->Faces);
			
			for (uint32_t j = 0; j < triangles->size(); j++) {
				
				// calculate triangle Size
				Eigen::Vector3f a = pMesh.vertex(triangles->at(j).Vertices[0]);
				Eigen::Vector3f b = pMesh.vertex(triangles->at(j).Vertices[1]);
				Eigen::Vector3f c = pMesh.vertex(triangles->at(j).Vertices[2]);
				
				float triangleSize = (b-a).cross(c-a).norm()*0.5;
				
				switch (type) {
					case TRI_S_MIN: {
						ret = std::min(ret, triangleSize);
						break;
					}
					case TRI_S_MAX: {
						ret = std::max(ret, triangleSize);
						break;
					}
					case TRI_S_AVG: {
						ret += triangleSize / triangleAmount;
						break;
					}
				}
			}
		}
		
		return ret;
	}
}