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
    for (auto x : Msg.Data) {
        switch(x.second.Type) {
            case DATATYPE_INT:
                printf("%d: %d\n", x.first, *(int*)x.second.pData);
                break;
            case DATATYPE_BOOLEAN:
                printf("%d: %s\n", x.first, *(bool*)x.second.pData ? "True" : "False");
                break;
            case DATATYPE_STRING:
                printf("%d: ", x.first);
                for (auto x : *(u32string*)x.second.pData) {
                    if (x < 127) printf("%c", (char)x);
                    else printf("[%X]", x);
                }
                printf("\n");
                break;
            default:
                printf("%d: unhandled data type\n", x.first);
        }
    }
    printf("\n");
}


GUI::GUI()
{

}
GUI::~GUI()
{
    for (auto x : m_TopLevelWidgets) {
        delete x->pWidget;
        delete x;
    }
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

    m_pWin = pWin; //can be used for the input processing later on
    m_pWin->character()->startListening(this);
    m_pWin->keyboard()->startListening(this);

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

void GUI::render(CForge::RenderDevice* renderDevice)
{
    //blending required as the text will be applied as an alpha mask
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //to allow overlapping widget backgrounds
    glDisable(GL_DEPTH_TEST);

    for (auto x : m_TopLevelWidgets) {
        x->pWidget->draw(renderDevice);
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
    submitTopLevelWidget(window);
    //try to not have them overlap by default
    float x = 0;
    float y = 0;
    float h = 0;
    for (int i = 0; i < m_TopLevelWidgets.size() - 1; i++) {
        x += m_TopLevelWidgets[i]->pWidget->getWidth();
        h = std::max(h, m_TopLevelWidgets[i]->pWidget->getWidth());
        if (x > m_pWin->width()) {
            x = 0;
            y += h;
            h = 0;
        };
    }
    window->setPosition(x, y);
    return form;
}
TextWidget * GUI::createPlainText()
{
    TextWidget* text = new TextWidget(this, nullptr);
    submitTopLevelWidget(text);
    return text;
}

void GUI::submitTopLevelWidget(BaseWidget* widget)
{
    //events can lead to the widget to be registered as top level widget
    //before it is even fully initialised. Thus we need to check if it's
    //not already there before submitting again.
    for (auto x : m_TopLevelWidgets) {
        if (x->pWidget == widget) {
            return;
        }
    }
    m_TopLevelWidgets.push_back(new TopLevelWidgetHandler(widget));
}

void GUI::registerEvent(BaseWidget* widget, GUI::EventType et)
{
    BaseWidget* top = widget->getTopWidget();
    //since child widgets might register themselves before the top level widget
    //is fully initialised, we need to first add them to the list if they're
    //not already in there
    submitTopLevelWidget(top);

    //Find the right top level widget to register the event to
    auto tlw = m_TopLevelWidgets.begin();
    while (tlw != m_TopLevelWidgets.end()) {
        if ((*tlw)->pWidget == top) {
            break;
        }
        tlw++;
    }

    //get the correct list for the event type
    vector<BaseWidget*>* list;
    switch (et) {
        case EVENT_CLICK:
            list = &(*tlw)->eventsMouseDown;
            break;
        case EVENT_DRAG:
            list = &(*tlw)->eventsMouseDrag;
            break;
        case EVENT_KEYPRESS:
            list = &(*tlw)->eventsKeyPress;
            break;
    }

    //insert elements with higher level (deeper within the gui scene graph)
    //at the front so the don't get covered up by larger Widgets in which
    //they might be wrapped.
    int level = widget->getLevel();
    vector<BaseWidget*>::iterator it = list->begin();
    while (it != list->end()) {
        if ((*it)->getLevel() < level) {
            list->insert(it, widget);
            return;
        }
        it++;
    }
    //if all registered elements were of higher level, add the new one to the back
    list->push_back(widget);
    return;
}
void GUI::registerMouseDownEvent ( BaseWidget* widget )
{
    registerEvent(widget, EVENT_CLICK);
}
void GUI::registerMouseDragEvent(BaseWidget* widget)
{
    registerEvent(widget, EVENT_DRAG);
}
void GUI::registerKeyPressEvent(BaseWidget* widget)
{
    registerEvent(widget, EVENT_KEYPRESS);
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

            //check if anything was clicked and set the focus accordingly
            //click events take precedence over dragable widgets
            if (focusedWidget != nullptr) focusedWidget->focusLost();
            focusedWidget = nullptr;

            //First, check which top level widget the click landed in
            auto tlw = m_TopLevelWidgets.rbegin();
            while (tlw != m_TopLevelWidgets.rend()) {
                if ((*tlw)->pWidget->checkHitbox(mpos)) {
                    break;
                }
                tlw++;
            }
            TopLevelWidgetHandler* topHandle;
            if (tlw == m_TopLevelWidgets.rend()) {
                //click was not within any window
                return;
            } else {
                //pull it to the front
                topHandle = *tlw;
                m_TopLevelWidgets.erase(tlw.base() - 1); //-1 because of specifics with reverse iterators
                m_TopLevelWidgets.push_back(topHandle);
            }

            //check the registered events
            for (auto x : topHandle->eventsMouseDown) {
                if (x->checkHitbox(mpos)) {
                    if (!focusedWidget) {
                        focusedWidget = x;
                        focusedWidget->focus();
                        focusedClickOffset = mpos - focusedWidget->getPosition();
                    }
                    mouseEvent.adjustedPosition = mpos - focusedClickOffset;
                    x->onClick(mouseEvent);
                    break;
                }
            }
            if (!focusedWidget) {
                for (auto x : topHandle->eventsKeyPress) {
                    if (x->checkHitbox(mpos)) {
                        focusedWidget = x;
                        focusedWidget->focus();
                        focusedClickOffset = mpos - focusedWidget->getPosition();
                        break;
                    }
                }
            }
            if (!focusedWidget) {
                for (auto x : topHandle->eventsMouseDrag) {
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
//     std::cout << "Received " << (uint32_t)codepoint << std::endl;

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
        if (kc.key == CForge::Keyboard::KEY_R && m_pWin->keyboard()->keyState(CForge::Keyboard::KEY_RIGHT_CONTROL))
            for (auto x : m_TopLevelWidgets) x->pWidget->setPosition(0, 0);
    }
}
uint32_t GUI::getWindowHeight()
{
    return m_pWin->height();
}
uint32_t GUI::getWindowWidth()
{
    return m_pWin->width();
}

