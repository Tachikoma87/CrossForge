
#include <Eigen/Eigen>
#include <tiny_gltf.h>

namespace CForge {

/**
 * \class GLTFIOutil
 * \brief Utility class for converting various formats between CForge and tinyGLTF.
*/
class GLTFIOutil {
protected:
//#pragma region util
		static int componentCount(const int type);

		static int sizeOfGltfComponentType(const int componentType);

		static bool componentIsMatrix(const int type);

		static void toVec2f(const std::vector<std::vector<float>>* pIn, std::vector<Eigen::Vector2f>* pOut);

		static void toVec3f(const std::vector<std::vector<float>>* pIn, std::vector<Eigen::Vector3f>* pOut);

		static void toVec4f(const std::vector<std::vector<float>>* pIn, std::vector<Eigen::Vector4f>* pOut);

		static void toQuatf(const std::vector<std::vector<float>>* pIn, std::vector<Eigen::Quaternionf>* pOut);

		static void toMat4f(const std::vector<std::vector<float>>* pIn, std::vector<Eigen::Matrix4f>* pOut);

		static Eigen::Matrix4f toSingleMat4(const std::vector<double>* pin);

		static void fromVec2f(const std::vector<Eigen::Vector2f>* pIn, std::vector<std::vector<float>>* pOut);

		static void fromVec3f(const std::vector<Eigen::Vector3f>* pIn, std::vector<std::vector<float>>* pOut);

		static void fromVec4f(const std::vector<Eigen::Vector4f>* pIn, std::vector<std::vector<float>>* pOut);

		static void fromQuatf(const std::vector<Eigen::Quaternionf>* pIn, std::vector<std::vector<float>>* pOut);

		static void fromMat4f(const std::vector<Eigen::Matrix4f>* pIn, std::vector<std::vector<float>>* pOut);

		static void fromUVtoUVW(const std::vector<Eigen::Vector2f>* pIn, std::vector<Eigen::Vector3f>* pOut);

		static void fromUVWtoUV(const std::vector<Eigen::Vector3f>* pIn, std::vector<Eigen::Vector2f>* pOut);

		static int getGltfComponentType(const float value) { return TINYGLTF_COMPONENT_TYPE_FLOAT; }

		static int getGltfComponentType(const unsigned char value) { return TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE; }

		static int getGltfComponentType(const char value) { return TINYGLTF_COMPONENT_TYPE_BYTE; }

		static int getGltfComponentType(const unsigned short value) { return TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT; }

		static int getGltfComponentType(const short value) { return TINYGLTF_COMPONENT_TYPE_SHORT; }

		static int getGltfComponentType(const unsigned int value) { return TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT; }

		static int getGltfComponentType(const int value) { return TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT; }

		static Eigen::Vector3f getTranslation(const Eigen::Matrix4f& transformation);

		static Eigen::Quaternionf getRotation(const Eigen::Matrix4f& transformation);

		static Eigen::Vector3f getScale(const Eigen::Matrix4f& transformation);
//#pragma endregion
};

}//CForge
