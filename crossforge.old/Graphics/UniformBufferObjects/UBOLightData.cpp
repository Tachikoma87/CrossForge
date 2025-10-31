#include "UBOLightData.h"
#include "../Lights/DirectionalLight.h"
#include "../Lights/PointLight.h"
#include "../Lights/SpotLight.h"

namespace CForge {

#pragma region DirectionalLightData
	void UBOLightData::DirectionalLightData::init(uint32_t LightCount) {
		// compute offsets
		if (LightCount == 0) return;

		for (uint32_t i = 0; i < LightCount; ++i) {
			uint32_t Offset = 0;
			DirectionOffsets.push_back(Offset + i * sizeof(float) * 4);

			Offset += LightCount * sizeof(float) * 4;
			ColorOffsets.push_back(Offset + i * sizeof(float)*4);
			IntensityOffsets.push_back(ColorOffsets[i] + sizeof(float) * 3);

			Offset += LightCount * sizeof(float) * 4;
			LightMatricesOffset.push_back(Offset + i * sizeof(float) * 16);

			Offset += LightCount * sizeof(float) * 16;
			ShadowIDOffsets.push_back(Offset + i * sizeof(int32_t) * 4);

			Offset += LightCount * sizeof(float) * 4;

		}//for[lights]
		
		// initialize buffer
		Buffer.init(GLBuffer::BTYPE_UNIFORM, GLBuffer::BUSAGE_DYNAMIC_DRAW, nullptr, size());
	
	}//initialize

	void UBOLightData::DirectionalLightData::clear(void) {
		Buffer.clear();
		DirectionOffsets.clear();
		ColorOffsets.clear();
		IntensityOffsets.clear();
		LightMatricesOffset.clear();
		ShadowIDOffsets.clear();
	}//clear


	uint32_t UBOLightData::DirectionalLightData::lightCount(void)const {
		return DirectionOffsets.size();
	}

	uint32_t UBOLightData::DirectionalLightData::size(void) const {
		uint32_t Rval = 0;
		Rval += sizeof(float) * 4; // Direction vector
		Rval += sizeof(float) * 4; // Color/Intensity vector
		Rval += sizeof(float) * 16; // Light space matrix
		Rval += sizeof(int32_t) * 4; // Shadow ID
		return Rval * lightCount();
	}//size

#pragma endregion

#pragma region PointLightData
	void UBOLightData::PointLightData::init(uint32_t LightCount) {

		for (uint32_t i = 0; i < LightCount; ++i) {
			uint32_t Offset = 0;
			PositionOffsets.push_back(Offset + i * sizeof(float) * 4);
			Offset += LightCount * sizeof(float) * 4;

			ColorOffsets.push_back(Offset + i * sizeof(float) * 4);
			IntensityOffsets.push_back(ColorOffsets[i] + sizeof(float) * 3);
			Offset += LightCount * sizeof(float) * 4;

			AttenuationOffsets.push_back(Offset + i * sizeof(float) * 4);
			Offset += LightCount * sizeof(float) * 4;

			DirectionOffsets.push_back(Offset + i * sizeof(float) * 4);
			Offset += LightCount * sizeof(float) + 4;

			LightMatrixOffsets.push_back(Offset + i * sizeof(float) * 16);
			Offset += LightCount * sizeof(float) * 16;

			ShadowIDOffsets.push_back(Offset + i * sizeof(int32_t) * 4);
			Offset += LightCount * sizeof(int32_t) * 4;
		}//for[light count]

		// initialize buffer
		Buffer.init(GLBuffer::BTYPE_UNIFORM, GLBuffer::BUSAGE_STATIC_DRAW, nullptr, size());
	}//initialize

	void UBOLightData::PointLightData::clear(void) {
		PositionOffsets.clear();
		ColorOffsets.clear();
		IntensityOffsets.clear();
		AttenuationOffsets.clear();
		DirectionOffsets.clear();
		LightMatrixOffsets.clear();
		ShadowIDOffsets.clear();
		Buffer.clear();
	}//clear

	uint32_t UBOLightData::PointLightData::lightCount(void)const {
		return PositionOffsets.size();
	}//lightCount

