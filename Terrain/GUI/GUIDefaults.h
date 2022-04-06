#pragma once

#include <string>

struct BackgroundStyle {
    float alpha = 0.5f;
    float color[3] = {1.0f, 1.0f, 0.0f};
};

struct FontStyle {
    std::string FileName = "Assets/DejaVuSans.ttf"; //Font file to load
    int PixelSize = 40; //height of text in pixels
};
