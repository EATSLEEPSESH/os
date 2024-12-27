#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main() {
    void *integral_handle = dlopen("./libintegral.so", RTLD_LAZY);
    void *derivative_handle = dlopen("./libderivative.so", RTLD_LAZY);

    if (!integral_handle || !derivative_handle) {
        fprintf(stderr, "Error opening libraries: %s\n", dlerror());
        return 1;
    }

    typedef float (*SinIntegralFunc)(float, float, float);
    typedef float (*DerivativeFunc)(float, float);

    SinIntegralFunc SinIntegral = (SinIntegralFunc)dlsym(integral_handle, "SinIntegral");
    DerivativeFunc Derivative = (DerivativeFunc)dlsym(derivative_handle, "Derivative");

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
        scanf("%d", &command);

        if (command == 0) {
            break;
        } else if (command == 1) {
            printf("Enter A, B, and e: ");
            scanf("%f %f %f", &A, &B, &e);
            printf("Integral: %f\n", SinIntegral(A, B, e));
        } else if (command == 2) {
            printf("Enter A and deltaX: ");
            scanf("%f %f", &A, &deltaX);
            printf("Derivative: %f\n", Derivative(A, deltaX));
        } else {
            printf("Invalid command\n");
        }
    }

    dlclose(integral_handle);
    dlclose(derivative_handle);
    return 0;
}
