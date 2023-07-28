#include "pch.h"
#include "MathHelper.h"
namespace MathHelpers {
    int Clamp(int value, int min, int max) {
        return Min(Max(value, min), max);
    }

    float Clamp(float value, float min, float max) {
        return Min(Max(value, min), max);
    }

    int Min(int value1, int value2)
    {
        return value1 < value2 ? value1 : value2;
    }
    float Min(float value1, float value2)
    {
        return value1 < value2 ? value1 : value2;
    }
    int Max(int value1, int value2)
    {
        return value1 > value2 ? value1 : value2;
    }
    float Max(float value1, float value2)
    {
        return value1 > value2 ? value1 : value2;
    }
    int Wrap(int value, int min, int max)
    {
        if (value < 0) return max;
        if (value > max) return 0;
        return value;
    }
}
