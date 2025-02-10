#include <math.h>

float Derivative(float A, float deltaX) {
    return (sin(A + deltaX) - sin(A)) / deltaX;
}
