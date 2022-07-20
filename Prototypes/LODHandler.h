#pragma once

#include "SLOD.h"
#include "../CForge/AssetIO/T3DMesh.hpp"

namespace CForge {
	
	/*
	* loads and stores LOD models with meta files
	*/
	class LODHandler{
		
		public:
			
			enum TriSizeInfo {
				TRI_S_MIN = 0,
				TRI_S_MAX,
				TRI_S_AVG
			};
			
			static float getTriSizeInfo(CForge::T3DMesh<float>& pMesh, TriSizeInfo type);

			LODHandler(void);
			~LODHandler(void);
			void generateLODmodels(const std::string Filepath);
			void generateLODmodels(const std::string Filepath, const std::vector<float>& decimateAmount, const std::vector<float>& triBBratios);
			void writeMetaFile(const std::string metaFilePath, const std::vector<float>& decimateAmount, const std::vector<float>& triBBratios);
			bool readMetaFile(const std::string metaFilePath, std::vector<float>* decAmount, std::vector<float>* triBBratios);
		
			void waitForLODgeneration();
		
		private:
			std::string assembleFolderPath(const std::string Filepath);
			bool checkLODmodel(const std::string metaFilePath);
			SLOD* pSLOD;
	};
}

