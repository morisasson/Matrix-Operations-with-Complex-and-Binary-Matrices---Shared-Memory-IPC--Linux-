//208067587
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include <pthread.h>

#define MAX_SIZE 128

typedef struct {
    int rows;
    int cols1;
    int cols2;
    double complex* matrix1;
    double complex* matrix2;
    double complex* result;
    char operation[MAX_SIZE];
} ThreadData;

// Function to parse the matrix from the input string
int parse_matrix(const char* input, int* rows, int* cols, double complex** matrix) {
    if (sscanf(input, "(%d,%d:", rows, cols) != 2) return 0;

    if (*rows <= 0 || *cols <= 0) return 0;

    *matrix = (double complex*) malloc((*rows) * (*cols) * sizeof(double complex));
    const char* values = strchr(input, ':') + 1;
    for (int i = 0; i < (*rows) * (*cols); i++) {
        double real, imag = 0;
        if (sscanf(values, "%lf+%lfi", &real, &imag) == 2) {
            (*matrix)[i] = real + imag * I;
        } else if (sscanf(values, "%lf-%lfi", &real, &imag) == 2) {
            (*matrix)[i] = real - imag * I;
        } else if (sscanf(values, "%lf", &real) == 1) {
            (*matrix)[i] = real + 0 * I;
        } else {
            free(*matrix);
            return 0;
        }
        values = strchr(values, ',');
        if (values) values++;
    }
    if (values && strchr(values, ')') == NULL) {
        free(*matrix);
        return 0;
    }
    return 1;
}

// Function to print the matrix
void print_matrix(int rows, int cols, double complex* matrix) {
    printf("(%d,%d:", rows, cols);
    for (int i = 0; i < rows * cols; i++) {
        double real = creal(matrix[i]);
        double imag = cimag(matrix[i]);
        if (imag == 0) {
            if (real == (int)real) {
                printf("%d", (int)real);
            } else {
                printf("%.1f", real);
            }
        } else if (imag > 0) {
            if (imag == (int)imag && real == (int)real) {
                printf("%d+%di", (int)real, (int)imag);
            } else if (imag == (int)imag) {
                printf("%.1f+%di", real, (int)imag);
            } else if (real == (int)real) {
                printf("%d+%.1fi", (int)real, imag);
            } else {
                printf("%.1f+%.1fi", real, imag);
            }
        } else {
            if (imag == (int)imag && real == (int)real) {
                printf("%d%di", (int)real, (int)imag);
            } else if (imag == (int)imag) {
                printf("%.1f%di", real, (int)imag);
            } else if (real == (int)real) {
                printf("%d%.1fi", (int)real, imag);
            } else {
                printf("%.1f%.1fi", real, imag);
            }
        }
        if (i < rows * cols - 1) {
            printf(",");
        }
    }
    printf(")\n");
}

// Function to check if the matrix is binary
int is_binary_matrix(int rows, int cols, double complex* matrix) {
    for (int i = 0; i < rows * cols; i++) {
        if (creal(matrix[i]) != 0 && creal(matrix[i]) != 1) return 0;
        if (cimag(matrix[i]) != 0) return 0;
    }
    return 1;
}

// Function to perform matrix addition
void matrix_add(int rows, int cols, double complex* matrix1, double complex* matrix2, double complex* result) {
    for (int i = 0; i < rows * cols; i++) {
        result[i] = matrix1[i] + matrix2[i];
    }
}

// Function to perform matrix multiplication
void matrix_mul(int rows, int cols1, int cols2, double complex* matrix1, double complex* matrix2, double complex* result) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols2; j++) {
            result[i * cols2 + j] = 0;
            for (int k = 0; k < cols1; k++) {
                result[i * cols2 + j] += matrix1[i * cols1 + k] * matrix2[k * cols2 + j];
            }
        }
    }
}

// Function to perform AND operation on binary matrices
void matrix_and(int rows, int cols, double complex* matrix1, double complex* matrix2, double complex* result) {
    for (int i = 0; i < rows * cols; i++) {
        result[i] = (creal(matrix1[i]) == 1 && creal(matrix2[i]) == 1) ? 1 : 0;
    }
}

// Function to perform OR operation on binary matrices
void matrix_or(int rows, int cols, double complex* matrix1, double complex* matrix2, double complex* result) {
    for (int i = 0; i < rows * cols; i++) {
        result[i] = (creal(matrix1[i]) == 1 || creal(matrix2[i]) == 1) ? 1 : 0;
    }
}

