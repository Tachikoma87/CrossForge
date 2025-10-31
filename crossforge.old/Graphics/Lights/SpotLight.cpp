#include "SpotLight.h"

using namespace Eigen;

namespace CForge {

	SpotLight::SpotLight(void): ILight(LIGHT_SPOT, "SpotLight") {

	}//Constructor

	SpotLight::~SpotLight(void) {
		m_Attenuation = Vector3f(0.0f, 0.0f, 0.0f);
	}//Destructor

	void SpotLight::init(Eigen::Vector3f Position, Eigen::Vector3f Direction, Eigen::Vector3f Color, float Intensity, Eigen::Vector3f Attenuation, Eigen::Vector2f CutOff) {
		ILight::init(Position, Direction, Color, Intensity);
		m_Attenuation = Attenuation;
		m_CutOff = CutOff;
	}//initialize

	void SpotLight::attenuation(Eigen::Vector3f Attenuation) {
		m_Attenuation = Attenuation;
	}//attenuation

	void SpotLight::cutOff(Eigen::Vector2f CutOff) {
		m_CutOff = CutOff;
	}//CutOff

	Eigen::Vector3f SpotLight::attenuation(void)const {
		return m_Attenuation;
	}//attenuation

	Eigen::Vector2f SpotLight::cutOff(void)const {
		return m_CutOff;
	}//CutOff

}//name space