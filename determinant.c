#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define T 12

int calcRec(int* matrix, int row, int column, int* visited);
int* readMatrix(char* filename);
void writeMatrix();

int main(){
    writeMatrix();
    int* matrix = readMatrix("det.txt");
    if(matrix == NULL){
        return -1;
    }
    for(int i = 0; i < T * T; i++){
        if(i % T == 0){
            printf("\n");
        }
        printf("%d ", matrix[i]);
    }
    printf("\n");
    clock_t start, end;
    double time_passed;
    int* visited = malloc(sizeof(int)*T);
    start = clock();
    int outcome = calcRec(matrix, 0, 0, visited);
    end = clock();
    time_passed = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("%d\n", outcome);
    printf("%f\n", time_passed);
    free(matrix);

    return 0;
}
void writeMatrix(){
    FILE *f;
    f = fopen("det.txt", "w");
    for(int i = 0; i < T*T;){
        fprintf(f, "%d ", (int)(rand() % 10));
        i++;
        if(i % T == 0 && i != 0){
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
        //printf("File opened, initiating determinant calculation...\n");
        int *matrix = malloc(T * T * sizeof(int));
        int i = 0;
        while(fscanf(f, "%d ", &matrix[i]) == 1 && i < T * T){
            i++;
        }
        fclose(f);
        return matrix;
    }
}


int calcRec(int *matrix, int row, int column, int* visited){ //(x,y) of the matrix element that's determinant is being calculated is (column, row)
    int edge_size = T - row;                             //since the matrix is square shaped, top-left corner will always be (row, row)
    int determinant = 0;                                //the column will dictate what index not to take
    visited[row] = column;
    // Base case: determinant of 1x1 matrix is the element itself
    if (edge_size == 1) {
        //printf("n = %d: index = 0: m = %d\n", edge_size, matrix[row * MAX_SIZE + column]);
        return matrix[row * T + column];
    } else {
        for(int i = 0, k = 0; i < T; i++){
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
                    determinant += matrix[row * T + i] * cofactor;
                } else {
                    determinant -= matrix[row * T + i] * cofactor;
                }
                //printf("%d\n", cofactor);
                //printf("%d\n", determinant);
                k++;
            }
        }
    }
    visited[row] = -1;
    return determinant;
}