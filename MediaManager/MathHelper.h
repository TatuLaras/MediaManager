#pragma once

namespace MathHelpers {
    int Clamp(int value, int min, int max);
    float Clamp(float value, float min, float max);

    int Min(int value1, int value2);
    float Min(float value1, float value2);
    int Max(int value1, int value2);
    float Max(float value1, float value2);

    int Repeat(int value, int min, int max);
}
