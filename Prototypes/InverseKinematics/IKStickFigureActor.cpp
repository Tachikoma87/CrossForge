#include "IKStickFigureActor.h"
#include <crossforge/AssetIO/SAssetIO.h>
#include <crossforge/Math/CForgeMath.h>
#include <crossforge/Utility/CForgeUtility.h>
#include <crossforge/MeshProcessing/PrimitiveShapeFactory.h>

using namespace Eigen;

namespace CForge {

	IKStickFigureActor::IKStickFigureActor(void) {
		m_ClassName = "IKStickFigureActor";
		m_JointSize = 0.1f;
		m_BoneSize = 0.2f;
		m_pAnimationController = nullptr;
	}//Constructor

	IKStickFigureActor::~IKStickFigureActor(void) {
		clear();
	}//Destructor

	void IKStickFigureActor::init(T3DMesh<float>* pMesh, InverseKinematicsController* pController) {
		clear();

		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		if (nullptr == pController) throw NullpointerExcept("pController");
		if (pMesh->boneCount() == 0) throw CForgeExcept("Mesh contains no skeleton. Creation of IKStickFigureActor not possible!");

		m_pAnimationController = pController;

		// initialize the actors
		T3DMesh<float> M;
		SAssetIO::load("MyAssets/JointMarker.gltf", &M);
		for (uint32_t i = 0; i < M.materialCount(); ++i) {
			T3DMesh<float>::Material* pMat = M.getMaterial(i);

			pMat->VertexShaderGeometryPass.push_back("Shader/BasicGeometryPass.vert");
			pMat->FragmentShaderGeometryPass.push_back("Shader/BasicGeometryPass.frag");

			pMat->VertexShaderShadowPass.push_back("Shader/ShadowPassShader.vert");
			pMat->FragmentShaderShadowPass.push_back("Shader/ShadowPassShader.frag");

			pMat->VertexShaderForwardPass.push_back("Shader/ForwardPassPBS.vert");
			pMat->FragmentShaderForwardPass.push_back("Shader/ForwardPassPBS.frag");

			pMat->Metallic = 0.04f;
			pMat->Roughness = 0.7f;
		}//for[materials]
		/*PrimitiveShapeFactory::uvSphere(&M, Vector3f(1.0f, 1.0f, 1.0f), 8, 8);
		for (uint32_t i = 0; i < M.materialCount(); ++i) {
			auto* pMat = M.getMaterial(i);
			buildMaterial(pMat);
			CForgeUtility::defaultMaterial(pMat, CForgeUtility::METAL_COPPER);
		}*/
		M.computePerVertexNormals();
		M.computeAxisAlignedBoundingBox();
		m_Joint.init(&M);
		M.clear();

		PrimitiveShapeFactory::cylinder(&M, Vector2f(1.0f, 1.0f), Vector2f(1.0f, 1.0f), 1.0f, 8, Vector2f(0.0f, 0.0f));
		for (uint32_t i = 0; i < M.materialCount(); ++i) {
			auto* pMat = M.getMaterial(i);
			buildMaterial(pMat);
			CForgeUtility::defaultMaterial(pMat, CForgeUtility::METAL_STAINLESS_STEEL);
		}
		M.computePerVertexNormals();
		M.computeAxisAlignedBoundingBox();
		m_Bone.init(&M);
		M.clear();

		// initialize scene graph
		m_RootSGN.init(nullptr);
		m_SG.init(&m_RootSGN);

		m_JointValues = m_pAnimationController->retrieveSkeleton();
		for (uint32_t i = 0; i < m_JointValues.size(); ++i) {
			SGNTransformation* pTransform = new SGNTransformation();
			SGNGeometry* pGeomJoint = new SGNGeometry();
			SGNGeometry* pGeomBone = new SGNGeometry();

			m_JointTransformSGNs.push_back(pTransform);
			m_JointSGNs.push_back(pGeomJoint);
			m_BoneSGNs.push_back(pGeomBone);
		}//for[all bones]

		// estimate a good value for the joint and bone sizes
		Vector3f Diag = (pMesh->aabb().diagonal().norm() < 0.0001f) ? T3DMesh<float>::computeAxisAlignedBoundingBox(pMesh).diagonal() : pMesh->aabb().diagonal();

		m_JointSize = Diag.norm() / 30.0f;				//m_JointSize = Diag.norm() / 75.0f;
		m_BoneSize = (Diag.norm() / 75.0f) / 3.0f;		//m_BoneSize = m_JointSize / 3.0f;

		// create Scene graph nodes for all joints
		createBone(pMesh->rootBone(), &m_RootSGN);
		jointSize(m_JointSize);
		boneSize(m_BoneSize);
		
		BoundingVolume BV;
		m_Bone.boundingVolume(BV);

	}//initialize

	void IKStickFigureActor::clear(void) {
		IKSkeletalActor::clear();

		StaticActor m_Joint;
		StaticActor m_Bone;
		
		m_SG.clear();
		m_RootSGN.clear();
		for (auto& i : m_JointSGNs) if (nullptr != i) delete i;	
		for (auto& i : m_JointTransformSGNs) if (nullptr != i) delete i;	
		for (auto& i : m_BoneSGNs) if (nullptr != i) delete i;
		for (auto& i : m_JointValues) if(nullptr != i) delete i;
		m_JointSGNs.clear();
		m_JointTransformSGNs.clear();
		m_BoneSGNs.clear();
		m_JointValues.clear();

		m_JointSize = 0.0f;
		m_BoneSize = 0.0f;
	}//clear

	void IKStickFigureActor::release(void) {
		delete this;
	}//release

