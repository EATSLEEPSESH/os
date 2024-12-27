#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

// Функция для проверки числа на простоту
int is_prime(int num) {
    if (num <= 1) return 0;
    if (num <= 3) return 1;
    if (num % 2 == 0 || num % 3 == 0) return 0;
    for (int i = 5; i * i <= num; i += 6) {
        if (num % i == 0 || num % (i + 2) == 0) return 0;
    }
    return 1;
}

int main() {
    int num;
    FILE *output_file = fopen("output.txt", "w");
    if (output_file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    while (scanf("%d", &num) == 1) {
        if (is_prime(num)) {
            fclose(output_file);
            exit(EXIT_SUCCESS);
        } else {
            fprintf(output_file, "%d\n", num);
        }
    }

    fclose(output_file);
    exit(EXIT_SUCCESS);
}
