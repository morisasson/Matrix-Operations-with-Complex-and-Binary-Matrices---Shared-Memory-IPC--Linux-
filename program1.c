//208067587
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>

#define MAX_SIZE 128

// Function to parse the matrix from the input string
int parse_matrix(const char* input, int* rows, int* cols, double complex** matrix) {
    if (sscanf(input, "(%d,%d:", rows, cols) != 2) return 0;

    if (*rows <= 0 || *cols <= 0) return 0;

    // Ensure the matrix is square
    if (*rows != *cols) return 0;

    *matrix = (double complex*) malloc((*rows) * (*cols) * sizeof(double complex));
    if (*matrix == NULL) return 0;

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
void print_matrix(int rows, int cols, double complex* matrix, int both_complex) {
    printf("(%d,%d:", rows, cols);
    for (int i = 0; i < rows * cols; i++) {
        double real = creal(matrix[i]);
        double imag = cimag(matrix[i]);

        if (imag == 0) {
            if (real == 0 && both_complex) {
                printf("0+0i");
            } else if (both_complex) {
                if (real == (int)real) {
                    printf("%d+0i", (int)real);
                } else {
                    printf("%.1f+0i", real);
                }
            } else {
                if (real == (int)real) {
                    printf("%d", (int)real);
                } else {
                    printf("%.1f", real);
                }
            }
        } else if (real == 0 && both_complex) {
            if (imag == (int)imag) {
                printf("%di", (int)imag);
            } else {
                printf("%.1fi", imag);
            }
        } else {
            if (imag > 0) {
                if (real == (int)real && imag == (int)imag) {
                    printf("%d+%di", (int)real, (int)imag);
                } else if (real == (int)real) {
                    printf("%d+%.1fi", (int)real, imag);
                } else if (imag == (int)imag) {
                    printf("%.1f+%di", real, (int)imag);
                } else {
                    printf("%.1f+%.1fi", real, imag);
                }
            } else {
                if (real == (int)real && imag == (int)imag) {
                    printf("%d%di", (int)real, (int)imag);
                } else if (real == (int)real) {
                    printf("%d%.1fi", (int)real, imag);
                } else if (imag == (int)imag) {
                    printf("%.1f%di", real, (int)imag);
                } else {
                    printf("%.1f%.1fi", real, imag);
                }
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

// Function to check if the matrix is complex
int is_complex_matrix(int rows, int cols, double complex* matrix) {
    for (int i = 0; i < rows * cols; i++) {
        if (cimag(matrix[i]) != 0) return 1;
    }
    return 0;
}

// Function to perform matrix addition
void matrix_add(int rows, int cols, double complex* matrix1, double complex* matrix2, double complex* result) {
    for (int i = 0; i < rows * cols; i++) {
        result[i] = matrix1[i] + matrix2[i];
    }
}

// Function to perform matrix subtraction
void matrix_sub(int rows, int cols, double complex* matrix1, double complex* matrix2, double complex* result) {
    for (int i = 0; i < rows * cols; i++) {
        result[i] = matrix1[i] - matrix2[i];
    }
}

// Function to perform matrix multiplication
void matrix_mul(int rows, int cols, double complex* matrix1, double complex* matrix2, double complex* result) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[i * cols + j] = 0;
            for (int k = 0; k < cols; k++) {
                result[i * cols + j] += matrix1[i * cols + k] * matrix2[k * cols + j];
            }
        }
    }
}

// Function to perform matrix transpose
void matrix_transpose(int rows, int cols, double complex* matrix, double complex* result) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[j * rows + i] = matrix[i * cols + j];
        }
    }
}

