#include "PbrMaterialsComponent.h"

namespace crossforge {
	PbrMaterial::PbrMaterial() {
		m_ambientOcclusion = 0.0f;
		m_metallic = 0.04f;
		m_roughness = 0.1f;
		initialize();
	}
	PbrMaterial::~PbrMaterial() {
		clear();
	}

	void PbrMaterial::initialize(std::shared_ptr<PbrMaterial> pRef) {
		clear();
		if (nullptr != pRef) {
			m_ambientOcclusion = pRef->ambientOcclusion();
			m_metallic = pRef->metallic();
			m_roughness = pRef->roughness();
			for (uint32_t i = 0; i < COLOR_TYPE_COUNT; ++i) m_colors.push_back(pRef->color(ColorType(i)));
			for (uint32_t i = 0; i < TEXTURE_TYPE_COUNT; ++i) m_textures.push_back(pRef->texture(TextureType(i)));
		}
		m_pUbo = std::make_shared<UBOPbrMaterialComponent>();
		m_pUbo->initialize();
		updateUbo();

	}
	void PbrMaterial::clear() {
		m_ambientOcclusion = 0.0f;
		m_metallic = 0.04f;
		m_roughness = 0.1f;
		for (uint32_t i = 0; i < COLOR_TYPE_COUNT; ++i) m_colors.push_back(Eigen::Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
		for (uint32_t i = 0; i < TEXTURE_TYPE_COUNT; ++i) m_textures.push_back(nullptr);
		m_pUbo = nullptr;
	}
	
	float& PbrMaterial::ambientOcclusion() {
		return m_ambientOcclusion;
	}
	float& PbrMaterial::metallic() {
		return m_metallic;
	}
	float& PbrMaterial::roughness() {
		return m_roughness;
	}

	bool PbrMaterial::updateUbo() {
		if (nullptr == m_pUbo) throw NotInitializedExcept("m_pUbo");
		m_pUbo->setAmbientOcclusion(m_ambientOcclusion);
		m_pUbo->setMetallic(m_metallic);
		m_pUbo->setRougness(m_roughness);
		m_pUbo->setColor(m_colors[COLOR_TYPE_ALBEDO]);
		return true;
	}


	Eigen::Vector4f& PbrMaterial::color(ColorType type) {
		if (0 > type || type >= m_colors.size()) throw IndexOutOfBoundsExcept("type");
		return m_colors[type];
	}

	TextureEntityPtr& PbrMaterial::texture(TextureType type) {
		if (0 > type || type >= TEXTURE_TYPE_COUNT) throw IndexOutOfBoundsExcept("type");
		return m_textures[type];
	}

	UBOPBRMaterialComponentPtr PbrMaterial::uboPbrMaterial() {
		return m_pUbo;
	}



	PbrMaterialsComponent::PbrMaterialsComponent(): ComponentBase(PbrMaterialsComponent::identification) {

	}
	PbrMaterialsComponent::PbrMaterialsComponent(const std::string childIdentification): ComponentBase(PbrMaterialsComponent::identification) {
		m_inheritance.push_back(childIdentification);
	}
	PbrMaterialsComponent::~PbrMaterialsComponent() {
		m_pbrMaterials.clear();
	}

	void PbrMaterialsComponent::initialize() {
		clear();
	}
	void PbrMaterialsComponent::clear() {
		m_pbrMaterials.clear();
	}

	std::vector<PbrMaterialPtr>& PbrMaterialsComponent::pbrMaterials() {
		return m_pbrMaterials;
	}
	void PbrMaterialsComponent::addPbrMaterial(PbrMaterialPtr pMaterial) {
		// nullptr will be explicitly allowed here
		m_pbrMaterials.push_back(pMaterial);
	}
	PbrMaterialPtr PbrMaterialsComponent::getMaterial(uint32_t index) {
		if (0 >= m_pbrMaterials.size()) throw IndexOutOfBoundsExcept("index");
		return m_pbrMaterials[index];
	}
	uint32_t PbrMaterialsComponent::getMaterialCount()const {
		return m_pbrMaterials.size();
	}
}