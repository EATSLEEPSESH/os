#include <stdio.h>
#include <stdlib.h>
#include "mylib.h"

int main() {
    char command[256];
    printf("Enter command (1 for Derivative, 2 for GCF): ");

    while (1) {
        scanf("%s", command);

        if (command[0] == '1') {
            float A, deltaX;
            printf("Enter A and deltaX: ");
            scanf("%f %f", &A, &deltaX);
            printf("Derivative1: %f\n", Derivative1(A, deltaX));
            printf("Derivative2: %f\n", Derivative2(A, deltaX));
        } else if (command[0] == '2') {
            int A, B;
            printf("Enter A and B: ");
            scanf("%d %d", &A, &B);
            printf("GCF1: %d\n", GCF1(A, B));
            printf("GCF2: %d\n", GCF2(A, B));
        } else if (command[0] == '0') {
            printf("Exiting...\n");
            break;
        } else {
            printf("Unknown command. Try again.\n");
        }

        printf("Enter command (1 for Derivative, 2 for GCF, 0 to exit): ");
    }

    return 0;
}