// Function to perform NOT operation on binary matrix
void matrix_not(int rows, int cols, double complex* matrix, double complex* result) {
    for (int i = 0; i < rows * cols; i++) {
        result[i] = (creal(matrix[i]) == 0) ? 1 : 0;
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

int main() {
    char input1[MAX_SIZE], input2[MAX_SIZE], operation[MAX_SIZE];
    double complex *matrix1 = NULL, *matrix2 = NULL, *result = NULL;
    int rows1, cols1, rows2, cols2;

    while (1) {
        if (fgets(input1, MAX_SIZE, stdin) == NULL) break;
        input1[strcspn(input1, "\n")] = 0;
        if (strcmp(input1, "END") == 0) break;

        if (!parse_matrix(input1, &rows1, &cols1, &matrix1)) {
            printf("ERR\n");
            continue;
        }

        if (fgets(input2, MAX_SIZE, stdin) == NULL) break;
        input2[strcspn(input2, "\n")] = 0;
        if (strcmp(input2, "END") == 0) break;

        if (strcmp(input2, "NOT") == 0 || strcmp(input2, "TRANSPOSE") == 0) {
            strcpy(operation, input2);
            rows2 = rows1;
            cols2 = cols1;
        } else if (!parse_matrix(input2, &rows2, &cols2, &matrix2)) {
            printf("ERR\n");
            free(matrix1);
            continue;
        } else {
            if (fgets(operation, MAX_SIZE, stdin) == NULL) {
                free(matrix1);
                free(matrix2);
                break;
            }
            operation[strcspn(operation, "\n")] = 0;
        }

        if (strcmp(operation, "END") == 0) break;

        if (rows1 != cols1 || rows2 != cols2 || rows1 != rows2) {
            printf("ERR\n");
            free(matrix1);
            if (matrix2) free(matrix2);
            continue;
        }

        result = (double complex*) malloc(rows1 * cols1 * sizeof(double complex));
        if (result == NULL) {
            printf("ERR\n");
            free(matrix1);
            if (matrix2) free(matrix2);
            break;
        }

        int both_complex = is_complex_matrix(rows1, cols1, matrix1) && is_complex_matrix(rows2, cols2, matrix2);

        if (strcmp(operation, "ADD") == 0) {
            matrix_add(rows1, cols1, matrix1, matrix2, result);
            print_matrix(rows1, cols1, result, both_complex);
        } else if (strcmp(operation, "SUB") == 0) {
            matrix_sub(rows1, cols1, matrix1, matrix2, result);
            print_matrix(rows1, cols1, result, both_complex);
        } else if (strcmp(operation, "MUL") == 0) {
            matrix_mul(rows1, cols1, matrix1, matrix2, result);
            print_matrix(rows1, cols1, result, both_complex);
        } else if (strcmp(operation, "AND") == 0) {
            if (is_binary_matrix(rows1, cols1, matrix1) && is_binary_matrix(rows1, cols1, matrix2)) {
                matrix_and(rows1, cols1, matrix1, matrix2, result);
                print_matrix(rows1, cols1, result, 0);
            } else {
                printf("ERR\n");
            }
        } else if (strcmp(operation, "OR") == 0) {
            if (is_binary_matrix(rows1, cols1, matrix1) && is_binary_matrix(rows1, cols1, matrix2)) {
                matrix_or(rows1, cols1, matrix1, matrix2, result);
                print_matrix(rows1, cols1, result, 0);
            } else {
                printf("ERR\n");
            }
        } else if (strcmp(operation, "NOT") == 0) {
            if (is_binary_matrix(rows1, cols1, matrix1)) {
                matrix_not(rows1, cols1, matrix1, result);
                print_matrix(rows1, cols1, result, 0);
            } else {
                printf("ERR\n");
            }
        } else if (strcmp(operation, "TRANSPOSE") == 0) {
            matrix_transpose(rows1, cols1, matrix1, result);
            print_matrix(rows1, cols1, result, both_complex);
        } else {
            printf("ERR\n");
        }

        free(matrix1);
        if (matrix2) free(matrix2);
        free(result);
        matrix1 = matrix2 = result = NULL;
    }

    return 0;
}
