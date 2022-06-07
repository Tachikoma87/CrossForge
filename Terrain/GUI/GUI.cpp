#include "GUI.h"
#include "Widget.h"
#include "WidgetBackground.h"
#include "GUIDefaults.h"
#include "Widgets/Form.h"
#include "Widgets/Window.h"

#include <stdio.h>
#include <iostream>

#include <glad/glad.h>
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
    for (auto x : fontFaces) {
        delete x;
    }
    FT_Done_FreeType(library);
}

void GUI::testInit(CForge::GLWindow* pWin)
{
    //Compile the shaders

    //Text
    SShaderManager* shaderManager = SShaderManager::instance();
    vector<ShaderCode*> vsSources;
    vector<ShaderCode*> fsSources;
    string errorLog;
    ShaderCode* vertexShader = shaderManager->createShaderCode("Shader/text.vert", "330 core", 0, "", "");
    ShaderCode* fragmentShader = shaderManager->createShaderCode("Shader/text.frag", "330 core", 0, "", "");
    vsSources.push_back(vertexShader);
    fsSources.push_back(fragmentShader);
    TextShader = shaderManager->buildShader(&vsSources, &fsSources, &errorLog);
    shaderManager->release();
    //BG (single color)
    shaderManager = SShaderManager::instance();
    vsSources.clear();
    fsSources.clear();
    vertexShader = shaderManager->createShaderCode("Shader/BackgroundColored.vert", "330 core", 0, "", "");
    fragmentShader = shaderManager->createShaderCode("Shader/BackgroundColored.frag", "330 core", 0, "", "");
    vsSources.push_back(vertexShader);
    fsSources.push_back(fragmentShader);
    BackgroundColoredShader = shaderManager->buildShader(&vsSources, &fsSources, &errorLog);
    shaderManager->release();
    
    loadFonts();

    //Test rendering
//     testtext.init(U"Beispieltext. ÄäÖöÜüß!?", fontFace, TextShader);


    m_pWin = pWin; //can be used for the input processing later on
    m_pWin->character()->startListening(this);
    m_pWin->keyboard()->startListening(this);

    
//     BackgroundStyle b;
//     callbackTest = CallbackTestClass();
//     auto a = new FormWidget(1, this, nullptr);
//     registerMouseDragEvent(a);
//     a->startListening(&callbackTest);
//     a->addOption(1, DATATYPE_INT, U"first input");
//     a->addOption(2, DATATYPE_INT, U"second input");
//     a->addOption(1, DATATYPE_INT, U"3st input, collides with 1");
//     a->setLimit(1, 5);
//     testBG.push_back(a);
}
void GUI::loadFonts()
{
    //initialise Freetype
    int error;
    error = FT_Init_FreeType(&library);
    if (error) {
        //Error occured loading the library
        //TODO: figure out how to exit the program
        printf("Error loading freetype\n");
    }
    FontStyle1 f1;
    FontFace* font = new FontFace(f1, library);
    fontFaces.push_back(font);
    FontStyle2 f2;
    font = new FontFace(f2, library);
    fontFaces.push_back(font);
}

void GUI::testRender()
{
    //blending required as the text will be applied as an alpha mask
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //to allow overlapping widget backgrounds
    glDisable(GL_DEPTH_TEST);

    for (auto x : testBG) {
        if (x != nullptr) x->draw(m_renderDevice);
    }

    //terrain rendering does not work properly with enabled blending
    glDisable(GL_BLEND);


    glEnable(GL_DEPTH_TEST);
}

FormWidget * GUI::createOptionsWindow(std::u32string title, int FormID)
{
    WindowWidget* window = new WindowWidget(title, this, nullptr);
    FormWidget* form = new FormWidget(FormID, this, window);
    window->setContentWidget(form);
    testBG.push_back(window);
    return form;
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

