//208067587
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_SIZE 128
#define SHM_SIZE 2048
#define SEM_NAME "/my_semaphore"

typedef struct {
    char operation[MAX_SIZE];
    int rows1;
    int cols1;
    double complex matrix1[MAX_SIZE];
    int rows2;
    int cols2;
    double complex matrix2[MAX_SIZE];
} SharedData;

void print_matrix(int rows, int cols, double complex* matrix, int is_complex);
void matrix_add(int rows, int cols, double complex* matrix1, double complex* matrix2, double complex* result);
void matrix_sub(int rows, int cols, double complex* matrix1, double complex* matrix2, double complex* result);
void matrix_mul(int rows, int cols, double complex* matrix1, double complex* matrix2, double complex* result);
void matrix_transpose(int rows, int cols, double complex* matrix, double complex* result);
void matrix_not(int rows, int cols, double complex* matrix, double complex* result);
void matrix_and(int rows, int cols, double complex* matrix1, double complex* matrix2, double complex* result);
void matrix_or(int rows, int cols, double complex* matrix1, double complex* matrix2, double complex* result);
int is_binary_matrix(int rows, int cols, double complex* matrix);
int is_complex_matrix(int rows, int cols, double complex* matrix);

int main() {
    int shmid;
    key_t key;
    if ((key = ftok("/tmp", 'A')) == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    if ((shmid = shmget(key, SHM_SIZE, 0666)) == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    SharedData *shared_data = (SharedData*) shmat(shmid, NULL, 0);
    sem_t* sem;
    if ((sem = sem_open(SEM_NAME, 0)) == SEM_FAILED) {
        perror("sem_open");
    }

    SharedData *cur_ptr = shared_data;
    while (1) {
        sem_wait(sem);
        if (strlen(cur_ptr->operation) == 0) {
            sem_post(sem);
            break;
        }
        char operation[MAX_SIZE];
        strcpy(operation, cur_ptr->operation);
        if (strcmp(operation, "ERR") == 0) {
            printf("ERR\n");
            sem_post(sem);
            continue;
        }

        if (strcmp(operation, "TERMINATE") == 0) {
            sem_post(sem);
            break;
        }

        int rows1 = cur_ptr->rows1;
        int cols1 = cur_ptr->cols1;
        double complex *matrix1 = cur_ptr->matrix1;

        int rows2 = cur_ptr->rows2;
        int cols2 = cur_ptr->cols2;
        double complex *matrix2 = cur_ptr->matrix2;

        int is_complex = is_complex_matrix(rows1, cols1, matrix1) || is_complex_matrix(rows2, cols2, matrix2);

        if ((strcmp(operation, "AND") == 0 || strcmp(operation, "OR") == 0) &&
            (!is_binary_matrix(rows1, cols1, matrix1) || !is_binary_matrix(rows2, cols2, matrix2))) {
            printf("ERR\n");
            strcpy(cur_ptr->operation, "ERR");
            sem_post(sem);
            continue;
        }

        if (strcmp(operation, "NOT") == 0 && !is_binary_matrix(rows1, cols1, matrix1)) {
            printf("ERR\n");
            strcpy(cur_ptr->operation, "ERR");
            sem_post(sem);
            continue;
        }

        double complex *result = (double complex*) malloc(rows1 * cols1 * sizeof(double complex));

        if (strcmp(operation, "ADD") == 0) {
            matrix_add(rows1, cols1, matrix1, matrix2, result);
        } else if (strcmp(operation, "SUB") == 0) {
            matrix_sub(rows1, cols1, matrix1, matrix2, result);
        } else if (strcmp(operation, "MUL") == 0) {
            matrix_mul(rows1, cols1, matrix1, matrix2, result);
        } else if (strcmp(operation, "AND") == 0) {
            matrix_and(rows1, cols1, matrix1, matrix2, result);
        } else if (strcmp(operation, "OR") == 0) {
            matrix_or(rows1, cols1, matrix1, matrix2, result);
        } else if (strcmp(operation, "NOT") == 0) {
            matrix_not(rows1, cols1, matrix1, result);
        } else if (strcmp(operation, "TRANSPOSE") == 0) {
            matrix_transpose(rows1, cols1, matrix1, result);
        } else {
            printf("ERR\n");
            sem_post(sem);
            free(result);
            continue;
        }

        print_matrix(rows1, cols1, result, is_complex);

        free(result);

        cur_ptr->operation[0] = '\0';

        sem_post(sem);

        cur_ptr++;
    }

    sem_close(sem);
    shmdt(shared_data);

    return 0;
}

void print_matrix(int rows, int cols, double complex* matrix, int is_complex) {
    printf("(%d,%d:", rows, cols);
    for (int i = 0; i < rows * cols; i++) {
        double real = creal(matrix[i]);
        double imag = cimag(matrix[i]);
        if (imag == 0 && !is_complex) {
            if (real == (int)real) {
                printf("%d", (int)real);
            } else {
                printf("%.1f", real);
            }
        } else if (imag == 0 && is_complex) {
            if (real == (int)real) {
                printf("%d+0i", (int)real);
            } else {
                printf("%.1f+0i", real);
            }
        } else if (real == 0) {
            if (imag == (int)imag) {
                printf("%di", (int)imag);
            } else {
                printf("%.1fi", imag);
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

void matrix_add(int rows, int cols, double complex* matrix1, double complex* matrix2, double complex* result) {
    for (int i = 0; i < rows * cols; i++) {
        result[i] = matrix1[i] + matrix2[i];
    }
}

void matrix_sub(int rows, int cols, double complex* matrix1, double complex* matrix2, double complex* result) {
    for (int i = 0; i < rows * cols; i++) {
        result[i] = matrix1[i] - matrix2[i];
    }
}

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

void matrix_transpose(int rows, int cols, double complex* matrix, double complex* result) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[j * rows + i] = matrix[i * cols + j];
        }
    }
}

void matrix_not(int rows, int cols, double complex* matrix, double complex* result) {
    for (int i = 0; i < rows * cols; i++) {
        result[i] = (creal(matrix[i]) == 0) ? 1 : 0;
    }
}

void matrix_and(int rows, int cols, double complex* matrix1, double complex* matrix2, double complex* result) {
    for (int i = 0; i < rows * cols; i++) {
        result[i] = (creal(matrix1[i]) == 1 && creal(matrix2[i]) == 1) ? 1 : 0;
    }
}

void matrix_or(int rows, int cols, double complex* matrix1, double complex* matrix2, double complex* result) {
    for (int i = 0; i < rows * cols; i++) {
        result[i] = (creal(matrix1[i]) == 1 || creal(matrix2[i]) == 1) ? 1 : 0;
    }
}

int is_binary_matrix(int rows, int cols, double complex* matrix) {
    for (int i = 0; i < rows * cols; i++) {
        if (creal(matrix[i]) != 0 && creal(matrix[i]) != 1) return 0;
        if (cimag(matrix[i]) != 0) return 0;
    }
    return 1;
}

int is_complex_matrix(int rows, int cols, double complex* matrix) {
    for (int i = 0; i < rows * cols; i++) {
        if (cimag(matrix[i]) != 0) return 1;
    }
    return 0;
}
