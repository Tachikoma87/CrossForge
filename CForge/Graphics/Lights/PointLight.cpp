#include "PointLight.h"

namespace CForge {

	PointLight::PointLight(void): ILight(LIGHT_POINT, "PointLight") {
		m_Attenuation = Eigen::Vector3f(1.0f, 0.0f, 0.0f);
	}//Constructor

	PointLight::~PointLight(void) {

	}//Destructor


	void PointLight::init(const Eigen::Vector3f Pos, const Eigen::Vector3f Dir, const Eigen::Vector3f Color, float Intensity, Eigen::Vector3f Attenuation) {
		ILight::init(Pos, Dir, Color, Intensity);
		m_Attenuation = Attenuation;
	}//initialize

	void PointLight::attenuation(Eigen::Vector3f Attenuation) {
		m_Attenuation = Attenuation;
	}//attenuation

	Eigen::Vector3f PointLight::attenuation(void)const {
		return m_Attenuation;
	}//attenuation

}//name space