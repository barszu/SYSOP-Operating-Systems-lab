#include "zad1.h"

void memory(unsigned int rows, unsigned int cols)
{
    double **matrix;

    /* Zaalokuj pamięć dla macierzy `matrix` zawierającej `rows` x `cols` wartości.
        Macierz będzie adresowana w sposob: matrix[row][col] = 3.14 */

    matrix = (double  **) malloc(sizeof (double *)* rows);

    for (int i = 0; i < rows; i++) {
        matrix[i] = (double *) malloc(cols * sizeof(double));
    }

    check_matrix(matrix, rows, cols);

    /* Zwolnij pamięć macierzy `matrix` */

    for (int i = 0; i < rows; i++){
        free(matrix[i]);
    }

    free(matrix);
}

int main(int arc, char **argv)
{
    srand(42);
    unsigned int cols = rand() % 20;
    unsigned int rows = rand() % 20;
    memory(cols, rows);
    check_memory(cols, rows);
}
