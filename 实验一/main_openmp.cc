#include<iostream>
#include<sys/time.h>//提供获取高精度时间（gettimeofday）的函数
#include<stdlib.h>
#include<stdio.h>
#include<omp.h>  // 添加OpenMP头文件

#define N 800
#define ITERATIONS 10
using namespace std;

double timestamp(){
  struct timeval tv;
  gettimeofday (&tv, 0);
  return tv.tv_sec + 1e-6*tv.tv_usec;
}

void yourFunction(float a, float b, float A[N][N], float B[N][N], float C[N][N]){
  // OpenMP并行化外层循环
  #pragma omp parallel for
  for(int j=0; j<N; j++){
    for(int i=0; i<N; i++){
      C[i][j]+=b*C[i][j];
      float tmp=0;
      for(int k=0; k<N; k++){
        tmp += A[i][k]*B[k][j];
      }
      C[i][j] += tmp*a;
    }
  }
}

int main(){
  // 打印版本信息
  printf("=== 版本2：OpenMP并行版本（栈分配） ===\n");
  printf("矩阵规模: %d x %d\n", N, N);
  printf("内存占用: %.2f MB\n", 3.0 * N * N * sizeof(float) / (1024.0 * 1024.0));
  printf("最大线程数: %d\n", omp_get_max_threads());
  
  float A[N][N], B[N][N], C[N][N];
  float a=0.5, b=0.3;
  for(int i=0; i<N; i++){
    for(int j=0; j<N; j++){
      A[i][j]=(float)rand()/(float)(RAND_MAX/a);
      B[i][j]=(float)rand()/(float)(RAND_MAX/a);
      C[i][j]=0;
    }
  }

  //Warm-up
  for(int j=0; j<N; j++){
    for(int i=0; i<N; i++){
      C[i][j]+=b*C[i][j];
      float tmp=0;
      for(int k=0; k<N; k++){
        tmp += A[i][k]*B[k][j];
      }
      C[i][j] += tmp*a;
    }
  }

  for(int j=0; j<N; j++){
    for(int i=0; i<N; i++){
      C[i][j] = 0;
    }
  }

  double time1=timestamp();
  for(int numOfTimes=0; numOfTimes<ITERATIONS; numOfTimes++){
    yourFunction(a, b, A, B, C);
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
  
  return 0;
}
