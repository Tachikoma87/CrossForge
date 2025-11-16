/*****************************************************************************\
*                                                                           *
* File(s): ColorComponent.h and ColorComponent.cpp                       *
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
#ifndef __CROSSFORGE_COLORCOMPONENT_H__
#define __CROSSFORGE_COLORCOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class ColorComponent : public ComponentBase {
	public:
		static inline std::string identification = "ColorComponent";

		ColorComponent();
		~ColorComponent();

		void initialize();
		void clear()override;

		Eigen::Vector4f& color();

	protected:
		ColorComponent(const std::string childIdentification);

		Eigen::Vector4f m_color;
	};

	typedef std::shared_ptr<ColorComponent> ColorComponentPtr;
}

#endif 