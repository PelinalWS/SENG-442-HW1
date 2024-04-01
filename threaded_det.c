#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX_SIZE 16
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
int calcRec(int *matrix, int row, int column, int tid);

// Main function
int main() {
    writeMatrix();
    int* matrix = readMatrix("det.txt");
    clock_t start, end;
    double time_passed;
    start = clock();
    int determinant = laplaceDet(matrix, MAX_SIZE);
    end = clock();
    printf("Determinant: %d\n", determinant);
    time_passed = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time passed: %f", time_passed);
    return 0;
}

void writeMatrix(){
    FILE *f;
    f = fopen("det.txt", "w");
    for(int i = 0; i < MAX_SIZE * MAX_SIZE;){
        fprintf(f, "%d ", (int)(rand() % 10));
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
        thread_data[i]->sign = 1;
        pthread_create(&tid[i], NULL, calcDet, thread_data[i]);
    }

    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(tid[i], NULL);
        determinant += thread_data[i]->result;
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

    // Store the partial result in the data structure

    for(int i = 0; i < MAX_SIZE/NUM_THREADS; i++) {
        if(i % 2 == 0){
            data->result += calcRec(matrix, 0, column + i, column);
        } else {
            data->result -= calcRec(matrix, 0, column + i, column);
        }
    }
    pthread_exit(NULL);
}

/*
        0 1 0 0
        0 0 0 0  (1,0)
        0 0 0 0
        0 0 0 0

*/
int calcRec(int *matrix, int row, int column, int tid){ //(x,y) of the matrix element that's determinant is being calculated is (column, row)
    int edge_size = MAX_SIZE - row;                             //since the matrix is square shaped, top-left corner will always be (row, row)
    long determinant = 0;                                //the column will dictate what index not to take
    // Base case: determinant of 1x1 matrix is the element itself
    if (edge_size == 1) {
        //printf("tid = %d: n = %d: index = 0: m = %d\n", tid, edge_size, matrix[0]);
        return matrix[0];
    } else {
        int* temp = malloc((edge_size - 1) * (edge_size - 1) * sizeof(int));
        if (temp == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        for (int oldRow = 1, newRow = 0; oldRow < edge_size; oldRow++, newRow++) {
            for (int oldColumn = 0, newColumn = 0; oldColumn < edge_size; oldColumn++) {
                if (oldColumn != column) {
                    temp[newRow * (edge_size - 1) + newColumn] = matrix[oldRow * edge_size + oldColumn];
                    newColumn++;
                }
            }
        }

        for(int a = 0; a < (edge_size - 1); a++){
            for(int b = 0; b < edge_size -1; b++){
                //printf("tid = %d: n = %d: index = %d: m = %d\n", tid, edge_size, a*(edge_size - 1)+b, temp[a*(edge_size - 1)+b]);
            }
        }
        //printf("\n");
        for(int i = 0; i < edge_size - 1; i++){
            if(i % 2 == 0) {
                determinant += calcRec(temp, row + 1, i, tid);
            } else {
                determinant -= calcRec(temp, row + 1, i, tid);
            }
        }
        free(temp);
    }
    return determinant;
}