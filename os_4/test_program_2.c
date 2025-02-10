#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main() {
    // Открываем библиотеки
    void *integral_handle = dlopen("./libintegral.so", RTLD_LAZY);
    void *derivative_handle = dlopen("./libderivative.so", RTLD_LAZY);

    if (!integral_handle || !derivative_handle) {
        fprintf(stderr, "Error opening libraries: %s\n", dlerror());
        return 1;
    }

    // Определяем типы указателей на функции
    typedef float (*SinIntegralFunc)(float, float, float);
    typedef float (*DerivativeFunc)(float, float);

    // Загружаем символы из библиотек
    SinIntegralFunc SinIntegral = (SinIntegralFunc)dlsym(integral_handle, "SinIntegral");
    DerivativeFunc Derivative = (DerivativeFunc)dlsym(derivative_handle, "Derivative");

    // Проверяем, успешно ли загружены символы
    if (!SinIntegral || !Derivative) {
        fprintf(stderr, "Error loading symbols: %s\n", dlerror());
        dlclose(integral_handle);
        dlclose(derivative_handle);
        return 1;
    }

    float A, B, e, deltaX;
    int command;

    while (1) {
        printf("Enter command (1 for integral, 2 for derivative, 0 to exit): ");
        if (scanf("%d", &command) != 1) {
            fprintf(stderr, "Invalid input\n");
            continue;
        }

        if (command == 0) {
            break;
        } else if (command == 1) {
            printf("Enter A, B, and e: ");
            if (scanf("%f %f %f", &A, &B, &e) != 3) {
                fprintf(stderr, "Invalid input\n");
                continue;
            }
            printf("Integral: %f\n", SinIntegral(A, B, e));
        } else if (command == 2) {
            printf("Enter A and deltaX: ");
            if (scanf("%f %f", &A, &deltaX) != 2) {
                fprintf(stderr, "Invalid input\n");
                continue;
            }
            printf("Derivative: %f\n", Derivative(A, deltaX));
        } else {
            printf("Invalid command\n");
        }
    }

    // Закрываем библиотеки
    dlclose(integral_handle);
    dlclose(derivative_handle);
    return 0;
}
