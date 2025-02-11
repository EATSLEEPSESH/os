#include "mylib.h"
#include <math.h>

// Реализация функции Derivative1
float Derivative1(float A, float deltaX) {
    return (sin(A + deltaX) - sin(A)) / deltaX;
}

// Реализация функции Derivative2
float Derivative2(float A, float deltaX) {
    return (sin(A + deltaX) - sin(A - deltaX)) / (2 * deltaX);
}

// Реализация функции GCF1 (Алгоритм Евклида)
int GCF1(int A, int B) {
    while (B != 0) {
        int temp = B;
        B = A % B;
        A = temp;
    }
    return A;
}

// Реализация функции GCF2 (Наивный алгоритм)
int GCF2(int A, int B) {
    int min = A < B ? A : B;
    for (int i = min; i > 0; i--) {
        if (A % i == 0 && B % i == 0) {
            return i;
        }
    }
    return 1;
}
