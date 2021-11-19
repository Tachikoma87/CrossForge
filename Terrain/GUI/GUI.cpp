#include "GUI.h"
#include "WidgetBackground.h"
#include "GUIDefaults.h"

#include "CForge/Graphics/Shader/SShaderManager.h"

using namespace std; 
using namespace CForge;

GUI::GUI(CForge::RenderDevice* renderDevice)
{
    m_renderDevice = renderDevice;
}
GUI::~GUI()
{
    for (auto x : testBG) {
        delete x;
    }
}

void GUI::testInit()
{
    WidgetBackgroundColored* a = new WidgetBackgroundColored();
    SShaderManager* shaderManager = SShaderManager::instance();

    vector<ShaderCode*> vsSources;
    vector<ShaderCode*> fsSources;
    string errorLog;
    
    ShaderCode* vertexShader =
        shaderManager->createShaderCode("Shader/ScreenQuad.vert", "330 core",
                                        0, "", "");
    ShaderCode* fragmentShader =
        shaderManager->createShaderCode("Shader/ScreenQuad2.frag", "330 core",
                                        0, "", "");

    vsSources.push_back(vertexShader);
    fsSources.push_back(fragmentShader);

    auto shader = shaderManager->buildShader(&vsSources, &fsSources, &errorLog);
    
    shaderManager->release();
    
    BackgroundStyle b;
    a->init(b, shader);
    testBG.push_back(a);
}
void GUI::testRender()
{
    for (auto x : testBG) {
        x->render(m_renderDevice);
    }
}

