/*****************************************************************************\
*                                                                           *
* File(s): ShaderSourceComponent.h and ShaderSourceComponent.cpp                       *
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
#ifndef __CROSSFORGE_SHADERSOURCECOMPONENT_H__
#define __CROSSFORGE_SHADERSOURCECOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class ShaderSourceComponent : public ComponentBase {
	public:
		static inline std::string identification = "ShaderSourceComponent";

		ShaderSourceComponent();
		~ShaderSourceComponent();

		void initialize(const std::shared_ptr<const ShaderSourceComponent> pRef = nullptr);
		void clear();

		std::vector<std::string>& vertexShaderSources();
		std::vector<std::string>& fragmentShaderSources();
		std::vector<std::string>& computeShaderSources();

		const std::vector<std::string> getVertexShaderSources()const;
		const std::vector<std::string> getFragmentShaderSources()const;
		const std::vector<std::string> getComputeShaderSources()const;

		void setVertexShaderSources(const std::vector<std::string> sources);
		void setFragmentShaderSources(const std::vector<std::string> sources);
		void setComputeShaderSources(const std::vector<std::string> sources);

	protected:
		ShaderSourceComponent(const std::string childIdentification);

		std::vector<std::string> m_vertexShaderSources;
		std::vector<std::string> m_fragmentShaderSources;
		std::vector<std::string> m_computeShaderSources;
	};

	using ShaderSourceComponentPtr = std::shared_ptr<ShaderSourceComponent>;
	using ShaderSourceComponentCPtr = std::shared_ptr<const ShaderSourceComponent>;
}

#endif