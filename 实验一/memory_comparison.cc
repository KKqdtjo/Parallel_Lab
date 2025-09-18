#include<iostream>
#include<sys/time.h>
#include<stdlib.h>
#include<stdio.h>

#define N 800
#define ITERATIONS 100

double timestamp(){
  struct timeval tv;
  gettimeofday (&tv, 0);
  return tv.tv_sec + 1e-6*tv.tv_usec;
}

void test_stack_allocation() {
    printf("=== 栈内存分配测试 ===\n");
    
    double start = timestamp();
    for(int iter = 0; iter < ITERATIONS; iter++) {
        // 栈上分配
        float A[N][N], B[N][N], C[N][N];
        
        // 简单的矩阵运算防止编译器优化
        for(int i = 0; i < 10; i++) {
            for(int j = 0; j < 10; j++) {
                A[i][j] = 1.0f;
                B[i][j] = 2.0f;
                C[i][j] = A[i][j] + B[i][j];
            }
        }
        
        // 使用结果防止优化
        if(C[0][0] != 3.0f) printf("Error\n");
    }
    double end = timestamp();
    
    printf("栈分配总时间: %.6f秒\n", end - start);
    printf("平均每次分配: %.6f秒\n", (end - start) / ITERATIONS);
}

void test_heap_allocation() {
    printf("\n=== 堆内存分配测试 ===\n");
    
    double start = timestamp();
    for(int iter = 0; iter < ITERATIONS; iter++) {
        // 堆上分配
        float *A = (float*)malloc(N * N * sizeof(float));
        float *B = (float*)malloc(N * N * sizeof(float));
        float *C = (float*)malloc(N * N * sizeof(float));
        
        // 简单的矩阵运算
        for(int i = 0; i < 10; i++) {
            for(int j = 0; j < 10; j++) {
                A[i*N + j] = 1.0f;
                B[i*N + j] = 2.0f;
                C[i*N + j] = A[i*N + j] + B[i*N + j];
            }
        }
        
        // 使用结果防止优化
        if(C[0] != 3.0f) printf("Error\n");
        
        // 释放内存
        free(A);
        free(B);
        free(C);
    }
    double end = timestamp();
    
    printf("堆分配总时间: %.6f秒\n", end - start);
    printf("平均每次分配: %.6f秒\n", (end - start) / ITERATIONS);
}

void test_memory_access_pattern() {
    printf("\n=== 内存访问模式测试 ===\n");
    
    // 栈分配
    float A_stack[N][N];
    float *A_heap = (float*)malloc(N * N * sizeof(float));
    
    // 初始化
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            A_stack[i][j] = i * j;
            A_heap[i*N + j] = i * j;
        }
    }
    
    // 测试栈访问速度
    double start = timestamp();
    float sum1 = 0;
    for(int iter = 0; iter < 10; iter++) {
        for(int i = 0; i < N; i++) {
            for(int j = 0; j < N; j++) {
                sum1 += A_stack[i][j];
            }
        }
    }
    double stack_access_time = timestamp() - start;
    
    // 测试堆访问速度
    start = timestamp();
    float sum2 = 0;
    for(int iter = 0; iter < 10; iter++) {
        for(int i = 0; i < N; i++) {
            for(int j = 0; j < N; j++) {
                sum2 += A_heap[i*N + j];
            }
        }
    }
    double heap_access_time = timestamp() - start;
    
    printf("栈访问时间: %.6f秒 (sum=%.2f)\n", stack_access_time, sum1);
    printf("堆访问时间: %.6f秒 (sum=%.2f)\n", heap_access_time, sum2);
    printf("性能比率: %.2fx\n", heap_access_time / stack_access_time);
    
    free(A_heap);
}

int main() {
    printf("内存分配方式性能对比测试\n");
    printf("矩阵大小: %d x %d\n", N, N);
    printf("内存占用: %.2f MB\n", 3.0 * N * N * sizeof(float) / (1024.0 * 1024.0));
    printf("测试次数: %d\n\n", ITERATIONS);
    
    test_stack_allocation();
    test_heap_allocation();
    test_memory_access_pattern();
    
    return 0;
}
