#include <math.h>

float SinIntegral(float A, float B, float e) {
    // Пример реализации интеграла синуса
    float result = 0;
    float step = (B - A) / e;
    for (float x = A; x < B; x += step) {
        result += sin(x) * step;
    }
    return result;
}
