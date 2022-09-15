#pragma once

bool maximumQuality = false;
bool lowQuality = !maximumQuality && false;


float settingSizeScale = (lowQuality ? 1 : (maximumQuality ? 32 : 4));








