#include <crossforge/core/CoreDefinitions.h>
#include <crossforge/core/SLogger.h>
#include <crossforge/ecs/ComponentBase.h>
#include "Prototypes/AssetIO/Components/MaterialDataComponent.h"
#include <crossforge/utility/GeneralUtility.hpp>
void test() {
	crossforge::LoggerPtr pLogger = crossforge::Logger::instance();

	pLogger->log("test entry");


	crossforge::ComponentBasePtr pCompTest = std::make_shared<crossforge::ComponentBase>(crossforge::ComponentBase::identification);
	pLogger->log(pCompTest->getIdentification());

	CForge::MaterialDataComponentPtr pMatComp = std::make_shared<CForge::MaterialDataComponent>();
	pLogger->log("Material comp: " + pMatComp->getIdentification());

	pLogger->log(crossforge::GeneralUtility::toLowerCase("HelloYouString"));

	throw IndexOutOfBoundsExcept("Test");

	pLogger->destroy();
}