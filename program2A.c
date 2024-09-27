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
#define SHM_SIZE sizeof(SharedData)*16
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

int parse_matrix(const char* input, int* rows, int* cols, double complex** matrix);
int is_binary_matrix(int rows, int cols, double complex* matrix);
int is_complex_matrix(int rows, int cols, double complex* matrix);

int main() {
    //printf("%d\n", (int)sizeof(SharedData));
    int shmid;
    key_t key = ftok("/tmp", 'A');
    if((shmid = shmget(key, SHM_SIZE,  IPC_CREAT | IPC_EXCL | 0600)) == -1){
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    SharedData *shared_data;
    if((shared_data = (SharedData*) shmat(shmid, NULL, 0)) == (SharedData*)-1){
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    sem_t *sem;
    if((sem = sem_open(SEM_NAME, O_CREAT, 0644, 1)) == SEM_FAILED){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
   // sem_post(sem);
    char input1[MAX_SIZE], input2[MAX_SIZE], operation[MAX_SIZE];
    SharedData *cur_ptr = shared_data;

    while (1) {
        // Read first input
        fgets(input1, MAX_SIZE, stdin);
        input1[strcspn(input1, "\n")] = 0;
        sem_wait(sem);
        if (strcmp(input1, "END") == 0) {
            sem_post(sem);
            break;
        }

        int rows1, cols1;
        double complex *matrix1;

        // Check first input
        if (!parse_matrix(input1, &rows1, &cols1, &matrix1)) {
            strcpy(cur_ptr->operation, "ERR");
            sem_post(sem);
            continue;
        }

        // Read second input
        fgets(input2, MAX_SIZE, stdin);
        input2[strcspn(input2, "\n")] = 0;
        if (strcmp(input2, "END") == 0) {
           // strcpy(cur_ptr->operation, "TERMINATE");
            sem_post(sem);
            free(matrix1);
            break;
        }

        int rows2, cols2;
        double complex *matrix2;

        // Check if second input is an operation
        if (strcmp(input2, "NOT") == 0 || strcmp(input2, "TRANSPOSE") == 0) {
            strcpy(operation, input2);

            // For operations that require only one matrix
            if (strcmp(operation, "NOT") == 0 && !is_binary_matrix(rows1, cols1, matrix1)) {
                strcpy(cur_ptr->operation, "ERR");
                free(matrix1);
                sem_post(sem);
                continue;
            }

            cur_ptr->rows1 = rows1;
            cur_ptr->cols1 = cols1;
            memcpy(cur_ptr->matrix1, matrix1, rows1 * cols1 * sizeof(double complex));
            strcpy(cur_ptr->operation, operation);

            sem_post(sem);
        } else {
            // Check if second input is a matrix
            if (!parse_matrix(input2, &rows2, &cols2, &matrix2)) {
                strcpy(cur_ptr->operation, "ERR");
                sem_post(sem);
                free(matrix1);
                continue;
            }

            // Read third input
            fgets(operation, MAX_SIZE, stdin);
            operation[strcspn(operation, "\n")] = 0;
            if (strcmp(operation, "END") == 0) {
                strcpy(cur_ptr->operation, "TERMINATE");
                sem_post(sem);
                free(matrix1);
                free(matrix2);
                break;
            }

            // Check if third input is a valid operation
            if (!(strcmp(operation, "ADD") == 0 || strcmp(operation, "SUB") == 0 ||
                  strcmp(operation, "MUL") == 0 || strcmp(operation, "AND") == 0 ||
                  strcmp(operation, "OR") == 0)) {
                strcpy(cur_ptr->operation, "ERR");
                sem_post(sem);
                free(matrix1);
                free(matrix2);
                continue;
            }

            // For AND and OR operations, check if matrices are binary
            if ((strcmp(operation, "AND") == 0 || strcmp(operation, "OR") == 0) &&
                (!is_binary_matrix(rows1, cols1, matrix1) || !is_binary_matrix(rows2, cols2, matrix2))) {
                strcpy(cur_ptr->operation, "ERR");
                free(matrix1);
                free(matrix2);
                sem_post(sem);
                continue;
            }

            // For operations that require two matrices
            cur_ptr->rows1 = rows1;
            cur_ptr->cols1 = cols1;
            memcpy(cur_ptr->matrix1, matrix1, rows1 * cols1 * sizeof(double complex));

            cur_ptr->rows2 = rows2;
            cur_ptr->cols2 = cols2;
            memcpy(cur_ptr->matrix2, matrix2, rows2 * cols2 * sizeof(double complex));

            strcpy(cur_ptr->operation, operation);

            sem_post(sem);

            free(matrix2);
        }

        free(matrix1);
        cur_ptr++;
    }
    shmdt(shared_data);
    shmctl(shmid, IPC_RMID, NULL);
    sem_close(sem);
    return 0;
}

int parse_matrix(const char* input, int* rows, int* cols, double complex** matrix) {
    if (sscanf(input, "(%d,%d:", rows, cols) != 2) return 0;
    if (*rows <= 0 || *cols <= 0) return 0;

    *matrix = (double complex*) malloc((*rows) * (*cols) * sizeof(double complex));
    const char* values = strchr(input, ':') + 1;
    for (int i = 0; i < (*rows) * (*cols); i++) {
        double real = 0.0, imag = 0.0;
        int items_read = sscanf(values, "%lf+%lfi", &real, &imag);
        if (items_read != 2) {
            items_read = sscanf(values, "%lf-%lfi", &real, &imag);
            if (items_read != 2) {
                items_read = sscanf(values, "%lf", &real);
                imag = 0.0; // Explicitly set imag to 0 if only real part is present
            }
        }
        (*matrix)[i] = real + imag * I;
        values = strchr(values, ',');
        if (values) values++;
    }
    if (values && strchr(values, ')') == NULL) {
        free(*matrix);
        return 0;
    }
    return 1;
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