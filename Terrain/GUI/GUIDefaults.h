/*****************************************************************************\
*                                                                           *
* File(s): GUIDefaults.h                                      *
*                                                                           *
* Content:    *
*          .                                         *
*                                                                           *
*                                                                           *
* Author(s): Simon Kretzschmar, Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_GUIDEFAULTS_H__
#define __CFORGE_GUIDEFAULTS_H__

#include <string>

namespace CForge {

    struct BackgroundStyle {
        float Alpha = 0.7f;
        float Color[3] = { 0.0f, 0.0f, 0.0f };
    };
    struct BorderStyle {
        float LineWidth = 1.0f;
        float Color[3] = { 1.0f, 1.0f, 1.0f };
        float Alpha = 1.0f;
    };


    //Probably would've been easier to just do in the GUI class where it's used
    //But I preferred to have it all together in this file

    //NOTE: If you add any new Fonts, you also need to load the respective
    //      FontStyle in GUI::loadFonts()

    struct FontStyle {
        std::string FileName; //Font file to load
        int PixelSize; //height of text in pixels
        float FontColor[3] = { 1.0f, 1.0f, 1.0f };
        float FontColorHighlight[3] = { 1.0f, 1.0f, 0.5f };
    };

    struct FontStyle1 : FontStyle {         //the default font, used for almost all default widgets
        FontStyle1() {
            FileName = "Assets/DejaVuSans.ttf";
            PixelSize = 16;
        }
    };
    struct FontStyle2 : FontStyle {         //bold variant
        FontStyle2() {
            FileName = "Assets/DejaVuSans-Bold.ttf";
            PixelSize = 16;
        }
    };

    /* The characters that should be loaded into the glyph map texture.
     * Characters not present in this list will not display correctly (usually as □).
     * The provided string covers all printable characters in the ascii and latin1
     * unicode ranges. Feel free to append any other characters you may need.
     *
     * Note that even if a character is not included and won't display, it will still
     * correctly be read as input.
     */
    const std::u32string CharactersToLoadIntoTexture = U" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~¡¢£¤¥¦§¨©ª«¬®¯°±²³´µ¶·¸¹º»¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ▾";

    struct WidgetStyle {
        int WithinWidgetPadding = 5;
        int TextPadding = 3;
        int TextLineGap = 0;    //padding between two subsequent lines in a multiline text widget
    };
    struct InputTextStyle {
        float WrongInputColor[3] = { 1.0f, 0.5f, 0.5f };
        float ValidInputColor[3] = { 0.5f, 1.0f, 0.5f };
    };
    struct InputSliderStyle {
        float CursorWidth = 0;
        float CursorBorder = 3.0f;
        float Width = 100;
        float Height = 10;
        int RoundLabelNumber = 2;   //round the number text to this many decimals after the comma. Does  NOT affect return value! Negative value disables rounding.
    };

}//name space
#endif