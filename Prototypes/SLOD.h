#pragma once
#include "../thirdparty/tinyxml2/tinyxml2.h"
#include "../CForge/AssetIO/File.h"
#include <chrono>

namespace CForge {

	class SLOD{
	public:
		static SLOD* instance(void);
		
		void release(void);
		std::vector<float>* getLevels();
		void update();
		void setResolution();
		double getDeltaTime();
		
	protected:
		SLOD(void);
		~SLOD(void);

		void init(void);
		void clear(void);

	private:
		double deltaTime;
		Eigen::Vector2i resolution;
		std::chrono::steady_clock::time_point lastTickCount;
		
		// prefers to render low level LOD instead of high ones
		bool forceSLOD = false;
		bool m_preferLowLOD = false;
		
		std::vector<float> LODLevels = /*{1.0, 0.9}; //*/{1.0, 0.75, 0.5, 0.25, 0.125, 0.0625 };
		std::vector<float> LODDistances;
		
		static SLOD* m_pInstance;
		static uint32_t m_InstanceCount;
	};
	
	//typedef SLOD SLOD;
}
