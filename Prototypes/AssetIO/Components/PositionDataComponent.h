/*****************************************************************************\
*                                                                           *
* File(s): PositionDataComponent.h and PositionDataComponent.cpp            *
*                                                                           *
* Content:                            *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tachikoma87                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CROSSFORGE_POSITIONDATACOMPONENT_H__
#define __CROSSFORGE_POSITIONDATACOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>


namespace crossforge {
	class PositionDataComponent: public ComponentBase {
	public:
		inline static std::string identification = "PositionDataComponent";

		PositionDataComponent();
		~PositionDataComponent();

		void initialize(std::vector<Eigen::Vector3f> positions);
		void clear();

		Eigen::Vector3f operator[](const uint32_t index)const;
		Eigen::Vector3f& operator[](const uint32_t index);

		void setPositions(std::vector<Eigen::Vector3f> positions);
		void setPosition(Eigen::Vector3f position, uint32_t index);
		std::vector<Eigen::Vector3f> getPositions()const;
		std::vector<Eigen::Vector3f>& getPositions();
		Eigen::Vector3f getPosition(uint32_t index)const;
		Eigen::Vector3f& getPosition(uint32_t index);

		uint32_t getPositionCount()const;

	protected:
		std::vector<Eigen::Vector3f> m_positions;
	};

	typedef std::shared_ptr<PositionDataComponent> PositionDataComponentPtr;
}


#endif