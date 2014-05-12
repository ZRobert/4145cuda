#include <stdio.h>
#include <cuda.h>
#include <stdlib.h>
#include <time.h>

//Function prototypes
__global__ void gpu_ranksort(int *input, int *output, int N);
void cpu_ranksort(int *input, int *output, int N);
int sortCheck(int *input1, int *input2, int N);
void populateArrays(int *input, int *output1, int *output2, int N);

int main(int argc, char *argv[]){
    
    int N = 10;
    int i;
    int *a, *b, *c;
    int *dev_a, *dev_b;
    int gridSize = 32, blockSize = 32;
    cudaEvent_t start, stop;
    float elapsed_time_ms;
    
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    
    dim2 Grid(blockSize);
    dim2 Block(gridSize);

    //Setting up the data
    a = (int*) malloc(N * sizeof(int));
    b = (int*) malloc(N * sizeof(int));
    c = (int*) malloc(N * sizeof(int));
    
    populateArrays(a,b,c, N);
    
    printf("Input array: ");
    for(i = 0; i < N; i++){
        printf("%d ", a[i]);
    }
    printf("\n");
    
    cudaMalloc((void**)&dev_a, N * sizeof(int));
    cudaMalloc((void**)&dev_b, N * sizeof(int));
    
    cudaMemcpy(dev_a, a, N * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(dev_b, b, N * sizeof(int), cudaMemcpyHostToDevice);
    
    //Executing rank sort
    cudaEventRecord(start, 0);
    
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&elapsed_time_ms, start, stop);
    
    printf("Time to calculate results on GPU: %f ms. \n", elapsed_time_ms);
    
    cudaEventRecord(start, 0);
    
    cpu_ranksort(a, c, N);
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&elapsed_time_ms, start, stop);
    
    printf("Time to calculate results on CPU: %f ms.\n", elapsed_time_ms);
    
    
    //Finished and Cleanup
    printf("GPU Output array: ");
    for(i = 0; i < N; i++){
        printf("%d ", b[i]);
    }
    printf("\n");
    
    printf("CPU Output array: ");
    for(i = 0; i < N; i++){
        printf("%d ", c[i]);
    }
    printf("\n");
    
    if(sortCheck(b,c,N))
        printf("\nPASSED\n\n");
    else
        printf("\nFAILED\n\n");
    
    free(a);
    free(b);
    free(c);
    cudaFree(dev_a);
    cudaFree(dev_b);
    
    cudaEventDestory(start);
    cudaEventDestory(stop);
    
    return 0;
}

__global__ void gpu_ranksort(int *input, int *output, int N){
    int start = threadIdx.x + blockDim.x * blockIdx.x;
    int i, count = 0;
    if(start < N)
        for(i = 0; i < N; i++){
            if(input[i] > input[start])
                count++;
        }
        while(ouput[count] == input[start])
            count++;
    output[count] = input[start];
    
}

void cpu_ranksort(int *input, int *output, int N){
    int i, j;
    int count;

    
    for(i = 0; i < N; i++){
        count = 0;
        for(j = 0; j < N; j++){
            if(input[i] > input[j])
                count++;
        }
        
        while(input[i] == output[count])
            count++;
        
        output[count] = input[i];
    }
}

int sortCheck(int *input1, int *input2, int N){
    int i;
    for(i = 0; i < N; i++){
        if(input1[i] != input2[i])
            return 0;
    }
    return 1;
}

void populateArrays(int *input, int *output1, int *output2, int N){
    int i;
    srand(time(NULL));
    for(i = 0; i < N; i++){
        input[i] = rand()%10;
        output1[i] = output2[i] = -1;
    }
}