	uint32_t UBOLightData::PointLightData::size(void)const {
		uint32_t Rval = 0;
		Rval += sizeof(float) * 4; // Position	
		Rval += sizeof(float) * 4; // Color | Intensity
		Rval += sizeof(float) * 4; // Attenuation
		Rval += sizeof(float) * 4; // Direction
		Rval += sizeof(float) * 16; // LightMatrix
		Rval += sizeof(int32_t) * 4; // Shadow ID
		return lightCount() * Rval;
	}//size

#pragma endregion

#pragma region SpotLights
	void UBOLightData::SpotLightData::init(uint32_t LightCount) {
		for (uint32_t i = 0; i < LightCount; ++i) {
			uint32_t Offset = 0;
			PositionOffsets.push_back(Offset + i * sizeof(float) * 4);
			Offset += LightCount * sizeof(float) * 4;

			DirectionOffsets.push_back(Offset + i * sizeof(float) * 4);
			OuterCutOffOffsets.push_back(DirectionOffsets[i] + sizeof(float) * 3);
			Offset += LightCount * sizeof(float) * 4;

			ColorOffsets.push_back(Offset + i * sizeof(float) * 4);
			IntensityOffsets.push_back(ColorOffsets[i] + sizeof(float) * 3);
			Offset += LightCount * sizeof(float) * 4;

			AttenuationOffsets.push_back(Offset + i * sizeof(float) * 4);
			InnerCutOffOffsets.push_back(AttenuationOffsets[i] + sizeof(float) * 3);
			Offset += LightCount * sizeof(float) * 4;

			LightMatrixOffsets.push_back(Offset + i * sizeof(float) * 16);
			Offset += LightCount * sizeof(float) * 16;

			ShadowIDOffsets.push_back(Offset + i * sizeof(int32_t) * 4);
			Offset += LightCount + sizeof(int32_t) * 4;
		}//for[lights]

		Buffer.init(GLBuffer::BTYPE_UNIFORM, GLBuffer::BUSAGE_STATIC_DRAW, nullptr, size());
	}//initialize

	void UBOLightData::SpotLightData::clear(void) {
		PositionOffsets.clear();
		DirectionOffsets.clear();
		OuterCutOffOffsets.clear();
		ColorOffsets.clear();
		IntensityOffsets.clear();
		AttenuationOffsets.clear();
		InnerCutOffOffsets.clear();
		LightMatrixOffsets.clear();
		ShadowIDOffsets.clear();
		Buffer.clear();
	}//clear

	uint32_t UBOLightData::SpotLightData::lightCount(void)const {
		return PositionOffsets.size();
	}//lightCount

	uint32_t UBOLightData::SpotLightData::size(void)const {
		uint32_t Rval = 0;
		Rval += sizeof(float) * 4; // Position
		Rval += sizeof(float) * 4; // Direction | Outer CutOff
		Rval += sizeof(float) * 4; // Color | Intensity
		Rval += sizeof(float) * 4; // Attenuation | Inner Cut Off
		Rval += sizeof(float) * 16; // LightMatrix
		Rval += sizeof(int32_t) * 4; // Shadow ID
		return lightCount() * Rval;
	}//size

#pragma endregion

	UBOLightData::UBOLightData(void): CForgeObject("UBOLightData") {

	}//Constructor

	UBOLightData::~UBOLightData(void) {
		clear();
	}//Destructor

	void UBOLightData::init(uint32_t DirectionalLights, uint32_t PointLights, uint32_t SpotLights) {
		m_DirLightsData.init(DirectionalLights);
		m_PointLightsData.init(PointLights);
		m_SpotLightsData.init(SpotLights);
	}//initialize

	void UBOLightData::clear(void) {
		m_DirLightsData.clear();
		m_PointLightsData.clear();
		m_SpotLightsData.clear();
	}//clear

	uint32_t UBOLightData::size(ILight::LightType Type) {
		uint32_t Rval = 0; 
		switch (Type) {
		case ILight::LIGHT_DIRECTIONAL: Rval = m_DirLightsData.size(); break;
		case ILight::LIGHT_POINT: Rval = m_PointLightsData.size(); break;
		case ILight::LIGHT_SPOT: Rval = m_SpotLightsData.size(); break;
		default: throw CForgeExcept("Unknown light type specified!"); break;
		}
		return Rval;
	}//size

