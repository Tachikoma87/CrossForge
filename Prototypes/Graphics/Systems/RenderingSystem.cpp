
#include <Windows.h>
#include <glad/glad.h>
#include "RenderingSystem.h"

#include "../../ECS/PrototypeComponents/PositionComponent2D.h"


namespace crossforge {

	RenderingSystem::RenderingSystem(): SystemBase(identification) {

	}

	RenderingSystem::~RenderingSystem() {

	}

	void RenderingSystem::initialize() {
		clear();
	}

	void RenderingSystem::clear() {
		m_entities.clear();
	}

	void RenderingSystem::update() {

		for (auto pEntity : m_entities) {
			auto pScene = std::static_pointer_cast<SceneEntity>(pEntity);
			renderScene(pScene);
		}
	}

	bool RenderingSystem::isEntityValid(EntityBasePtr pEntity)const {
		bool result = true;
		if (nullptr == pEntity) throw NullpointerExcept("pEntity");
		if (0 != pEntity->getIdentification().compare(SceneEntity::identification)) result = false;
		return result;
	}

	void RenderingSystem::renderScene(SceneEntityPtr pScene) {
		// set viewport
		auto pSceneSettingsComponent = pScene->getSceneSettingsComponent();

		glViewport(pSceneSettingsComponent->viewportPosition().x(), pSceneSettingsComponent->viewportPosition().y(),
			pSceneSettingsComponent->viewportSize().x(), pSceneSettingsComponent->viewportSize().y());


		glScissor(pSceneSettingsComponent->viewportPosition().x(), pSceneSettingsComponent->viewportPosition().y(),
			pSceneSettingsComponent->viewportSize().x(), pSceneSettingsComponent->viewportSize().y());
		glEnable(GL_SCISSOR_TEST);

		auto pPos = pScene->getComponent<PositionComponent2D>();

		glClearColor(pPos->getPosition().x(), pPos->getPosition().y(), pPos->getPosition().x()/2.0f + pPos->getPosition().y()/2.0f, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);

	}

}