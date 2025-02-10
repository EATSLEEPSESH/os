#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#define MAX_THREADS 10

typedef struct {
    int n;
    double **matrix;
    double *result;
    int thread_id;
    int start_row;
    int end_row;
} ThreadData;

void *gauss_elimination(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    int n = data->n;
    double **matrix = data->matrix;
    double *result = data->result;
    int start_row = data->start_row;
    int end_row = data->end_row;

    for (int i = start_row; i < end_row; i++) {
        for (int j = i + 1; j < n; j++) {
            double factor = matrix[j][i] / matrix[i][i];
            for (int k = i; k < n; k++) {
                matrix[j][k] -= factor * matrix[i][k];
            }
            result[j] -= factor * result[i];
        }
    }

    pthread_exit(NULL);
}

void print_matrix(double **matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%lf ", matrix[i][j]);
        }
        printf("\n");
    }
}

void print_result(double *result, int n) {
    for (int i = 0; i < n; i++) {
        printf("%lf ", result[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number_of_threads>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    if (num_threads <= 0 || num_threads > MAX_THREADS) {
        fprintf(stderr, "Number of threads must be between 1 and %d\n", MAX_THREADS);
        return 1;
    }

    int n;
    printf("Enter the size of the matrix: ");
    scanf("%d", &n);
    srand(time(NULL));
    double **matrix = (double **)malloc(n * sizeof(double *));
    double *result = (double *)malloc(n * sizeof(double));
    for (int i = 0; i < n; i++) {
        matrix[i] = (double *)malloc(n * sizeof(double));
        for (int j = 0; j < n; j++) {
            matrix[i][j] = rand() % 10; // Заполняем матрицу случайными числами
        }
        result[i] = rand() % 10; // Заполняем вектор результатов случайными числами
    }

    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];
    int rows_per_thread = n / num_threads;

    for (int i = 0; i < num_threads; i++) {
        thread_data[i].n = n;
        thread_data[i].matrix = matrix;
        thread_data[i].result = result;
        thread_data[i].thread_id = i;
        thread_data[i].start_row = i * rows_per_thread;
        thread_data[i].end_row = (i == num_threads - 1) ? n : (i + 1) * rows_per_thread;
        pthread_create(&threads[i], NULL, gauss_elimination, (void *)&thread_data[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Back substitution
    for (int i = n - 1; i >= 0; i--) {
        result[i] = result[i] / matrix[i][i];
        for (int j = i - 1; j >= 0; j--) {
            result[j] -= matrix[j][i] * result[i];
        }
    }

    printf("Matrix:\n");
    print_matrix(matrix, n);
    printf("Result:\n");
    print_result(result, n);

    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
    free(result);

    return 0;
}
