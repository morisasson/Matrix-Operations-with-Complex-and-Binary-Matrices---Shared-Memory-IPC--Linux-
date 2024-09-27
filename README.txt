208067587
Matrix Operations and Shared Memory Programs
by Mori Sason

Program Overview
This project contains three main programs for matrix operations, including complex and binary matrices. The programs handle matrix addition, subtraction, multiplication, transpose, and logical operations such as AND, OR, and NOT.
The programs are designed to be run as standalone command-line tools or as a client-server system using shared memory for inter-process communication.

program1: 
Matrix Operations Program
This program reads matrices and operations from standard input, performs the specified operations, and prints the results

Functions Overview:
parse_matrix(const char* input, int* rows, int* cols, double complex** matrix): Parses the matrix from the input string.
print_matrix(int rows, int cols, double complex* matrix, int both_complex): Prints the matrix with correct formatting.
is_binary_matrix(int rows, int cols, double complex* matrix): Checks if the matrix contains only binary values.
is_complex_matrix(int rows, int cols, double complex* matrix): Checks if the matrix contains complex numbers.
matrix_add, matrix_sub, matrix_mul, matrix_transpose, matrix_not, matrix_and, matrix_or: Functions to perform respective matrix operations.

How to Run:
Open your terminal and navigate to the directory containing `program1.c` and `run_me_1.sh`. 
Then, run the following command:
 ./run_me_1.sh

program2A and program2B: 
Client-Server Programs Using Shared Memory
These programs use shared memory for inter-process communication. Program 2a acts as the client, reading input and writing it to shared memory, while Program 2b acts as the server, reading from shared memory, performing the operations, and printing the results.

Functions Overview:
parse_matrix(const char* input, int* rows, int* cols, double complex** matrix): Parses the matrix from the input string.
is_binary_matrix, is_complex_matrix: Utility functions to check matrix properties.
print_matrix, matrix_add, matrix_sub, matrix_mul, matrix_transpose, matrix_not, matrix_and, matrix_or: Functions to perform respective matrix operations and print results.
is_binary_matrix, is_complex_matrix: Utility functions to check matrix properties.

How to Run:
Open your terminal and navigate to the directory containing `run_me_2a.sh` and `run_me_2b.sh` and `program2A.c` and `program2B.c`. 
Then, run the following command:
 ./run_me_2a.sh
 ./run_me_2b.sh


program3: 
Matrix Operations with Multithreading
This program reads matrices and operations from standard input, performs the operations using multiple threads, and prints the results.

Functions Overview:
parse_matrix, print_matrix, is_binary_matrix, matrix_add, matrix_sub, matrix_mul, matrix_and, matrix_or: Same as above.
thread_function: Thread function to perform the specified matrix operation.

How to Run:
Open your terminal and navigate to the directory containing `program3.c` and `run_me_3.sh`. 
Then, run the following command:
 ./run_me_3.sh

License
-------

This project is licensed under the MIT License. See the LICENSE file for more details.


Author
------

Mori Sasson  
LinkedIn: https://www.linkedin.com/in/mori-sason-9a4811281  
Email: 8mori8@gmail.com


