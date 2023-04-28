#include "GUI.h"
#include "Widget.h"
#include "WidgetBackground.h"
#include "GUIDefaults.h"
#include "Widgets/Form.h"
#include "Widgets/Window.h"

#include <stdio.h>
#include <iostream>

#include <CForge/Graphics/OpenGLHeader.h>
#include <CForge/Graphics/Shader/SShaderManager.h>

using namespace std; 

namespace CForge {

    void CallbackTestClass::listen(const GUICallbackObject Msg)
    {
        printf("Received Callback from Form %d\n", Msg.FormID);
        for (auto x : Msg.Data) {
            switch (x.second.Type) {
            case INPUTTYPE_INT:
            case INPUTTYPE_DROPDOWN:
                printf("%d: %d\n", x.first, *(int*)x.second.pData);
                break;
            case INPUTTYPE_BOOL:
                printf("%d: %s\n", x.first, *(bool*)x.second.pData ? "True" : "False");
                break;
            case INPUTTYPE_STRING:
                printf("%d: ", x.first);
                for (auto x : *(u32string*)x.second.pData) {
                    if (x < 127) printf("%c", uint32_t(x));
                    else printf("[%X]", uint32_t(x));
                }
                printf("\n");
                break;
            case INPUTTYPE_RANGESLIDER:
                printf("%d: %f\n", x.first, *(float*)x.second.pData);
                break;
            default:
                printf("%d: unhandled data type\n", x.first);
            }
        }
        printf("\n");
    }


    GUI::GUI()
    {
        library = nullptr;
    }
    GUI::~GUI()
    {
        for (auto x : m_TopLevelWidgets) {
            delete x->pWidget;
            delete x;
        }
        if (m_Popup != nullptr) {
            delete m_Popup->pWidget;
            delete m_Popup;
        }
        for (auto x : fontFaces) {
            delete x;
        }
        if(nullptr != library) FT_Done_FreeType(library);
    }

    void GUI::init(CForge::GLWindow* pWin)
    {
        //Compile the shaders

        //Text
        SShaderManager* shaderManager = SShaderManager::instance();
        vector<ShaderCode*> vsSources;
        vector<ShaderCode*> fsSources;
        string errorLog;
#ifdef SHADER_GLES
        ShaderCode* vertexShader = shaderManager->createShaderCode("Shader/text.vert", "300 es", 0, "medium float");
        ShaderCode* fragmentShader = shaderManager->createShaderCode("Shader/text.frag", "300 es", 0, "medium float");
#else
        ShaderCode* vertexShader = shaderManager->createShaderCode("Shader/text.vert", "330 core", 0, "");
        ShaderCode* fragmentShader = shaderManager->createShaderCode("Shader/text.frag", "330 core", 0, "");
#endif

        vsSources.push_back(vertexShader);
        fsSources.push_back(fragmentShader);
        TextShader = shaderManager->buildShader(&vsSources, &fsSources, &errorLog);
        shaderManager->release();
        //BG (single color)
        shaderManager = SShaderManager::instance();
        vsSources.clear();
        fsSources.clear();
#ifdef SHADER_GLES
        vertexShader = shaderManager->createShaderCode("Shader/BackgroundColored.vert", "300 es", 0, "medium float");
        fragmentShader = shaderManager->createShaderCode("Shader/BackgroundColored.frag", "300 es", 0, "medium float");
#else
        vertexShader = shaderManager->createShaderCode("Shader/BackgroundColored.vert", "330 core", 0, "");
        fragmentShader = shaderManager->createShaderCode("Shader/BackgroundColored.frag", "330 core", 0, "");
#endif
        vsSources.push_back(vertexShader);
        fsSources.push_back(fragmentShader);
        BackgroundColoredShader = shaderManager->buildShader(&vsSources, &fsSources, &errorLog);
        shaderManager->release();

        loadFonts();

        m_pWin = pWin; //can be used for the input processing later on
        //m_pWin->character()->startListening(this);
        m_pWin->keyboard()->startListening(this);

        m_Popup = nullptr;

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
        if (m_Popup != nullptr) {
            m_Popup->pWidget->draw(renderDevice);
        }

        //terrain rendering does not work properly with enabled blending
        glDisable(GL_BLEND);


        glEnable(GL_DEPTH_TEST);
    }

