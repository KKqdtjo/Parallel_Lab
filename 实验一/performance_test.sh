#!/bin/bash
echo "=== OpenMP矩阵乘法性能测试 ==="
echo "测试时间: $(date)"
echo "系统信息:"
echo "CPU: $(cat /proc/cpuinfo | grep 'model name' | head -1 | cut -d: -f2 | xargs)"
echo "CPU核心数: $(cat /proc/cpuinfo | grep 'processor' | wc -l)"
echo "内存信息: $(free -h | grep 'Mem:' | awk '{print $2}')"
echo

# 确保程序已编译
if [ ! -f "./main" ]; then
    echo "正在编译程序..."
    make clean && make
    if [ $? -ne 0 ]; then
        echo "编译失败！"
        exit 1
    fi
fi

# 创建结果文件
RESULT_FILE="performance_results_$(date +%Y%m%d_%H%M%S).txt"
echo "结果将保存到: $RESULT_FILE"

# 测试不同规模的矩阵
echo "=== 多规模性能测试开始 ===" | tee $RESULT_FILE
echo "格式: 矩阵大小, 内存占用(MB), GFLOPS/s, 时间(s), 总GFLOPS" | tee -a $RESULT_FILE

# 小规模测试
echo "--- 小规模测试 ---" | tee -a $RESULT_FILE
for N in 200 400 600 800; do
    echo "测试 N=$N ..." | tee -a $RESULT_FILE
    ./main $N 2>&1 | grep -E "(矩阵规模|内存占用|GFLOPS/s|time)" | tee -a $RESULT_FILE
    echo "---" | tee -a $RESULT_FILE
done

# 中规模测试
echo "--- 中规模测试 ---" | tee -a $RESULT_FILE
for N in 1000 1200 1600 2000; do
    echo "测试 N=$N ..." | tee -a $RESULT_FILE
    ./main $N 2>&1 | grep -E "(矩阵规模|内存占用|GFLOPS/s|time)" | tee -a $RESULT_FILE
    echo "---" | tee -a $RESULT_FILE
done

# 大规模测试（根据内存情况）
echo "--- 大规模测试 ---" | tee -a $RESULT_FILE
for N in 2400 2800 3200; do
    echo "测试 N=$N ..." | tee -a $RESULT_FILE
    # 检查内存是否足够（粗略估算）
    MEMORY_NEEDED=$(echo "3 * $N * $N * 4 / 1024 / 1024" | bc)
    AVAILABLE_MEMORY=$(free -m | grep 'Mem:' | awk '{print $7}')
    
    if [ $MEMORY_NEEDED -lt $AVAILABLE_MEMORY ]; then
        ./main $N 2>&1 | grep -E "(矩阵规模|内存占用|GFLOPS/s|time)" | tee -a $RESULT_FILE
    else
        echo "跳过 N=$N (内存不足: 需要${MEMORY_NEEDED}MB, 可用${AVAILABLE_MEMORY}MB)" | tee -a $RESULT_FILE
    fi
    echo "---" | tee -a $RESULT_FILE
done

echo "=== 性能测试完成 ===" | tee -a $RESULT_FILE
echo "详细结果已保存到: $RESULT_FILE"
