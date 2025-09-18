#include<iostream>
#include<sys/time.h>//提供获取高精度时间（gettimeofday）的函数
#include<stdlib.h>
#include<stdio.h>
#include<omp.h>  // 添加OpenMP头文件

#define ITERATIONS 10
using namespace std;

double timestamp(){
  struct timeval tv;
  gettimeofday (&tv, 0);
  return tv.tv_sec + 1e-6*tv.tv_usec;
}

float* allocate1D(int size) {
    return (float*)malloc(size * sizeof(float));
}

void free1D(float *array) {
    if (array) free(array);
}

// 访问宏定义：将二维索引转换为一维索引
#define A(i,j,N) A[(i)*(N) + (j)]
#define B(i,j,N) B[(i)*(N) + (j)]
#define C(i,j,N) C[(i)*(N) + (j)]

// 方案A：使用一维数组的函数
void yourFunction(float a, float b, float *A, float *B, float *C, int N){
  // OpenMP并行化外层循环
  #pragma omp parallel for
  for(int j=0; j<N; j++){
    for(int i=0; i<N; i++){
      C(i,j,N) += b * C(i,j,N);
      float tmp = 0;
      for(int k=0; k<N; k++){
        tmp += A(i,k,N) * B(k,j,N);
      }
      C(i,j,N) += tmp * a;
    }
  }
}

int main(int argc, char *argv[]){
  // 获取矩阵大小（支持命令行参数）
  int N = 800;  // 默认大小
  if(argc > 1) {
    N = atoi(argv[1]);
    if(N <= 0) {
      printf("错误：矩阵大小必须为正数\n");
      return -1;
    }
  }
  
  // 打印版本信息
  printf("=== 版本4：一维数组优化版本（堆分配） ===\n");
  printf("矩阵规模: %d x %d\n", N, N);
  printf("内存占用: %.2f MB\n", 3.0 * N * N * sizeof(float) / (1024.0 * 1024.0));
  printf("最大线程数: %d\n", omp_get_max_threads());
  
  // 动态分配一维数组内存
  printf("正在分配连续内存...\n");
  float *A = allocate1D(N * N);
  float *B = allocate1D(N * N);
  float *C = allocate1D(N * N);
  
  // 检查内存分配是否成功
  if (!A || !B || !C) {
    printf("错误：内存分配失败！\n");
    free1D(A);
    free1D(B);
    free1D(C);
    return -1;
  }
  
  printf("内存分配成功！\n");
  float a=0.5, b=0.3;
  for(int i=0; i<N; i++){
    for(int j=0; j<N; j++){
      A(i,j,N) = (float)rand()/(float)(RAND_MAX/a);
      B(i,j,N) = (float)rand()/(float)(RAND_MAX/a);
      C(i,j,N) = 0;
    }
  }

  //Warm-up
  for(int j=0; j<N; j++){
    for(int i=0; i<N; i++){
      C(i,j,N) += b * C(i,j,N);
      float tmp = 0;
      for(int k=0; k<N; k++){
        tmp += A(i,k,N) * B(k,j,N);
      }
      C(i,j,N) += tmp * a;
    }
  }

  for(int j=0; j<N; j++){
    for(int i=0; i<N; i++){
      C(i,j,N) = 0;
    }
  }

  double time1=timestamp();
  for(int numOfTimes=0; numOfTimes<ITERATIONS; numOfTimes++){
    yourFunction(a, b, A, B, C, N);
  }
  double time2=timestamp();

  double time = (time2-time1)/ITERATIONS;
  // 使用long long避免整数溢出
  long long flops = 2LL*N*N + 2LL*N*N*N + 2LL*N*N;
  double gflopsPerSecond = (double)flops/(1000000000.0)/time;
  
  // 输出性能结果
  printf("\n=== 性能结果 ===\n");
  printf("GFLOPS/s=%.6lf\n",gflopsPerSecond );
  printf("GFLOPS=%.6lf\n",(double)flops/(1000000000.0));
  printf("time(s)=%.6lf\n",time);
  
  // 释放内存
  printf("\n正在释放内存...\n");
  free1D(A);
  free1D(B);
  free1D(C);
  printf("内存释放完成！\n");
  
  return 0;
}
