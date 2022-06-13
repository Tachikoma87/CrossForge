#pragma once

#include "SLOD.h"

namespace CForge {
	class LODHandler{
		
		public:
			LODHandler(void);
			~LODHandler(void);
			void generateLODmodels(const std::string Filepath);
			void generateLODmodels(const std::string Filepath, std::vector<float>* decimateAmount);
			void writeMetaFile(const std::string metaFilePath, std::vector<float>* decimateAmount);
			std::vector<float> readMetaFile(const std::string metaFilePath);
		
		private:
			std::string assembleFolderPath(const std::string Filepath);
			bool checkLODmodel(const std::string metaFilePath);
			SLOD* pSLOD;
	};
}