// Thread function to perform the specified matrix operation
void* thread_function(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    if (strcmp(data->operation, "ADD") == 0) {
        matrix_add(data->rows, data->cols1, data->matrix1, data->matrix2, data->result);
    } else if (strcmp(data->operation, "MUL") == 0) {
        matrix_mul(data->rows, data->cols1, data->cols2, data->matrix1, data->matrix2, data->result);
    } else if (strcmp(data->operation, "AND") == 0) {
        if (is_binary_matrix(data->rows, data->cols1, data->matrix1) && is_binary_matrix(data->rows, data->cols1, data->matrix2)) {
            matrix_and(data->rows, data->cols1, data->matrix1, data->matrix2, data->result);
        } else {
            printf("ERR\n");
        }
    } else if (strcmp(data->operation, "OR") == 0) {
        if (is_binary_matrix(data->rows, data->cols1, data->matrix1) && is_binary_matrix(data->rows, data->cols1, data->matrix2)) {
            matrix_or(data->rows, data->cols1, data->matrix1, data->matrix2, data->result);
        } else {
            printf("ERR\n");
        }
    } else {
        printf("ERR\n");
    }

    return NULL;
}

int main() {
    char input[MAX_SIZE];
    double complex* matrices[MAX_SIZE];
    int rows[MAX_SIZE], cols[MAX_SIZE];
    int matrix_count = 0;
    char operation[MAX_SIZE];

    while (1) {
        if (fgets(input, MAX_SIZE, stdin) == NULL) break;
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "END") == 0) break;

        if (strcmp(input, "ADD") == 0 || strcmp(input, "MUL") == 0 || strcmp(input, "AND") == 0 || strcmp(input, "OR") == 0) {
            strcpy(operation, input);

            // Check if all matrices are binary for AND and OR operations
            if ((strcmp(operation, "AND") == 0 || strcmp(operation, "OR") == 0)) {
                int all_binary = 1;
                for (int i = 0; i < matrix_count; i++) {
                    if (!is_binary_matrix(rows[i], cols[i], matrices[i])) {
                        all_binary = 0;
                        break;
                    }
                }
                if (!all_binary) {
                    printf("ERR\n");
                    for (int i = 0; i < matrix_count; i++) {
                        free(matrices[i]);
                    }
                    matrix_count = 0;
                    continue;
                }
            }

            if (matrix_count % 2 != 0) {
                printf("ERR\n");
                for (int i = 0; i < matrix_count; i++) {
                    free(matrices[i]);
                }
                matrix_count = 0;
                continue;
            }

            pthread_t threads[matrix_count / 2];
            ThreadData thread_data[matrix_count / 2];
            double complex* results[matrix_count / 2];

            for (int i = 0; i < matrix_count / 2; i++) {
                int intermediate_rows = rows[i * 2];
                int intermediate_cols1 = cols[i * 2];
                int intermediate_cols2 = cols[i * 2 + 1];
                results[i] = (double complex*) malloc(intermediate_rows * intermediate_cols2 * sizeof(double complex));
                thread_data[i] = (ThreadData){intermediate_rows, intermediate_cols1, intermediate_cols2, matrices[i * 2], matrices[i * 2 + 1], results[i], ""};
                strcpy(thread_data[i].operation, operation);

                pthread_create(&threads[i], NULL, thread_function, &thread_data[i]);
            }

            for (int i = 0; i < matrix_count / 2; i++) {
                pthread_join(threads[i], NULL);
                if (i == 0) {
                    memcpy(matrices[0], thread_data[i].result, rows[0] * cols[i * 2 + 1] * sizeof(double complex));
                    cols[0] = cols[i * 2 + 1];
                } else {
                    if (strcmp(operation, "ADD") == 0) {
                        matrix_add(thread_data[i].rows, thread_data[i].cols1, matrices[0], thread_data[i].result, matrices[0]);
                    } else if (strcmp(operation, "MUL") == 0) {
                        double complex* new_result = (double complex*) malloc(rows[0] * thread_data[i].cols2 * sizeof(double complex));
                        matrix_mul(thread_data[i].rows, thread_data[i].cols1, thread_data[i].cols2, matrices[0], thread_data[i].result, new_result);
                        memcpy(matrices[0], new_result, rows[0] * thread_data[i].cols2 * sizeof(double complex));
                        cols[0] = thread_data[i].cols2;
                        free(new_result);
                    } else if (strcmp(operation, "AND") == 0) {
                        matrix_and(thread_data[i].rows, thread_data[i].cols1, matrices[0], thread_data[i].result, matrices[0]);
                    } else if (strcmp(operation, "OR") == 0) {
                        matrix_or(thread_data[i].rows, thread_data[i].cols1, matrices[0], thread_data[i].result, matrices[0]);
                    }
                }
                free(results[i]);
            }

            print_matrix(rows[0], cols[0], matrices[0]);

            for (int i = 0; i < matrix_count; i++) {
                free(matrices[i]);
            }
            matrix_count = 0;
        } else {
            if (!parse_matrix(input, &rows[matrix_count], &cols[matrix_count], &matrices[matrix_count])) {
                printf("ERR\n");
                for (int i = 0; i < matrix_count; i++) {
                    free(matrices[i]);
                }
                matrix_count = 0;
            } else {
                matrix_count++;
            }
        }
    }

    return 0;
}
