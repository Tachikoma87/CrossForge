#include "GUI.h"
#include "Widget.h"
#include "WidgetBackground.h"
#include "GUIDefaults.h"
#include "Widgets/InputNumber.h"

#include <stdio.h>

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
    SShaderManager* shaderManager = SShaderManager::instance();

    vector<ShaderCode*> vsSources;
    vector<ShaderCode*> fsSources;
    string errorLog;
    
//     ShaderCode* vertexShader =
//         shaderManager->createShaderCode("Shader/ScreenQuad.vert", "330 core",
//                                         0, "", "");
//     ShaderCode* fragmentShader =
//         shaderManager->createShaderCode("Shader/ScreenQuad2.frag", "330 core",
//                                         0, "", "");
    ShaderCode* vertexShader =
        shaderManager->createShaderCode("Shader/text.vert", "330 core",
                                        0, "", "");
    ShaderCode* fragmentShader =
        shaderManager->createShaderCode("Shader/text.frag", "330 core",
                                        0, "", "");

    vsSources.push_back(vertexShader);
    fsSources.push_back(fragmentShader);

//     BackgroundColoredShader = shaderManager->buildShader(&vsSources, &fsSources, &errorLog);
    TextShader = shaderManager->buildShader(&vsSources, &fsSources, &errorLog);
    
    shaderManager->release();
    
    fontFace = new FontFace();

    //Test rendering
//     testtext.init(U"Beispieltext. ÄäÖöÜüß!?", fontFace, TextShader);

    
//     BackgroundStyle b;
    auto a = new InputNumber(this, nullptr);
//
    testBG.push_back(a);
}
void GUI::testRender()
{
    for (auto x : testBG) {
        if (x != nullptr) x->draw(m_renderDevice);
    }
//     testtext.render(m_renderDevice);
}
void GUI::registerMouseDownEvent ( BaseWidget* widget )
{
    //simple push for now since it's only one element for testing for now, optimize later
    m_events_mouseDown.push_back(widget);
}
void GUI::processMouseEvents ( CForge::Mouse* mouse )
{
    auto mpos = mouse->position();
    static bool leftHoldDown = false;
    if (mouse->buttonState(CForge::Mouse::BTN_LEFT)) {
        if (!leftHoldDown) {
            //left mouse button was just pressed down
            leftHoldDown = true;
            printf("mouse click at %f  %f\n", mpos[0], mpos[1]);
            //more efficient designs than a simple list could be implemented
            //depending on how many widgets will be registered in total
            for (auto x : m_events_mouseDown) {
                if (x->checkHitbox(mpos))
                    x->onClick(mouse);
            }
        } else {
            //hold down
            /*for (auto x : m_events_mouseDown) {
                if (x->checkHitbox(mpos))
                    x->onClick(mouse);
            }*/
        }
    } else if (leftHoldDown) {
        //mouse button released
        leftHoldDown = false;
    }
}

