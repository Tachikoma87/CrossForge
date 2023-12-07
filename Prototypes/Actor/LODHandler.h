#pragma once

#include "SLOD.h"
#include <crossforge/AssetIO/T3DMesh.hpp>
#include "LODActor.h"

namespace CForge {
	
	/**
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
			
			/**
			* loads lod modells from hdd, or generates them if not available or invalid
			* @param pActor - LODActor containing mesh
			*/
			void loadLODmeshes(LODActor* pActor, const std::string Filepath, const std::vector<float>& decimateAmount, const std::vector<float>& triBBratios);
			void loadLODmeshes(LODActor* pActor, const std::string Filepath);
			
			/**
			* @param in/out LODmeshes original mesh on index 0
			* @param in/out stages - input degree of decimation, stages where decimation failed get removed
			*/
			void generateLODMeshes(std::vector<T3DMesh<float>*>* LODmeshes, std::vector<float>* stages);

			void writeMetaFile(const std::string metaFilePath, const std::vector<float>& decimateAmount, const std::vector<float>& triBBratios);
			bool readMetaFile(const std::string metaFilePath, std::vector<float>* decAmount, std::vector<float>* triBBratios);
			
		private:
			void generateAndStoreLOD(std::vector<T3DMesh<float>*>* LODmeshes, std::vector<float>* stages, std::string Filepath);
			void storeLODMeshes(std::vector<T3DMesh<float>*> meshes, std::string Filepath);
			std::string getModelPath(std::string Filepath, uint32_t index);
			std::string assembleFolderPath(const std::string Filepath);
			bool checkLODmodel(const std::string metaFilePath);
			SLOD* pSLOD;
	};
}

