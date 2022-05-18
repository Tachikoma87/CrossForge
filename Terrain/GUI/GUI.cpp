#include "GUI.h"
#include "Widget.h"
#include "WidgetBackground.h"
#include "GUIDefaults.h"
#include "Widgets/Form.h"

#include <stdio.h>
#include <iostream>

#include "CForge/Graphics/Shader/SShaderManager.h"

using namespace std; 
using namespace CForge;


void CallbackTestClass::listen(const CallbackObject Msg)
{
    printf("Received Callback from Form %d\n", Msg.FormID);
    for (auto x : Msg.Data)
        printf("%d: %d\n", x.first, *(int*)x.second.pData);
    printf("\n");
}


GUI::GUI(CForge::RenderDevice* renderDevice)
{
    m_renderDevice = renderDevice;
}
GUI::~GUI()
{
    for (auto x : testBG) {
        delete x;
    }
    delete fontFace;
}

void GUI::testInit(CForge::GLWindow* pWin)
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


    m_pWin = pWin; //can be used for the input processing later on
    m_pWin->character()->startListening(this);
    m_pWin->keyboard()->startListening(this);

    
//     BackgroundStyle b;
    callbackTest = CallbackTestClass();
    auto a = new FormWidget(1, this, nullptr);
    registerMouseDragEvent(a);
    a->startListening(&callbackTest);
    a->addOption(1, DATATYPE_INT, U"first input");
    a->addOption(2, DATATYPE_INT, U"second input");
    a->addOption(1, DATATYPE_INT, U"3st input, collides with 1");
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
void GUI::registerMouseDragEvent(BaseWidget* widget)
{
    m_events_mouseDrag.push_back(widget);
}
void GUI::registerKeyPressEvent(BaseWidget* widget)
{
    m_events_keyPress.push_back(widget);
}
void GUI::processEvents()
{
    processMouseEvents(m_pWin->mouse());
//     processKeyboardEvents(m_pWin->keyboard());
}
void GUI::processMouseEvents ( CForge::Mouse* mouse )
{
    mouseEventInfo mouseEvent;
    auto mpos = mouse->position();
    static bool leftHoldDown = false;
    if (mouse->buttonState(CForge::Mouse::BTN_LEFT)) {
        if (!leftHoldDown) {
            //left mouse button was just pressed down
            leftHoldDown = true;

            printf("mouse click at %f  %f\n", mpos[0], mpos[1]);

            //check if anything was clicked and set the focus accordingly
            //click events take precedence over dragable widgets
            if (focusedWidget != nullptr) focusedWidget->focusLost();
            focusedWidget = nullptr;

            //more efficient designs than a simple list could be implemented
            //depending on how many widgets will be registered in total
            for (auto x : m_events_mouseDown) {
                if (x->checkHitbox(mpos)) {
                    if (!focusedWidget) {
                        focusedWidget = x;
                        focusedWidget->focus();
                        focusedClickOffset = mpos - focusedWidget->getPosition();
                    }
                    mouseEvent.adjustedPosition = mpos - focusedClickOffset;
                    x->onClick(mouseEvent);
                }
            }
            if (!focusedWidget) {
                for (auto x : m_events_keyPress) {
                    if (x->checkHitbox(mpos)) {
                        focusedWidget = x;
                        focusedWidget->focus();
                        focusedClickOffset = mpos - focusedWidget->getPosition();
                        break;
                    }
                }
            }
            if (!focusedWidget) {
                for (auto x : m_events_mouseDrag) {
                    if (x->checkHitbox(mpos)) {
                        focusedWidget = x;
                        focusedWidget->focus();
                        focusedClickOffset = mpos - focusedWidget->getPosition();
                        break;
                    }
                }
            }
        } else {
            //hold down
            if (focusedWidget != nullptr) {
//               if (focusedWidget->checkHitbox(mpos))
                mouseEvent.adjustedPosition = mpos - focusedClickOffset;
                focusedWidget->onDrag(mouseEvent);
            }
        }
    } else if (leftHoldDown) {
        //mouse button released
        leftHoldDown = false;
    }
}
void GUI::processKeyboardEvents(CForge::Keyboard* keyboard)
{
    //Dummy (original purpose didn't work out)
}


void GUI::listen(char32_t codepoint)
{
    //print characters and keycodes for testing purposes
    std::cout << "Received " << codepoint << std::endl;

    //pass the character to the focused widget.
    if (focusedWidget != nullptr) focusedWidget->onKeyPress(codepoint);
}
void GUI::listen(CForge::KeyboardCallback kc)
{
    //Unfortunately, the text character callback does not include backspace, enter or similar keys.
    if (kc.state == CForge::Keyboard::KEY_PRESSED) {
        if (kc.key == CForge::Keyboard::KEY_BACKSPACE)
            listen(U'\b');
        if (kc.key == CForge::Keyboard::KEY_ENTER)
            listen(U'\n');
    }
}

