#include "LODHandler.h"
#include "../../CForge/Core/SLogger.h"
#include "../thirdparty/tinyxml2/tinyxml2.h"
#include "../CForge/AssetIO/SAssetIO.h"
#include "../CForge/AssetIO/T3DMesh.hpp"

#include <filesystem>
#include <fstream>

namespace CForge {

	LODHandler::LODHandler(void){
		pSLOD = SLOD::instance();
	}//Constructor

	LODHandler::~LODHandler(void) {
		pSLOD->release();
		//clear();
	}//Destructor
	
	void LODHandler::generateLODmodels(const std::string Filepath) {
		
		std::vector<float>* pDecAmount = pSLOD->getLevels();
		this->generateLODmodels(Filepath, pDecAmount);
	}

	std::string LODHandler::assembleFolderPath(const std::string Filepath) {
		std::string base_filename = Filepath.substr(Filepath.find_last_of("/\\") + 1);
		// get rid of filetype
		base_filename = base_filename.substr(0, base_filename.find_first_of("."));
		std::string folderpath = Filepath.substr(0, Filepath.find_last_of("/\\") + 1);

		return folderpath + base_filename + "/";
	}
	
	void LODHandler::generateLODmodels(const std::string Filepath, std::vector<float>* decimateAmount) {
		if (Filepath.empty()) throw CForgeExcept("Empty filepath specified");

		std::vector<float>* pDecAmount = decimateAmount;
		// use predefined stages
		if (decimateAmount == nullptr) {
			pDecAmount = pSLOD->getLevels();
		}
		
		std::string lodFolderpath = assembleFolderPath(Filepath);
		std::string metaFilePath = lodFolderpath + "meta.xml";
		
		// create Folder if non existent
		if (!std::filesystem::is_directory(lodFolderpath)) {
			std::filesystem::create_directory(lodFolderpath);
		}
		
		// create Metafile
		if (!std::filesystem::exists(metaFilePath)) {
			writeMetaFile(metaFilePath, pDecAmount);
		}
		else {
			std::vector<float> values = readMetaFile(metaFilePath);
			
			std::cout << "\nread values are:\n";
			for (float x : values)
				std::cout << x << "\n";
		}
	}
	
	bool LODHandler::checkLODmodel(const std::string metaFilePath)
	{
		// TODO open  Metafile and check valid object
		if (!std::filesystem::exists(metaFilePath)) {
			return false;
		}
		else {
			std::vector<float> values = readMetaFile(metaFilePath);
			if (pSLOD->forceSLOD) {
				if (values != *pSLOD->getLevels())
					return false;
			}
			
			// check if model files for lod exist
			for (uint32_t i = 0; i < values.size(); i++) {
				
			}
		}
	}

	void LODHandler::writeMetaFile(const std::string metaFilePath, std::vector<float>* decimateAmount) {
		
		tinyxml2::XMLDocument doc;
		
		tinyxml2::XMLElement* head = doc.NewElement("lod");
		doc.InsertEndChild(head);
		
		tinyxml2::XMLElement* levels = doc.NewElement("levels");
		levels->SetText(decimateAmount->size());
		head->InsertEndChild(levels);
		
		tinyxml2::XMLElement* reductionAmounts = doc.NewElement("reduction");
		head->InsertEndChild(reductionAmounts);
		// append reduction value for each level
		for (uint8_t i = 0; i < decimateAmount->size(); i++) {
			tinyxml2::XMLElement* redAmount = doc.NewElement("level");
			redAmount->SetAttribute("stage", i+1);
			redAmount->SetText(decimateAmount->at(i));
			reductionAmounts->InsertEndChild(redAmount);
		}
		
		doc.SaveFile(metaFilePath.c_str(), false);
	}

	std::vector<float> LODHandler::readMetaFile(const std::string metaFilePath) {
		tinyxml2::XMLDocument doc;
		std::cout << doc.LoadFile(metaFilePath.c_str());
		tinyxml2::XMLElement* lodElem = doc.FirstChildElement("lod");
		tinyxml2::XMLElement* levels = lodElem->FirstChildElement("levels");
		uint32_t stages = atoi(levels->GetText());
		
		tinyxml2::XMLElement* reduction = lodElem->FirstChildElement("reduction");
		tinyxml2::XMLElement* redAmount = reduction->FirstChildElement("level");
		std::vector<float> ret;
		for (uint32_t i = 0; i < stages; i++) {
			ret.push_back(atof(redAmount->GetText()));
			redAmount = redAmount->NextSiblingElement();
		}
		
		return ret;
	}
}