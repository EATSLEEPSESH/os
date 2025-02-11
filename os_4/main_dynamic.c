#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

// Типы функций из библиотеки
typedef float (*DerivativeFunc)(float, float);
typedef int (*GCFFunc)(int, int);

int main() {
    void *handle;
    DerivativeFunc Derivative1, Derivative2;
    GCFFunc GCF1, GCF2;
    char *error;
    int use_derivative1 = 1; // Флаг для переключения между реализациями
    int use_gcf1 = 1; // Флаг для переключения между реализациями

    // Загрузка библиотеки
    handle = dlopen("./libmylib.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Error loading library: %s\n", dlerror());
        return 1;
    }

    // Получение указателей на функции
    *(void **)(&Derivative1) = dlsym(handle, "Derivative1");
    *(void **)(&Derivative2) = dlsym(handle, "Derivative2");
    *(void **)(&GCF1) = dlsym(handle, "GCF1");
    *(void **)(&GCF2) = dlsym(handle, "GCF2");

    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "Error getting symbols: %s\n", error);
        dlclose(handle);
        return 1;
    }

    char command[256];
    printf("Enter command (1 for Derivative, 2 for GCF, 0 to switch implementation): ");

    while (1) {
        scanf("%s", command);

        if (command[0] == '1') {
            float A, deltaX;
            printf("Enter A and deltaX: ");
            scanf("%f %f", &A, &deltaX);
            if (use_derivative1) {
                printf("Derivative1: %f\n", Derivative1(A, deltaX));
            } else {
                printf("Derivative2: %f\n", Derivative2(A, deltaX));
            }
        } else if (command[0] == '2') {
            int A, B;
            printf("Enter A and B: ");
            scanf("%d %d", &A, &B);
            if (use_gcf1) {
                printf("GCF1: %d\n", GCF1(A, B));
            } else {
                printf("GCF2: %d\n", GCF2(A, B));
            }
        } else if (command[0] == '0') {
            use_derivative1 = !use_derivative1;
            use_gcf1 = !use_gcf1;
            printf("Switched implementation!\n");
        } else {
            printf("Unknown command. Try again.\n");
        }

        printf("Enter command (1 for Derivative, 2 for GCF, 0 to switch implementation): ");
    }

    // Освобождение библиотеки
    dlclose(handle);
    return 0;
}
