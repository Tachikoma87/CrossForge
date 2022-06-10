#pragma once

#include <string>

struct BackgroundStyle {
    float Alpha = 0.5f;
    float Color[3] = {0.0f, 0.0f, 0.0f};
};
struct BorderStyle {
    float LineWidth = 1.0f;
    float Color[3] = {1.0f, 1.0f, 1.0f};
    float Alpha = 1.0f;
};


//Probably would've been easier to just do in the GUI class where it's used
//But I preferred to have it all together in this file

struct FontStyle {
    std::string FileName; //Font file to load
    int PixelSize; //height of text in pixels
    float FontColor[3];
    float FontColorHighlight[3];
};

struct FontStyle1 : FontStyle {         //the default font, used for almost all default widgets
    FontStyle1() {
        FileName = "Assets/DejaVuSans.ttf";
        PixelSize = 16;
        float font_color[3] = {1.0f, 1.0f, 1.0f};
        float font_color_highlight[3] = {1.0f, 1.0f, 0.5f};
        std::copy(font_color, font_color+3, FontColor);  //stupid c++ not allowing direct array assignment
        std::copy(font_color_highlight, font_color_highlight+3, FontColorHighlight);
    }
};
struct FontStyle2 : FontStyle {         //bold variant
    FontStyle2() {
        FileName = "Assets/DejaVuSans-Bold.ttf";
        PixelSize = 16;
        float font_color[3] = {1.0f, 1.0f, 1.0f};
        float font_color_highlight[3] = {1.0f, 1.0f, 0.5f};
        std::copy(font_color, font_color+3, FontColor);
        std::copy(font_color_highlight, font_color_highlight+3, FontColorHighlight);
    }
};

struct WidgetStyle {
    int WithinWidgetPadding = 5;
    int TextPadding = 3;
};
struct InputTextStyle {
    float WrongInputColor[3] = {1.0f, 0.5f, 0.5f};
    float ValidInputColor[3] = {0.5f, 1.0f, 0.5f};
};
