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

struct FontStyle {
    std::string FileName = "Assets/DejaVuSans.ttf"; //Font file to load
    int PixelSize = 20; //height of text in pixels
    float FontColor[3] = {1.0f, 1.0f, 1.0f};
    float FontColorHighlight[3] = {1.0f, 1.0f, 0.5f};
};

struct WidgetStyle {
    int WithinWidgetPadding = 5;
};