	void IKStickFigureActor::jointMaterial(const T3DMesh<float>::Material Mat) {
		for (uint32_t i = 0; i < m_Joint.materialCount(); ++i) {
			auto* pMat = m_Joint.material(i);
			pMat->color(Mat.Color);
			pMat->roughness(Mat.Roughness);
			pMat->metallic(Mat.Metallic);
		}
	}//jointColor

	void IKStickFigureActor::boneMaterial(const T3DMesh<float>::Material Mat) {
		for (uint32_t i = 0; i < m_Bone.materialCount(); ++i) {
			auto* pMat = m_Bone.material(i);
			pMat->color(Mat.Color);
			pMat->roughness(Mat.Roughness);
			pMat->metallic(Mat.Metallic);
		}
	}//boneColor

	void IKStickFigureActor::jointSize(float Size) {
		for (auto i : m_JointSGNs) i->scale(Vector3f(Size, Size, Size));
		m_JointSize = Size;
	}//jointSize

	void IKStickFigureActor::boneSize(float Size) {
		for (auto i : m_BoneSGNs) i->scale(Vector3f(Size, 1.0f, Size));
		m_BoneSize = Size;
	}//boneSize

	T3DMesh<float>::Material IKStickFigureActor::jointMaterial(void)const {
		T3DMesh<float>::Material Rval;
		if (m_Joint.materialCount() > 0) {
			Rval.Color = m_Joint.material(0)->color();
			Rval.Roughness = m_Joint.material(0)->roughness();
			Rval.Metallic = m_Joint.material(0)->metallic();
		}
		return Rval;
	}//jointMaterial

	T3DMesh<float>::Material IKStickFigureActor::boneMaterial(void)const {
		T3DMesh<float>::Material Rval;
		if (m_Bone.materialCount() > 0) {
			Rval.Color = m_Bone.material(0)->color();
			Rval.Roughness = m_Bone.material(0)->roughness();
			Rval.Metallic = m_Bone.material(0)->metallic();
		}
		return Rval;
	}//boneMaterial

	float IKStickFigureActor::jointSize(void)const {
		return m_JointSize;
	}//joinSize

	float IKStickFigureActor::boneSize(void)const {
		return m_BoneSize;
	}//boneSize


	void IKStickFigureActor::render(class RenderDevice* pRDev, Eigen::Quaternionf Rotation, Eigen::Vector3f Translation, Eigen::Vector3f Scale) {

		// set current animation data 
		// if active animation is nullptr bind pose will be set
		m_pAnimationController->applyAnimation(true);
		m_pAnimationController->updateSkeletonValues(&m_JointValues);

		for (auto i : m_JointValues) {
			m_JointTransformSGNs[i->ID]->translation(Scale.cwiseProduct(i->LocalPosition));
			m_JointTransformSGNs[i->ID]->rotation(i->LocalRotation);
			m_JointTransformSGNs[i->ID]->scale(i->LocalScale);
		}
		m_RootSGN.translation(Translation);
		m_RootSGN.rotation(Rotation);
		m_RootSGN.scale(Scale);
		
		// compute bone transformations
		for (auto i : m_JointValues) {
			if (i->Parent == -1) continue; // we don't care about the root node

			Vector3f BoneVec = m_JointValues[i->ID]->LocalPosition;
			//Matrix3f BoneOrientation = CForgeMath::alignVectors(Vector3f::UnitY(), BoneVec.normalized());

			//Quaternionf R; // = m_JointValues[i->Parent]->LocalRotation;
			//R = BoneOrientation;
			Quaternionf R;
			R.setFromTwoVectors(Vector3f::UnitY(), BoneVec); // produces better orientation of bone than CForgeMath::alignVectors

			if (R.norm() < 1.01f) {
				m_BoneSGNs[i->ID]->rotation(R);
			}
			else {
				printf("Error occured\n");
			}

			float Length = BoneVec.norm();
			Vector3f s = m_BoneSGNs[i->ID]->scale();
			s.y() = Length;
			m_BoneSGNs[i->ID]->scale(s);

		}

		m_SG.update(1.0f);
		m_SG.render(pRDev);
	}//render

	void IKStickFigureActor::buildMaterial(T3DMesh<float>::Material* pMat) {
		pMat->Color = Vector4f(0.0f, 0.0f, 1.0f, 1.0f);
		pMat->Metallic = 0.3f;
		pMat->Roughness = 0.2f;
		pMat->VertexShaderForwardPass.push_back("Shader/ForwardPassPBS.vert");
		pMat->FragmentShaderForwardPass.push_back("Shader/ForwardPassPBS.frag");
		pMat->VertexShaderGeometryPass.push_back("Shader/BasicGeometryPass.vert");
		pMat->FragmentShaderGeometryPass.push_back("Shader/BasicGeometryPass.frag");
		pMat->VertexShaderShadowPass.push_back("Shader/ShadowPassShader.vert");
		pMat->FragmentShaderShadowPass.push_back("Shader/ShadowPassShader.frag");
	}//buildMaterial

	void IKStickFigureActor::createBone(T3DMesh<float>::Bone* pBone, SGNTransformation* pParent) {

		SGNTransformation* pTransSGN = m_JointTransformSGNs[pBone->ID];
		SGNGeometry* pGeomSGN = nullptr;
		pGeomSGN = m_JointSGNs[pBone->ID];

		pTransSGN->init(pParent);
		pGeomSGN->init(pTransSGN, &m_Joint);

		for (auto i : pBone->Children) {

			pGeomSGN = m_BoneSGNs[i->ID];
			pGeomSGN->init(pTransSGN, &m_Bone);
			createBone(i, pTransSGN);
		}

	}//createBone

}//name space