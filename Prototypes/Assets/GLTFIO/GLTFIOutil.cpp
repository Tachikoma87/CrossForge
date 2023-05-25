
#include "GLTFIOutil.hpp"

namespace CForge {

int GLTFIOutil::sizeOfGltfComponentType(const int componentType) {
	switch (componentType) {
	default:
		return 1;
	case TINYGLTF_COMPONENT_TYPE_BYTE:
		return 1;
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
		return 1;
	case TINYGLTF_COMPONENT_TYPE_SHORT:
		return 2;
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
		return 2;
	case TINYGLTF_COMPONENT_TYPE_INT:
		return 4;
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
		return 4;
	case TINYGLTF_COMPONENT_TYPE_FLOAT:
		return 4;
	case TINYGLTF_COMPONENT_TYPE_DOUBLE:
		return 8;
	}
}//sizeOfGltfComponentType

int GLTFIOutil::componentCount(const int type) {
	switch (type) {
	default:
		return 1;
	case TINYGLTF_TYPE_SCALAR:
		return 1;
	case TINYGLTF_TYPE_VEC2:
		return 2;
	case TINYGLTF_TYPE_VEC3:
		return 3;
	case TINYGLTF_TYPE_VEC4:
		return 4;
	case TINYGLTF_TYPE_MAT2:
		return 4;
	case TINYGLTF_TYPE_MAT3:
		return 9;
	case TINYGLTF_TYPE_MAT4:
		return 16;
	}
}

bool GLTFIOutil::componentIsMatrix(const int type) {
	switch (type) {
	case TINYGLTF_TYPE_MAT2:
	case TINYGLTF_TYPE_MAT3:
	case TINYGLTF_TYPE_MAT4:
		return true;
	}

	return false;
}

void GLTFIOutil::toVec2f(const std::vector<std::vector<float>>* pIn, std::vector<Eigen::Vector2f>* pOut) {
	for (auto e : *pIn) {
		Eigen::Vector2f vec;

		vec(0) = e[0];
		vec(1) = e[1];
		pOut->push_back(vec);
	}
}

void GLTFIOutil::toVec3f(const std::vector<std::vector<float>>* pIn, std::vector<Eigen::Vector3f>* pOut) {
	for (auto e : *pIn) {
		Eigen::Vector3f vec;

		vec(0) = e[0];
		vec(1) = e[1];
		vec(2) = e[2];
		pOut->push_back(vec);
	}
}

void GLTFIOutil::toVec4f(const std::vector<std::vector<float>>* pIn, std::vector<Eigen::Vector4f>* pOut) {
	for (auto e : *pIn) {
		Eigen::Vector4f vec;

		vec(0) = e[0];
		vec(1) = e[1];
		vec(2) = e[2];
		vec(3) = e[3];
		pOut->push_back(vec);
	}
}

void GLTFIOutil::toQuatf(const std::vector<std::vector<float>>* pIn, std::vector<Eigen::Quaternionf>* pOut) {
	for (auto e : *pIn) {
		Eigen::Quaternionf quat(e[3], e[0], e[1], e[2]);

		pOut->push_back(quat);
	}
}

void GLTFIOutil::toMat4f(const std::vector<std::vector<float>>* pIn, std::vector<Eigen::Matrix4f>* pOut) {
	for (auto vec : *pIn) {
		Eigen::Matrix4f mat;

		for (int i = 0; i < vec.size(); i++) {
			mat(i % 4, i / 4) = vec[i];
		}

		pOut->push_back(mat);
	}
}

Eigen::Matrix4f GLTFIOutil::toSingleMat4(const std::vector<double>* pIn) {
	Eigen::Matrix4f mat;

	for (int i = 0; i < pIn->size(); i++) {
		mat(i % 4, i / 4) = (float)(*pIn)[i];
	}

	return mat;
}

void GLTFIOutil::fromVec2f(const std::vector<Eigen::Vector2f>* pIn, std::vector<std::vector<float>>* pOut) {
	for (auto e : *pIn) {
		std::vector<float> toAdd;
		toAdd.push_back(e(0));
		toAdd.push_back(e(1));
		pOut->push_back(toAdd);
	}
}

void GLTFIOutil::fromVec3f(const std::vector<Eigen::Vector3f>* pIn, std::vector<std::vector<float>>* pOut) {
	for (auto e : *pIn) {
		std::vector<float> toAdd;
		toAdd.push_back(e(0));
		toAdd.push_back(e(1));
		toAdd.push_back(e(2));
		pOut->push_back(toAdd);
	}
}

void GLTFIOutil::fromVec4f(const std::vector<Eigen::Vector4f>* pIn, std::vector<std::vector<float>>* pOut) {
	for (auto e : *pIn) {
		std::vector<float> toAdd;
		toAdd.push_back(e(0));
		toAdd.push_back(e(1));
		toAdd.push_back(e(2));
		toAdd.push_back(e(3));
		pOut->push_back(toAdd);
	}
}

void GLTFIOutil::fromQuatf(const std::vector<Eigen::Quaternionf>* pIn, std::vector<std::vector<float>>* pOut) {
	for (auto e : *pIn) {
		std::vector<float> toAdd;
		toAdd.push_back(e.x());
		toAdd.push_back(e.y());
		toAdd.push_back(e.z());
		toAdd.push_back(e.w());
		pOut->push_back(toAdd);
	}
}

void GLTFIOutil::fromMat4f(const std::vector<Eigen::Matrix4f>* pIn, std::vector<std::vector<float>>* pOut) {
	for (auto mat : *pIn) {
		std::vector<float> toAdd;
		for (int i = 0; i < 16; i++) {
			toAdd.push_back(mat(i % 4, i / 4));
		}
		pOut->push_back(toAdd);
	}
}

void GLTFIOutil::fromUVtoUVW(const std::vector<Eigen::Vector2f>* pIn, std::vector<Eigen::Vector3f>* pOut) {
	for (auto e : *pIn) {
		Eigen::Vector3f vec;

		vec(0) = e(0);
		vec(1) = e(1);
		vec(2) = 0.0f;
		pOut->push_back(vec);
	}
}

void GLTFIOutil::fromUVWtoUV(const std::vector<Eigen::Vector3f>* pIn, std::vector<Eigen::Vector2f>* pOut) {
	for (auto e : *pIn) {
		Eigen::Vector2f vec;

		vec(0) = e(0);
		vec(1) = e(1);
		pOut->push_back(vec);
	}
}

Eigen::Vector3f GLTFIOutil::getTranslation(const Eigen::Matrix4f& mat) {
	return Eigen::Vector3f(mat(0, 3), mat(1, 3), mat(2, 3));
}

Eigen::Quaternionf GLTFIOutil::getRotation(const Eigen::Matrix4f& mat) {
	//TODO should this be sufficient?
	//Eigen::Quaternionf ret(mat);
	//return ret;

	auto scale = getScale(mat);

	Eigen::Matrix4f rotation(mat);

	rotation(0, 0) /= scale.x();
	rotation(1, 0) /= scale.x();
	rotation(2, 0) /= scale.x();

	rotation(0, 1) /= scale.y();
	rotation(1, 1) /= scale.y();
	rotation(2, 1) /= scale.y();

	rotation(0, 2) /= scale.z();
	rotation(1, 2) /= scale.z();
	rotation(2, 2) /= scale.z();

	float t;
	Eigen::Vector4f q;

	if (rotation(2, 2) < 0) {
		if (rotation(0, 0) > rotation(1, 1)) {
			t = 1.0 + rotation(0, 0) - rotation(1, 1) - rotation(2, 2);
			q(0) = t;
			q(1) = rotation(0, 1) + rotation(1, 0);
			q(2) = rotation(2, 0) + rotation(0, 2);
			q(3) = rotation(1, 2) + rotation(2, 1);
		}
		else {
			t = 1.0 - rotation(0, 0) + rotation(1, 1) - rotation(2, 2);
			q(0) = rotation(0, 1) + rotation(1, 0);
			q(1) = t;
			q(2) = rotation(1, 2) + rotation(2, 1);
			q(3) = rotation(2, 0) + rotation(0, 2);
		}
	}
	else {
		if (rotation(0, 0) < -rotation(1, 1)) {
			t = 1.0 - rotation(0, 0) - rotation(1, 1) + rotation(2, 2);
			q(0) = rotation(2, 0) + rotation(0, 2);
			q(1) = rotation(1, 2) + rotation(2, 1);
			q(2) = t;
			q(3) = rotation(0, 1) - rotation(1, 0);
		}
		else {
			t = 1.0 + rotation(0, 0) + rotation(1, 1) + rotation(2, 2);
			q(0) = rotation(1, 2) - rotation(2, 1);
			q(1) = rotation(2, 0) - rotation(0, 2);
			q(2) = rotation(0, 1) - rotation(1, 0);
			q(3) = t;
		}
	}

	q *= 0.5 / sqrt(t);

	Eigen::Quaternionf quat(q(3), q(0), q(1), q(2));

	quat.normalize();

	return quat;
}

Eigen::Vector3f GLTFIOutil::getScale(const Eigen::Matrix4f& mat) {
	Eigen::Vector3f sx(mat(0, 0), mat(0, 1), mat(0, 2));
	Eigen::Vector3f sy(mat(1, 0), mat(1, 1), mat(1, 2));
	Eigen::Vector3f sz(mat(2, 0), mat(2, 1), mat(2, 2));

	Eigen::Vector3f scale(sx.norm(), sy.norm(), sz.norm());

	return scale;
}

}//CForge
