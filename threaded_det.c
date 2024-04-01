#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX_SIZE 8
#define NUM_THREADS 1

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct ThreadData {
    int* matrix;    // Pointer to the matrix
    int column;     // Row to expand along
    int* visited;
    int result;     // Partial result (determinant)
};

void writeMatrix();
int* readMatrix(char* filename);
int laplaceDet(int* matrix, int n);
void* calcDet(void* arg);
int calcRec(int *matrix, int row, int column, int* visited);

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
        thread_data[i]->column = i * (n / NUM_THREADS); // Divide columns evenly among threads
        thread_data[i]->visited = malloc(MAX_SIZE * sizeof(int));
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
    int column = data->column;
    int *visited = data-> visited;
    // Store the partial result in the data structure
    
    for(int i = 0; i < MAX_SIZE/NUM_THREADS; i++) {
        //printf("K = %d, 0, %d : %d\n", i, column+i, matrix[i]);
        if(i % 2 == 0){
            data->result += calcRec(matrix, 0, column + i, visited);
        } else {
            data->result -= calcRec(matrix, 0, column + i, visited);
        }
    }

    pthread_exit(NULL);
}

int calcRec(int *matrix, int row, int column, int* visited){ //(x,y) of the matrix element that's determinant is being calculated is (column, row)
    int edge_size = MAX_SIZE - row;                             //since the matrix is square shaped, top-left corner will always be (row, row)
    long determinant = 0;                                //the column will dictate what index not to take
    visited[row] = column;
    // Base case: determinant of 1x1 matrix is the element itself
    if (edge_size == 1) {
        //printf("n = %d: index = 0: m = %d\n", edge_size, matrix[row * MAX_SIZE + column]);
        return matrix[row * MAX_SIZE + column];
    } else {
        for(int i = 0, k = 0; i < MAX_SIZE; i++){
            int f = 1;
            for(int j = 0; j <= row; j++){
                if(visited[j] == i){
                    f = 0;
                }
            }
            if(f == 1){
                //printf("K = %d, %d, %d : %d\n", k, row+1, i, matrix[(row+1)*MAX_SIZE +i]);
                int cofactor;
                cofactor = calcRec(matrix, row + 1, i, visited);
                if(k % 2 == 0){
                    determinant += matrix[row * MAX_SIZE + i] * cofactor;
                } else {
                    determinant -= matrix[row * MAX_SIZE + i] * cofactor;
                }
                k++;
            }
        }
    }
    visited[row] = -1;
    return determinant;
}
