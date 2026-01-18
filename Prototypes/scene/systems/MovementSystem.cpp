#include "MovementSystem.h"
#include <crossforge/scene/components/Movement3DComponent.h>
#include <crossforge/scene/components/Transformation3DComponent.h>
namespace crossforge {

	MovementSystem::MovementSystem(): SystemBase(MovementSystem::identification) {

	}
	MovementSystem::~MovementSystem() {
		clear();
	}
	MovementSystem::MovementSystem(const std::string childIdentification): SystemBase(MovementSystem::identification) {
		m_inheritance.push_back(childIdentification);
	}

	void MovementSystem::initialize() {
		clear();
	}
	void MovementSystem::clear() {
		m_entities.clear();
	}
	void MovementSystem::update() {
		for (auto pEntity : m_entities) {
			auto pTransformation = pEntity->getComponent<Transformation3DComponent>();
			auto pMovement = pEntity->getComponent<Movement3DComponent>();
			if (nullptr == pTransformation || nullptr == pMovement) continue;

			pTransformation->localPosition() += pMovement->positionDelta();
			pTransformation->localRotation() *= pMovement->rotationDelta();
			pTransformation->localScale() += pMovement->scaleDelta();
		}
	}
	bool MovementSystem::isEntityValid(EntityBasePtr pEntity)const {
		if (nullptr == pEntity) throw NullpointerExcept("pEntity");
		return pEntity->hasComponent(Transformation3DComponent::identification) && pEntity->hasComponent(Movement3DComponent::identification);
	}
}