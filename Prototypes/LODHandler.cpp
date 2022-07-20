#include "LODHandler.h"
#include "../../CForge/Core/SLogger.h"
#include "../thirdparty/tinyxml2/tinyxml2.h"
#include "../CForge/AssetIO/SAssetIO.h"

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
	
	void LODHandler::generateLODmodels(const std::string Filepath) {
		
		std::vector<float> pDecAmount = *pSLOD->getLevels();
		this->generateLODmodels(Filepath, pDecAmount, std::vector<float>()); // TODO
	}

	std::string LODHandler::assembleFolderPath(const std::string Filepath) {
		std::string base_filename = Filepath.substr(Filepath.find_last_of("/\\") + 1);
		// get rid of filetype
		base_filename = base_filename.substr(0, base_filename.find_first_of("."));
		std::string folderpath = Filepath.substr(0, Filepath.find_last_of("/\\") + 1);

		return folderpath + base_filename + "/";
	}
	
	void LODHandler::generateLODmodels(const std::string Filepath, const std::vector<float>& decimateAmount, const std::vector<float>& triBBratios) {
		if (Filepath.empty()) throw CForgeExcept("Empty filepath specified");

		std::vector<float> pDecAmount = decimateAmount;
		
		std::string lodFolderpath = assembleFolderPath(Filepath);
		std::string metaFilePath = lodFolderpath + "meta.xml";
		
		// create Folder if non existent
		if (!std::filesystem::is_directory(lodFolderpath)) {
			std::filesystem::create_directory(lodFolderpath);
		}
		
		bool forceCreate = false;
		
		// create Metafile
		if (!std::filesystem::exists(metaFilePath) || forceCreate) { // metafile does not exist
			writeMetaFile(metaFilePath, pDecAmount, triBBratios);
		}
		else { // metafile already exists
			std::vector<float> values;
			std::vector<float> triBBratios;
			bool suc = readMetaFile(metaFilePath, &values,&triBBratios);
			
			for (uint32_t i = 0; i < values.size(); i++) {
				std::string base_filename = Filepath.substr(Filepath.find_last_of("/\\") + 1);
				base_filename = base_filename.substr(0, base_filename.find_first_of("."));
				std::string modelPath = lodFolderpath + base_filename + std::to_string(i) + ".obj";
				std::cout << modelPath << "\n";
				//SAssetIO::load()
			}
			
			
			std::cout << "\nread values are:\n";
			for (float x : values)
				std::cout << x << "\n";
		}
	}
	
	bool LODHandler::checkLODmodel(const std::string metaFilePath) // TODO remove?
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
		for (uint8_t i = 0; i < decimateAmount.size(); i++) {
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
		std::cout << doc.LoadFile(metaFilePath.c_str());
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

		return false;
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

	void LODHandler::waitForLODgeneration() {

	}
}