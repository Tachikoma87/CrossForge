#pragma once
#include "../thirdparty/tinyxml2/tinyxml2.h"
#include "../CForge/AssetIO/File.h"
#include <chrono>
#include "../CForge/Core/ITListener.hpp"
#include "../CForge/Graphics/VirtualCamera.h"

namespace CForge {

	class SLOD{
	public:
		static SLOD* instance(void);
		
		void release(void);
		std::vector<float>* getLevels();
		void update();
		void setResolution(Eigen::Vector2i resolution);
		double getDeltaTime();
		
		uint32_t getResPixAmount();
		std::vector<float> getLODPercentages();
		
		void setLODLevels(std::vector<float> levels);
		
		// Size of which Triangles should appear on Screen in pixels
		uint32_t TriangleSize = 100;
		
		bool storeLOD = true;
		bool skipMeshLoader = false;
		bool forceLODregeneration = false;
		
	protected:
		SLOD(void);
		~SLOD(void);

		void init();
		void clear(void);

	private:
		double deltaTime = 1.0 / 60.0;
		Eigen::Vector2i resolution;
		uint32_t res_pixAmount;
		std::chrono::steady_clock::time_point lastTickCount;
		
		// prefers to render low level LOD instead of high ones
		//bool m_preferLowLOD = false;
		
		std::vector<float> LODLevels = /*{1.0, 0.9}; //*/{1.0, 0.75, 0.5, 0.25, 0.125, 0.125/2, 0.125/2/2, 0.125/2/2/2};
		std::vector<float> LODPercentages = {1.0*0.5, 0.75*0.5, 0.5*0.5, 0.25*0.5, 0.125*0.5, 0.125/2*0.5, 0.125/2/2*0.5, 0.125/2/2/2*0.5};
		std::vector<float> LODDistances;
		
		static SLOD* m_pInstance;
		static uint32_t m_InstanceCount;
	};
}
