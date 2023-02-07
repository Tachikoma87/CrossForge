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
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_GUIDEFAULTS_H__
#define __CFORGE_GUIDEFAULTS_H__

#include <string>

/**
 * \file
 * \brief Default values used across the GUI.
 *
 * This file includes many default values used within the GUI and
 * its widgets. Because no proper dynamic theming is implemented at
 * this point, these definitions can be used to adjust basic aspects
 * of the GUI's visual appearance.
 */

namespace CForge {

    /** \brief The default style for CForge::WidgetBackgroundColored
     *         objects.
     * \ingroup GUI */
    struct BackgroundStyle {
        float Alpha = 0.7f;
        float Color[3] = { 0.0f, 0.0f, 0.0f };
    };
    /** \brief The default style for CForge::WidgetBackgroundBorder
     *         objects.
     * \ingroup GUI */
    struct BorderStyle {
        float LineWidth = 1.0f;
        float Color[3] = { 1.0f, 1.0f, 1.0f };
        float Alpha = 1.0f;
    };


    //Probably would've been easier to just do in the GUI class where it's used
    //But I preferred to have it all together in this file

    //NOTE: If you add any new Fonts, you also need to load the respective
    //      FontStyle in GUI::loadFonts()

    /** \brief Defines information about a single font style.
     *
     * You can add more font styles in a way similar to FontStyle1 or
     * FontStyle2. Note however, that in order for them to be usable by
     * widgets, they need to be manually loaded in CForge::GUI::loadFonts
     * at the current time.
     *
     * \ingroup GUI
     */
    struct FontStyle {
        std::string FileName;                               ///< The font file to load
        int PixelSize;                                      ///< height of text in pixels
        float FontColor[3] = { 1.0f, 1.0f, 1.0f };          ///< Default text color.
        float FontColorHighlight[3] = { 1.0f, 1.0f, 0.5f }; ///< Highlighted text color (eg. active text input).
    };

    /** \brief The default font style used through out most of the default
     *         widgets.
     * \ingroup GUI */
    struct FontStyle1 : FontStyle {         //the default font, used for almost all default widgets
        FontStyle1() {
            FileName = "Assets/DejaVuSans.ttf";
            PixelSize = 16;
        }
    };
    /** \brief Used for bold text in CForge::WindowWidget's title and
     *         CForge::FormWidget's apply button.
     * \ingroup GUI */
    struct FontStyle2 : FontStyle {         //bold variant
        FontStyle2() {
            FileName = "Assets/DejaVuSans-Bold.ttf";
            PixelSize = 16;
        }
    };

    /** \brief The characters that should be loaded into the glyph map texture.
     *
     * Characters not present in this list will not display correctly (usually as □).
     * The provided string covers all printable characters in the ascii and latin1
     * unicode ranges, plus the arrow used for dropdown menus. Feel free to append
     * any other characters you may need.
     *
     * Note that even if a character is not included and won't display, it will still
     * correctly be read as input.
     *
     * \ingroup GUI
     * \sa CForge::FontFace::prepareCharMap
     */
    const std::u32string CharactersToLoadIntoTexture = U" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~¡¢£¤¥¦§¨©ª«¬®¯°±²³´µ¶·¸¹º»¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ▾";

    /** \brief Some default styling values that are used in multiple places.
     * \ingroup GUI */
    struct WidgetStyle {
        int WithinWidgetPadding = 5;    ///< Used for spacing in multiple widgets
        int TextPadding = 3;            ///< Padding around the (multiline) text of a CForge::TextWidget
        int TextLineGap = 0;            ///< padding between two subsequent lines in a multiline text widget
    };

    /** \brief Colours used by the CForge::InputTextWidget.
     * \ingroup GUI */
    struct InputTextStyle {
        float WrongInputColor[3] = { 1.0f, 0.5f, 0.5f };    ///< Used to denote invalid input.
        float ValidInputColor[3] = { 0.5f, 1.0f, 0.5f };    ///< Used to denote valid input.
    };

    /** \brief Some adjustable styling values of the CForge::InputSliderWidget.
     * \ingroup GUI */
    struct InputSliderStyle {
        float CursorWidth = 0;      ///< Width of the value indicator on the slider.
        float CursorBorder = 3.0f;  ///< Border thickness of the slider's value indicator.
        float Width = 100;          ///< Width of the slider.
        float Height = 10;          ///< Height of the slider.
        int RoundLabelNumber = 2;   /**< The display text of the slider widget should be rounded
                                         to this many decimals after the comma. Does  NOT affect
                                         return value! Negative value disables rounding. */
    };

}//name space
#endif