	uint32_t UBOLightData::lightCount(ILight::LightType Type) {
		uint32_t Rval = 0;
		switch (Type) {
		case ILight::LIGHT_DIRECTIONAL: Rval = m_DirLightsData.lightCount(); break;
		case ILight::LIGHT_POINT: Rval = m_PointLightsData.lightCount(); break;
		case ILight::LIGHT_SPOT: Rval = m_SpotLightsData.lightCount(); break;
		default: {
			throw CForgeExcept("Unknown light type specified!");
		}break;
		}//switch[Light type]
		return Rval;
	}//lightCount

	void UBOLightData::bind(uint32_t BindingPoint, ILight::LightType Type) {
		switch (Type) {
		case ILight::LIGHT_DIRECTIONAL: {
			m_DirLightsData.Buffer.bindBufferBase(BindingPoint);

		}break;
		case ILight::LIGHT_POINT: {
			m_PointLightsData.Buffer.bindBufferBase(BindingPoint);

		}break;
		case ILight::LIGHT_SPOT: {
			m_SpotLightsData.Buffer.bindBufferBase(BindingPoint);
		}break;
		default: throw CForgeExcept("Unknown light type specified!");
		}
	}//bind


	void UBOLightData::position(Eigen::Vector3f Pos, ILight::LightType Type, uint32_t Index) {
		switch (Type) {
		case ILight::LIGHT_DIRECTIONAL: {
			if (Index >= m_DirLightsData.lightCount()) throw IndexOutOfBoundsExcept("Index");
			// directional lights have no position
		}break;
		case ILight::LIGHT_POINT: {
			if (Index >= m_PointLightsData.lightCount()) throw IndexOutOfBoundsExcept("Index");
			m_PointLightsData.Buffer.bufferSubData(m_PointLightsData.PositionOffsets[Index], sizeof(float) * 3, Pos.data());
		}break;
		case ILight::LIGHT_SPOT: {
			if (Index >= m_SpotLightsData.lightCount()) throw IndexOutOfBoundsExcept("Index");
			m_SpotLightsData.Buffer.bufferSubData(m_SpotLightsData.PositionOffsets[Index], sizeof(float) * 3, Pos.data());
		}break;
		default: throw CForgeExcept("Unknown light type specified!");
		}
	}//position

	void UBOLightData::direction(Eigen::Vector3f Dir, ILight::LightType Type, uint32_t Index) {
		switch (Type) {
		case ILight::LIGHT_DIRECTIONAL: {
			if (Index >= m_DirLightsData.lightCount()) throw IndexOutOfBoundsExcept("Index");
			m_DirLightsData.Buffer.bufferSubData(m_DirLightsData.DirectionOffsets[Index], sizeof(float)*3, Dir.data());
		}break;
		case ILight::LIGHT_POINT: {
			if (Index >= m_PointLightsData.lightCount()) throw IndexOutOfBoundsExcept("Index");
			m_PointLightsData.Buffer.bufferSubData(m_PointLightsData.DirectionOffsets[Index], sizeof(float) * 3, Dir.data());
		}break;
		case ILight::LIGHT_SPOT: {
			if (Index >= m_SpotLightsData.lightCount()) throw IndexOutOfBoundsExcept("Index");
			m_SpotLightsData.Buffer.bufferSubData(m_SpotLightsData.DirectionOffsets[Index], sizeof(float) * 3, Dir.data());
		}break;
		default: throw CForgeExcept("Unknown light type specified!");
		}
	}//direction

