#pragma once

bool maximumQuality = true;
bool lowQuality = !maximumQuality && false;


float settingSizeScale = (lowQuality ? 1 : (maximumQuality ? 16 : 4));






