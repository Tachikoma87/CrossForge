#include "MovementSystem.h"
#include "PositionComponent2D.h"
#include <crossforge/Utility/CForgeUtility.h>
#include <crossforge/Math/CForgeMath.h>

namespace CForge {

	MovementSystem::MovementSystem(): SystemBase(MovementSystem::identification) {

	}

	MovementSystem::~MovementSystem() {

	}

	void MovementSystem::Initialize() {
		Clear();
	}

	void MovementSystem::Clear() {
		m_entities.clear();
	}

	void MovementSystem::Update() {
		for (EntityBasePtr pEntity : m_entities) {
			if (nullptr == pEntity) continue;

			PositionComponent2DPtr position = pEntity->GetComponent<PositionComponent2D>(PositionComponent2D::identification);
			Eigen::Vector2f delta = Eigen::Vector2f::Zero();

			delta.x() = CForgeMath::randRange(-10.0f, 10.0f);
			delta.y() = CForgeMath::randRange(-5.0f, 10.0f);

			position->GetPosition() += delta;

		}
	}

	bool MovementSystem::IsEntityValid(EntityBasePtr pEntity)const {
		return (nullptr != pEntity && pEntity->HasComponent(PositionComponent2D::identification));
	}

	bool MovementSystem::IsEntityRegistered(EntityBasePtr entity)const {
		bool result = false;
		for (EntityBasePtr pEntity : m_entities) {
			if (nullptr == pEntity) continue;
			if (pEntity->GetEntityId() == entity->GetEntityId()) {
				result = true;
				break;
			}
		}
		return result;
	}

	bool MovementSystem::AddEntity(EntityBasePtr pEntity) {
		bool result = false;
		if (!IsEntityRegistered(pEntity)) {
			m_entities.push_back(pEntity);
		}
		return result;
	}

}