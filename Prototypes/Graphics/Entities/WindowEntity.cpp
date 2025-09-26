#include "WindowEntity.h"

namespace CForge {

	WindowEntity::WindowEntity(): EntityBase(WindowEntity::identification) {
		
	}

	WindowEntity::~WindowEntity() {

	}

	WindowPropertiesComponentPtr WindowEntity::getWindowPropertiesComponent() {
		return getComponent<WindowPropertiesComponent>(WindowPropertiesComponent::identification);
	}

}