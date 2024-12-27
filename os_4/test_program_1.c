#include <stdio.h>
#include <stdlib.h>
#include "integral.h"
#include "derivative.h"

int main() {
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

    return 0;
}
