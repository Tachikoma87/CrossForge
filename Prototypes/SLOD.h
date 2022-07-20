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

		bool forceHighLOD = false;
		float LODOffset = 0.25;
		
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
		//bool forceSLOD = false; //TODO
		//bool m_preferLowLOD = false;
		
		std::vector<float> LODLevels = /*{1.0, 0.9}; //*/{1.0, 0.75, 0.5, 0.25, 0.125, 0.0625, 0.031, 0.0155};
		//std::vector<float> LODLevels = {1.0};
		//std::vector<float> LODPercentages = {FLT_EPSILON};
		//std::vector<float> LODPercentages = { 0.5, 0.4, 0.25, 0.125, 0.0625, 0.03125 };
		std::vector<float> LODPercentages = {1.0*0.5, 0.75*0.5, 0.5*0.5, 0.25*0.5, 0.125*0.5, 0.0625*0.5};
		//std::vector<float> LODPercentages = {0.5, 0.25, 0.1, 0.05, 0.01, 0.001};
		//std::vector<float> LODPercentages = {0.1, 0.01, 0.001, 0.0001, 0.00001, 0.000001};
		std::vector<float> LODDistances;
		
		static SLOD* m_pInstance;
		static uint32_t m_InstanceCount;
	};
}