    FormWidget* GUI::createOptionsWindow(std::u32string title, int FormID, std::u32string applyName)
    {
        WindowWidget* window = new WindowWidget(title, this, nullptr);
        FormWidget* form = new FormWidget(FormID, this, window, applyName);
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
    TextWidget* GUI::createPlainText()
    {
        TextWidget* text = new TextWidget(this, nullptr);
        submitTopLevelWidget(text);
        return text;
    }
    TextWidget* GUI::createTextWindow(std::u32string title)
    {
        WindowWidget* window = new WindowWidget(title, this, nullptr);
        TextWidget* text = new TextWidget(this, nullptr);
        window->setContentWidget(text);
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
        return text;
    }

    void GUI::submitTopLevelWidget(BaseWidget* widget)
    {
        //events can lead to the widget to be registered as top level widget
        //before it is even fully initialised. Thus we need to check if it's
        //not already there before submitting again.
        if (m_Popup != nullptr && m_Popup->pWidget == widget) return;
        for (auto x : m_TopLevelWidgets) {
            if (x->pWidget == widget) {
                return;
            }
        }
        m_TopLevelWidgets.push_back(new TopLevelWidgetHandler(widget));
    }
    /*  This function registers a widget as popup. If a popup is registered,
        only its and its child widget's events are processed, no other
        widgets will receive any clicks.
        Note however, that some restrains apply, most notably:
          * The widget must not have any parent widget */
    void GUI::registerWidgetAsPopup(BaseWidget* widget)
    {
        if (m_Popup != nullptr) {
            unregisterPopup();
        }
        //check that it's a top level widget
        if (widget != nullptr && widget->getTopWidget() == widget) {
            //check if it's already registered
            m_Popup = nullptr;
            auto tlw = m_TopLevelWidgets.begin();
            while (tlw != m_TopLevelWidgets.end()) {
                if ((*tlw)->pWidget == widget) {
                    //if it does already exist, reuse its handler and remove it from the list
                    m_Popup = *tlw;
                    m_TopLevelWidgets.erase(tlw);
                    break;
                }
                tlw++;
            }
            if (m_Popup == nullptr) {
                //if it didn't exist, create a new handler
                //note that at the current state of the codebase, this implies that
                //the popup widget in question has NO interactivity whatsoever,
                //which is probably either an accident/bug or a bad idea.
                m_Popup = new TopLevelWidgetHandler(widget);
            }
        }
    }
    /*
        Removes the currently registered popup if any.
        This function deletes both the handler and the widget itself!
        So make sure to not interact with it afterwards.

        If you want to reuse your popup, a less destructive alternative method
        needs to be implemented.
     */
    void GUI::unregisterPopup()
    {
        if (m_Popup != nullptr) {
            if (focusedWidget != nullptr) {
                if (focusedWidget->getTopWidget() == m_Popup->pWidget) {
                    focusedWidget = nullptr;
                }
            }
            delete m_Popup->pWidget;
            delete m_Popup;
            m_Popup = nullptr;
        }
    }

    void GUI::registerEvent(BaseWidget* widget, GUI::EventType et)
    {
        BaseWidget* top = widget->getTopWidget();
        //since child widgets might register themselves before the top level widget
        //is fully initialised, we need to first add them to the list if they're
        //not already in there
        submitTopLevelWidget(top);

        //Find the right top level widget to register the event to
        TopLevelWidgetHandler* tlw = nullptr;
        if (m_Popup != nullptr && m_Popup->pWidget == top) {
            tlw = m_Popup;
        }
        else {
            auto tlw_it = m_TopLevelWidgets.begin();
            while (tlw_it != m_TopLevelWidgets.end()) {
                if ((*tlw_it)->pWidget == top) {
                    tlw = *tlw_it;
                    break;
                }
                tlw_it++;
            }
        }

        //get the correct list for the event type
        vector<BaseWidget*>* list;
        switch (et) {
        case EVENT_CLICK:
            list = &(tlw->eventsMouseDown);
            break;
        case EVENT_DRAG:
            list = &(tlw->eventsMouseDrag);
            break;
        case EVENT_KEYPRESS:
            list = &(tlw->eventsKeyPress);
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
    void GUI::registerMouseDownEvent(BaseWidget* widget)
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
        processKeyboardEvents(m_pWin->keyboard());
    }
    void GUI::processMouseEvents(CForge::Mouse* mouse)
    {
        mouseEventInfo mouseEvent;
        mouseEvent.rawPosition = mouse->position();
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
                TopLevelWidgetHandler* topHandle = nullptr;
                if (m_Popup != nullptr) {
                    //There's a popup, so all input should go to that
                    if (m_Popup->pWidget->checkHitbox(mpos)) {
                        topHandle = m_Popup;
                    }
                    else {
                        //TODO idea for the future:
                        //  instead of making popups interested in all clicks the size of the application window,
                        //  have them inherit a popup interface class and call an "click outside the popup" method
                        return;
                    }
                }
                else {
                    auto tlw = m_TopLevelWidgets.rbegin();
                    while (tlw != m_TopLevelWidgets.rend()) {
                        if ((*tlw)->pWidget->checkHitbox(mpos)) {
                            break;
                        }
                        tlw++;
                    }
                    if (tlw == m_TopLevelWidgets.rend()) {
                        //click was not within any window
                        return;
                    }
                    else {
                        //pull it to the front
                        topHandle = *tlw;
                        m_TopLevelWidgets.erase(tlw.base() - 1); //-1 because of specifics with reverse iterators
                        m_TopLevelWidgets.push_back(topHandle);
                    }
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
            }
            else {
                //hold down
                if (focusedWidget != nullptr) {
                    mouseEvent.adjustedPosition = mpos - focusedClickOffset;
                    focusedWidget->onDrag(mouseEvent);
                }
            }
        }
        else if (leftHoldDown) {
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

            if (kc.key == Keyboard::KEY_UNKNOWN) listen(kc.Unicode);

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

}//name space
