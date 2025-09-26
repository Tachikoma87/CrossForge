#include "MovementSystem.h"
#include "PositionComponent2D.h"
#include <crossforge/Utility/CForgeUtility.h>
#include <crossforge/Math/CForgeMath.h>

namespace CForge {

	MovementSystem::MovementSystem(): SystemBase(MovementSystem::identification) {

	}

	MovementSystem::~MovementSystem() {

	}

	void MovementSystem::initialize() {
		clear();
	}

	void MovementSystem::clear() {
		m_entities.clear();
	}

	void MovementSystem::update() {
		for (EntityBasePtr pEntity : m_entities) {
			if (nullptr == pEntity) continue;

			PositionComponent2DPtr position = pEntity->getComponent<PositionComponent2D>(PositionComponent2D::identification);
			Eigen::Vector2f delta = Eigen::Vector2f::Zero();

			delta.x() = CForgeMath::randRange(-10.0f, 10.0f);
			delta.y() = CForgeMath::randRange(-5.0f, 10.0f);

			position->getPosition() += delta;

		}
	}

	bool MovementSystem::isEntityValid(EntityBasePtr pEntity)const {
		return (nullptr != pEntity && pEntity->hasComponent(PositionComponent2D::identification));
	}

	bool MovementSystem::isEntityRegistered(EntityBasePtr entity)const {
		bool result = false;
		for (EntityBasePtr pEntity : m_entities) {
			if (nullptr == pEntity) continue;
			if (pEntity->getEntityId() == entity->getEntityId()) {
				result = true;
				break;
			}
		}
		return result;
	}

	bool MovementSystem::addEntity(EntityBasePtr pEntity) {
		bool result = false;
		if (!isEntityRegistered(pEntity)) {
			m_entities.push_back(pEntity);
		}
		return result;
	}

}