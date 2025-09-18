# OpenMP矩阵乘法 - 多版本Makefile
CC = g++
CFLAGS = -O3
OPENMP_FLAGS = -fopenmp

# 默认目标
all: original openmp malloc2d malloc1d

# 版本1：原始串行版本（栈分配）
original: main_original.cc
	$(CC) $(CFLAGS) main_original.cc -o main_original
	@echo "✓ 编译完成: main_original (串行版本)"

# 版本2：OpenMP并行版本（栈分配）
openmp: main_openmp.cc
	$(CC) $(CFLAGS) $(OPENMP_FLAGS) main_openmp.cc -o main_openmp
	@echo "✓ 编译完成: main_openmp (OpenMP并行版本)"

# 版本3：二维指针版本（堆分配）
malloc2d: main_malloc2d.cc
	$(CC) $(CFLAGS) $(OPENMP_FLAGS) main_malloc2d.cc -o main_malloc2d
	@echo "✓ 编译完成: main_malloc2d (二维指针版本)"

# 版本4：一维数组版本
malloc1d: main_malloc1d.cc
	$(CC) $(CFLAGS) $(OPENMP_FLAGS) main_malloc1d.cc -o main_malloc1d
	@echo "✓ 编译完成: main_malloc1d (一维数组版本)"

# 保持原有的main目标（使用最新版本）
main: main_malloc1d.cc
	$(CC) $(CFLAGS) $(OPENMP_FLAGS) main_malloc1d.cc -o main

# 性能测试目标
test: all
	@echo "=== 四版本性能对比测试 ==="
	@echo "1. 原始串行版本："
	@./main_original
	@echo
	@echo "2. OpenMP并行版本："
	@./main_openmp  
	@echo
	@echo "3. 二维指针版本："
	@./main_malloc2d 800
	@echo
	@echo "4. 一维数组版本："
	@./main_malloc1d 800
	@echo "=== 测试完成 ==="

# 大规模测试（只测试支持大规模的版本）
test-large: malloc2d malloc1d
	@echo "=== 大规模矩阵测试 (N=2000) ==="
	@echo "3. 二维指针版本："
	@./main_malloc2d 2000
	@echo
	@echo "4. 一维数组版本："
	@./main_malloc1d 2000
	@echo "=== 大规模测试完成 ==="

# 清理目标
clean:
	rm -f main_original main_openmp main_malloc2d main_malloc1d main
	@echo "✓ 清理完成"

# 帮助信息
help:
	@echo "可用的编译目标："
	@echo "  make original  - 编译原始串行版本"
	@echo "  make openmp    - 编译OpenMP并行版本"
	@echo "  make malloc2d  - 编译二维指针版本"
	@echo "  make malloc1d  - 编译一维数组版本"
	@echo "  make all       - 编译所有版本"
	@echo "  make test      - 运行四版本性能对比"
	@echo "  make test-large- 运行大规模矩阵测试"
	@echo "  make clean     - 清理编译文件"
	@echo "  make help      - 显示此帮助信息"

# 声明伪目标
.PHONY: all test test-large clean help