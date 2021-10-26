#include "SGNTransformation.h"

using namespace Eigen;

namespace CForge {

	SGNTransformation::SGNTransformation(void): ISceneGraphNode("SGNTransformation") {
		m_Translation = Vector3f::Zero();
		m_Rotation = Quaternionf::Identity();
		m_Scale = Vector3f::Ones();
		m_TranslationDelta = Vector3f::Zero();
		m_RotationDelta = Quaternionf::Identity();
		m_ScaleDelta = Vector3f::Zero();
	}//Constructor

	SGNTransformation::~SGNTransformation(void) {
		clear();
	}//Destructor

	void SGNTransformation::init(ISceneGraphNode *pParent, Eigen::Vector3f Translation, Eigen::Quaternionf Rotation, Eigen::Vector3f Scale, Eigen::Vector3f TranslationDelta, Eigen::Quaternionf RotationDelta, Eigen::Vector3f ScaleDelta) {
		clear();
		ISceneGraphNode::init(pParent);
		translation(Translation);
		rotation(Rotation);
		scale(Scale);
		translationDelta(TranslationDelta);
		rotationDelta(RotationDelta);
		scaleDelta(ScaleDelta);
	}//initialize

	void SGNTransformation::clear(void) {
		ISceneGraphNode::clear();
		m_Translation = Vector3f::Zero();
		m_Rotation = Quaternionf::Identity();
		m_Scale = Vector3f::Ones();
		m_TranslationDelta = Vector3f::Zero();
		m_RotationDelta = Quaternionf::Identity();
		m_ScaleDelta = Vector3f::Ones();
	}//clear

	void SGNTransformation::translation(Eigen::Vector3f Translation) {
		m_Translation = Translation;
	}//translation

	void SGNTransformation::rotation(Eigen::Quaternionf Rotation) {
		m_Rotation = Rotation;
	}//rotation

	void SGNTransformation::scale(Eigen::Vector3f Scale) {
		m_Scale = Scale;
	}//scale

	void SGNTransformation::translationDelta(Eigen::Vector3f TranslationDelta) {
		m_TranslationDelta = TranslationDelta;
	}//translationDelta

	void SGNTransformation::rotationDelta(Eigen::Quaternionf RotationDelta) {
		m_RotationDelta = RotationDelta;
	}//rotationDelta

	void SGNTransformation::scaleDelta(Eigen::Vector3f ScaleDelta) {
		m_ScaleDelta = ScaleDelta;
	}//scaleDelta

	Eigen::Vector3f SGNTransformation::translation(void)const {
		return m_Translation;
	}//translation

	Eigen::Quaternionf SGNTransformation::rotation(void)const {
		return m_Rotation;
	}//rotation

	Eigen::Vector3f SGNTransformation::scale(void)const {
		return m_Scale;
	}//scale

	Eigen::Vector3f SGNTransformation::translationDelta(void)const {
		return m_TranslationDelta;
	}//translationDelta

	Eigen::Quaternionf SGNTransformation::rotationDelta(void)const {
		return m_RotationDelta;
	}//rotationDelta

	Eigen::Vector3f SGNTransformation::scaleDelta(void)const {
		return m_ScaleDelta;
	}//scaleDelta

	void SGNTransformation::update(float FPSScale) {
		if (!m_UpdateEnabled) return;

		m_Translation += FPSScale * m_TranslationDelta;
		float Temp = FPSScale;
		Quaternionf TargetRot = m_Rotation;
		while (Temp > 1.0f) {
			TargetRot = m_RotationDelta * TargetRot;
			Temp -= 1.0f;
		}
		m_Rotation = TargetRot.slerp(Temp, m_RotationDelta * TargetRot);
		m_Scale += FPSScale * m_ScaleDelta;

		for (auto i : m_Children) i->update(FPSScale);

	}//update

	void SGNTransformation::render(RenderDevice* pRDev, Eigen::Vector3f Translation, Eigen::Quaternionf Rotation, Eigen::Vector3f Scale) {
		if (m_RenderingEnabled) {
			Eigen::Vector3f T = Translation + Rotation * m_Translation;
			Eigen::Quaternionf R = Rotation * m_Rotation;
			Eigen::Vector3f S = Scale.cwiseProduct(m_Scale);

			for (auto i : m_Children) i->render(pRDev, T, R, S);
		}
	}//render

}//name space