	void UBOLightData::color(Eigen::Vector3f Color, ILight::LightType Type, uint32_t Index) {
		switch (Type) {
		case ILight::LIGHT_DIRECTIONAL: {
			if (Index >= m_DirLightsData.lightCount()) throw IndexOutOfBoundsExcept("Index");
			m_DirLightsData.Buffer.bufferSubData(m_DirLightsData.ColorOffsets[Index], sizeof(float) * 3, Color.data());
		}break;
		case ILight::LIGHT_POINT: {
			if (Index >= m_PointLightsData.lightCount()) throw IndexOutOfBoundsExcept("Index");
			m_PointLightsData.Buffer.bufferSubData(m_PointLightsData.ColorOffsets[Index], sizeof(float) * 3, Color.data());

		}break;
		case ILight::LIGHT_SPOT: {
			if (Index >= m_SpotLightsData.lightCount()) throw IndexOutOfBoundsExcept("Index");
			m_SpotLightsData.Buffer.bufferSubData(m_SpotLightsData.ColorOffsets[Index], sizeof(float) * 3, Color.data());
		}break;
		default: throw CForgeExcept("Unknown light type specified!");
		}
	}//color

	void UBOLightData::intensity(float Intensity, ILight::LightType Type, uint32_t Index) {
		switch (Type) {
		case ILight::LIGHT_DIRECTIONAL: {
			if (Index >= m_DirLightsData.lightCount()) throw IndexOutOfBoundsExcept("Index");
			m_DirLightsData.Buffer.bufferSubData(m_DirLightsData.IntensityOffsets[Index], sizeof(float), &Intensity); 
		}break;
		case ILight::LIGHT_POINT: {
			if (Index >= m_PointLightsData.lightCount()) throw IndexOutOfBoundsExcept("Index");
			m_PointLightsData.Buffer.bufferSubData(m_PointLightsData.IntensityOffsets[Index], sizeof(float), &Intensity);

		}break;
		case ILight::LIGHT_SPOT: {
			if (Index >= m_SpotLightsData.lightCount()) throw IndexOutOfBoundsExcept("Index");
			m_SpotLightsData.Buffer.bufferSubData(m_SpotLightsData.IntensityOffsets[Index], sizeof(float), &Intensity);
		}break;
		default: throw CForgeExcept("Unknown light type specified!");
		}
	}//intensity

	void UBOLightData::attenuation(Eigen::Vector3f Attenuation, ILight::LightType Type, uint32_t Index) {
		switch (Type) {
		case ILight::LIGHT_DIRECTIONAL: {
			if (Index >= m_DirLightsData.lightCount()) throw IndexOutOfBoundsExcept("Index");
			// directional lights do not have attenuation
		}break;
		case ILight::LIGHT_POINT: {
			if (Index >= m_PointLightsData.lightCount()) throw IndexOutOfBoundsExcept("Index");
			m_PointLightsData.Buffer.bufferSubData(m_PointLightsData.AttenuationOffsets[Index], sizeof(float)*3, Attenuation.data());
		}break;
		case ILight::LIGHT_SPOT: {
			if (Index >= m_SpotLightsData.lightCount()) throw IndexOutOfBoundsExcept("Index");
			m_SpotLightsData.Buffer.bufferSubData(m_SpotLightsData.AttenuationOffsets[Index], sizeof(float) * 3, Attenuation.data());
		}break;
		default: throw CForgeExcept("Unknown light type specified!");
		}
	}//attenuation

	void UBOLightData::cutOff(Eigen::Vector2f CutOff, ILight::LightType Type, uint32_t Index) {
		switch (Type) {
		case ILight::LIGHT_DIRECTIONAL: {
			// directional lights do not have cut off values
		}break;
		case ILight::LIGHT_POINT: {
			// point lights do not have cut off values
		}break;
		case ILight::LIGHT_SPOT: {
			if (Index >= m_SpotLightsData.lightCount()) throw IndexOutOfBoundsExcept("Index");
			const float InnerCutOff = std::cos(CutOff.x());
			const float OuterCutOff = std::cos(CutOff.y());
			m_SpotLightsData.Buffer.bufferSubData(m_SpotLightsData.InnerCutOffOffsets[Index], sizeof(float), &InnerCutOff);
			m_SpotLightsData.Buffer.bufferSubData(m_SpotLightsData.OuterCutOffOffsets[Index], sizeof(float), &OuterCutOff);
		}break;
		default: throw CForgeExcept("Unknown light type specified!");
		}
	}//cutOff

