/*****************************************************************************\
*                                                                           *
* File(s): ExampleBase.h and ExampleBase.cpp                       *
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
#ifndef __CROSSFORGE_EXAMPLEBASE_H__
#define __CROSSFORGE_EXAMPLEBASE_H__

#include <crossforge/application/ApplicationBase.h>
#include <crossforge/graphics/entities/WindowEntity.h>
#include <crossforge/graphics/systems/WindowSystem.h>


namespace crossforge {
	class ExampleBase : public ApplicationBase {
	public:

		virtual void initialize();
		virtual void update();


		virtual ~ExampleBase();
	protected:
		ExampleBase(const std::string exampleName);

		std::string m_exampleName;

		
	};

	using ExampleBasePtr = std::shared_ptr<ExampleBase>;
	using ExampleBaseCPtr = std::shared_ptr<const ExampleBase>;
}

#endif 
