#include <stdio.h>
#include <stdlib.h>

#define BUFFER 1024
#define T 5

int detCalc(int* matrix, int n);

int* readMatrix(char* filename);

char* writeMatrix();

int main(){
    char* filename = writeMatrix();
    int* matrix = readMatrix(filename);
    if(matrix == NULL){
        return -1;
    }
    for(int i = 0; i < T * T; i++){
        if(i % T == 0){
            printf("\n");
        }
        printf("%d ", matrix[i]);
    }

    int outcome = detCalc(matrix, T);
    printf("\n%d", outcome);
    
    free(matrix);

    return 0;
}
char* writeMatrix(){
    FILE *f;
    f = fopen("det.txt", "w");
    for(int i = 0; i < T*T;){
        fprintf(f, "%d ", (int)(rand() % 100));
        i++;
        if(i % T == 0 && i != 0){
            fprintf(f, "\n");
        } 
    }
    fclose(f);
    return "det.txt";
}

int* readMatrix(char* filename){
    FILE *f;
    f = fopen(filename, "r");
    if(f == NULL){
        printf("File couldn't be opened.\n");
        return NULL;
    } else {
        printf("File opened, initiating determinant calculation...\n");
        int *matrix = malloc(T * T * sizeof(int));
        int i = 0;
        while(fscanf(f, "%d ", &matrix[i]) == 1 && i < T * T){
            i++;
        }
        fclose(f);
        return matrix;
    }
}


int detCalc(int* matrix, int n){
    int det = 0;
    if (n == 1) {
        return *matrix;
    } 
    else {
        int *temp = malloc((n-1)*(n-1) * sizeof(int));
        int sign = 1;
        for (int i = 0; i < n; i++) {
            int row = 0;
            for (int j = 1; j < n; j++) {
                int column = 0;
                for (int k = 0; k < n; k++) {
                    if (k != i) {
                        temp[row * (n-1) + column] = matrix[j * n + k];
                        column++;
                    }
                }
                row++;
            }
            det += sign * matrix[i] * detCalc(temp, n - 1);
            sign = -sign;
        }
        free(temp);
    }
    return det;
}