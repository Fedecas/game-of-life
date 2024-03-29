#ifndef MATRIX_C
#define MATRIX_C

int** create_matrix(int rows, int columns);

void copy_matrix(int** src, int** dst, int rows, int columns);

int** destroy_matrix(int** arr, int rows, int columns);

#endif // !MATRIX_C
