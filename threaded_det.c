#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_SIZE 4
#define NUM_THREADS 4

// Structure to pass parameters to threads
struct ThreadData {
    int* matrix;    // Pointer to the matrix
    int n;          // Size of the matrix
    int column;     // Row to expand along
    int sign;       // Sign multiplier
    int result;     // Partial result (determinant)
};

void writeMatrix();
int* readMatrix(char* filename);
int laplaceDet(int* matrix, int n);
void* calcDet(void* arg);
int calcRec(int *matrix, int row, int column);

// Main function
int main() {
    writeMatrix();
    int* matrix = readMatrix("det.txt");
    // Print the matrix (for demonstration)
    /*
    printf("Matrix:\n");
    for (int i = 0; i < MAX_SIZE; i++) {
        for (int j = 0; j < MAX_SIZE; j++) {
            printf("%d ", matrix[i * MAX_SIZE + j]);
        }
        printf("\n");
    }
    */
    // Calculate determinant using Laplace expansion
    int determinant = laplaceDet(matrix, MAX_SIZE);
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

int laplaceDet(int* matrix, int n) {
    int determinant = 0;
    struct ThreadData* thread_data[NUM_THREADS];
    pthread_t tid[NUM_THREADS];

    for(int i = 0; i < NUM_THREADS; i++) {
        thread_data[i] = malloc(sizeof(struct ThreadData));
        thread_data[i]->matrix = matrix;
        thread_data[i]->n = n;
        thread_data[i]->column = i * (n / NUM_THREADS); // Divide columns evenly among threads
        pthread_create(&tid[i], NULL, calcDet, thread_data[i]);
    }

    for(int i = 0; i < NUM_THREADS; i++) {
        determinant += thread_data[i]->result;
        pthread_join(tid[i], NULL);
        free(thread_data[i]);
    }
    return determinant;
}

void* calcDet(void* arg) {
    struct ThreadData* data = (struct ThreadData*)arg;
    int* matrix = data->matrix;
    int n = data->n;
    int column = data->column;
    int sign = data->sign;
    int result = 0;

    // Store the partial result in the data structure
    data->result = result;

    for(int i = 0; i < MAX_SIZE/NUM_THREADS; i++) {
        if(i % 2 == 0){
            result += calcRec(matrix, 0, 0);
        } else {
            result -= calcRec(matrix, 0, 0);
        }
    }
    data->result = result;
    pthread_exit(NULL);
}

int calcRec(int *matrix, int row, int column){
    int n = MAX_SIZE - row;
    int determinant = 0;
    // Base case: determinant of 1x1 matrix is the element itself
    if (n == 1) {
        determinant = matrix[0];
    } else {
        int* temp = malloc((n - 1) * (n - 1) * sizeof(int));
        if (temp == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        for (int i = 1, temp_row = 0; i < n; i++, temp_row++) {
            int temp_col = 0;
            for (int j = 0, temp_col = 0; j < n; j++) {
                if (j != column) {
                    temp[temp_row * (n - 1) + temp_col] = matrix[i * n + j];
                    temp_col++;
                }
            }
        }
        for(int i = 0; i < n; i++){
            if(i % 2 == 0) {
                determinant += calcRec(temp, row + 1, column + i);
            } else {
                determinant -= calcRec(temp, row + 1, column + i);
            }
        }
    }
    return determinant;
}