	void UBOLightData::lightSpaceMatrix(Eigen::Matrix4f Mat, ILight::LightType Type, uint32_t Index) {
		switch (Type) {
		case ILight::LIGHT_DIRECTIONAL: {
			if (Index >= m_DirLightsData.lightCount()) throw IndexOutOfBoundsExcept("Index");
			m_DirLightsData.Buffer.bufferSubData(m_DirLightsData.LightMatricesOffset[Index], sizeof(float) * 16, Mat.data());
		}break;
		case ILight::LIGHT_POINT: {
			if (Index >= m_PointLightsData.lightCount()) throw IndexOutOfBoundsExcept("Index");
			m_PointLightsData.Buffer.bufferSubData(m_PointLightsData.LightMatrixOffsets[Index], sizeof(float) * 16, Mat.data());

		}break;
		case ILight::LIGHT_SPOT: {
			if (Index >= m_SpotLightsData.lightCount()) throw IndexOutOfBoundsExcept("Index");
			m_SpotLightsData.Buffer.bufferSubData(m_SpotLightsData.LightMatrixOffsets[Index], sizeof(float) * 16, Mat.data());
		}break;
		default: throw CForgeExcept("Unknown light type specified!");
		}
	}//lightSpaceMatrix

	void UBOLightData::shadowID(int32_t ID, ILight::LightType Type, uint32_t Index) {
		switch (Type) {
		case ILight::LIGHT_DIRECTIONAL: {
			if (Index >= m_DirLightsData.lightCount()) throw IndexOutOfBoundsExcept("Index");
			m_DirLightsData.Buffer.bufferSubData(m_DirLightsData.ShadowIDOffsets[Index], sizeof(int32_t), &ID);
		}break;
		case ILight::LIGHT_POINT: {
			if (Index >= m_PointLightsData.lightCount()) throw IndexOutOfBoundsExcept("Index");
			m_PointLightsData.Buffer.bufferSubData(m_PointLightsData.ShadowIDOffsets[Index], sizeof(int32_t), &ID);

		}break;
		case ILight::LIGHT_SPOT: {
			if (Index >= m_SpotLightsData.lightCount()) throw IndexOutOfBoundsExcept("Index");
			m_SpotLightsData.Buffer.bufferSubData(m_SpotLightsData.ShadowIDOffsets[Index], sizeof(int32_t), &ID);
		}break;
		default: throw CForgeExcept("Unknown light type specified!");
		}
	}//shadowID

	void UBOLightData::updateLight(ILight* pLight, uint32_t Index) {
	
		switch (pLight->type()) {
		case ILight::LIGHT_DIRECTIONAL: {
			DirectionalLight* pL = (DirectionalLight*)pLight;
			direction(pL->direction(), ILight::LIGHT_DIRECTIONAL, Index);
			color(pL->color(), ILight::LIGHT_DIRECTIONAL, Index);
			intensity(pL->intensity(), ILight::LIGHT_DIRECTIONAL, Index);
		}break;
		case ILight::LIGHT_POINT: {
			PointLight* pL = (PointLight*)pLight;
			position(pL->position(), ILight::LIGHT_POINT, Index);
			direction(pL->direction(), ILight::LIGHT_POINT, Index);
			color(pL->color(), ILight::LIGHT_POINT, Index);
			intensity(pL->intensity(), ILight::LIGHT_POINT, Index);
			attenuation(pL->attenuation(), ILight::LIGHT_POINT, Index);
		}break;
		case ILight::LIGHT_SPOT: {
			SpotLight* pL = (SpotLight*)pLight;
			position(pL->position(), ILight::LIGHT_SPOT, Index);
			direction(pL->direction(), ILight::LIGHT_SPOT, Index);
			color(pL->color(), ILight::LIGHT_SPOT, Index);
			intensity(pL->intensity(), ILight::LIGHT_SPOT, Index);
			attenuation(pL->attenuation(), ILight::LIGHT_SPOT, Index);
			cutOff(pL->cutOff(), ILight::LIGHT_SPOT, Index);
		}break;
		default: throw CForgeExcept("Unknown light type encountered!");
		}
	}//updateLight


}//name space