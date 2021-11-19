#pragma once

#include "WidgetBackground.h"
#include <CForge/Graphics/RenderDevice.h>
#include <vector>

class GUI {
public:
    GUI(CForge::RenderDevice* renderDevice);
    ~GUI();
    void testInit();
    void testRender();
private:
    std::vector<WidgetBackground*> testBG;
    CForge::RenderDevice* m_renderDevice;
};
