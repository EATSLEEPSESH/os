#include <math.h>

float SinIntegral(float A, float B, float e) {
    float integral = 0.0;
    float x = A;
    while (x < B) {
        integral += (sin(x) + sin(x + e)) / 2.0 * e;
        x += e;
    }
    return integral;
}
