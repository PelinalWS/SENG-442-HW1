#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_SIZE 32
#define NUM_THREADS 4

// Structure to pass parameters to threads
struct ThreadData {
    int* matrix;    // Pointer to the matrix
    int n;          // Size of the matrix
    int column;     // Row to expand along
    int sign;       // Sign multiplier
    int result;     // Partial result (determinant)
};

// Function to calculate determinant of a submatrix
void* detCalc(void* arg) {
    struct ThreadData* data = (struct ThreadData*)arg;
    int* matrix = data->matrix;
    int n = data->n;
    int column = data->column;
    int sign = data->sign;
    int result = 0;

    if (n == 1) {
        // Base case: determinant of 1x1 matrix is the element itself
        result = matrix[0];
    } else {
        // Allocate memory for temporary matrix
        int* temp = malloc((n - 1) * (n - 1) * sizeof(int));
        if (temp == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }

        int temp_row = 0;
        for (int i = 1; i < n; i++) {
            int temp_col = 0;
            for (int j = 0; j < n; j++) {
                if (j != column) {
                    temp[temp_row * (n - 1) + temp_col] = matrix[i * n + j];
                    temp_col++;
                }
            }
            temp_row++;
        }

        // Recursive calculation of submatrix determinants
        for (int j = 0; j < n - 1; j++) {

        }

        free(temp);
    }

    // Store the partial result in the data structure
    data->result = result;

    pthread_exit(NULL);
}

int calc(int *matrix, int row, int column) {
    int n = MAX_SIZE - row;
    int determinant = 0;

    if (n == 1) {
        // Base case: determinant of 1x1 matrix is the element itself
        return matrix[row * MAX_SIZE + column];
    }

    int* temp = malloc((n - 1) * (n - 1) * sizeof(int));
    if (temp == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    int temp_row = 0;
    for (int i = 1; i < n; i++) {
        int temp_col = 0;
        for (int j = 0; j < n; j++) {
            if (j != column) {
                temp[temp_row * (n - 1) + temp_col] = matrix[(i + row) * MAX_SIZE + j];
                temp_col++;
            }
        }
        temp_row++;
    }

    int sign = 1;
    for (int j = 0; j < n - 1; j++) {
        determinant += sign * matrix[row * MAX_SIZE + j] * calc(temp, row + 1, j);
        sign = -sign; // Alternate sign for cofactors
    }

    free(temp);
    return determinant;
}


// Function to calculate determinant using Laplace expansion
int laplaceDet(int* matrix, int n) {
    int determinant = 0;
    struct ThreadData* thread_data[NUM_THREADS];
    pthread_t tid[NUM_THREADS];

    // Create threads to calculate determinants of submatrices
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i] = malloc(sizeof(struct ThreadData));
        thread_data[i]->matrix = matrix;
        thread_data[i]->n = n;
        thread_data[i]->column = i * (n / NUM_THREADS); // Divide columns evenly among threads
        pthread_create(&tid[i], NULL, detCalc, thread_data[i]);
    }

    // Wait for all threads to finish and accumulate partial results
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(tid[i], NULL);
        determinant += thread_data[i]->result;
        free(thread_data[i]);
    }

    return determinant;
}

void writeMatrix();
int* readMatrix(char* filename);

// Main function
int main() {
    writeMatrix();
    int* matrix = readMatrix("det.txt");/*
    // Print the matrix (for demonstration)
    printf("Matrix:\n");
    for (int i = 0; i < MAX_SIZE; i++) {
        for (int j = 0; j < MAX_SIZE; j++) {
            printf("%d ", matrix[i * MAX_SIZE + j]);
        }
        printf("\n");
    }
    printf("HAAAAAAAAAAAAAAAAAAAAA\n");*/
    // Calculate determinant using Laplace expansion
    // int determinant = laplaceDet(matrix, MAX_SIZE);
    int determinant = calc(matrix, 0, 0);
    printf("Determinant: %d\n", determinant);
    return 0;
}

void writeMatrix(){
    FILE *f;
    f = fopen("det.txt", "w");
    for(int i = 0; i < MAX_SIZE * MAX_SIZE;){
        fprintf(f, "%d ", (int)(rand() % 100));
        i++;
        if(i % MAX_SIZE == 0 && i != 0){
            fprintf(f, "\n");
        } 
    }
    fclose(f);
}

int* readMatrix(char* filename){
    FILE *f;
    f = fopen(filename, "r");
    if(f == NULL){
        printf("File couldn't be opened.\n");
        return NULL;
    } else {
        printf("File opened, initiating determinant calculation...\n");
        int *matrix = malloc(MAX_SIZE * MAX_SIZE * sizeof(int));
        int i = 0;
        while(fscanf(f, "%d ", &matrix[i]) == 1 && i < MAX_SIZE * MAX_SIZE){
            i++;
        }
        fclose(f);
        return matrix;
    }
}
