#!/bin/bash
# 获取CPU信息
CPU_MODEL=$(cat /proc/cpuinfo | grep 'model name' | head -1 | cut -d: -f2 | xargs)
CPU_CORES=$(cat /proc/cpuinfo | grep 'processor' | wc -l)
CPU_THREADS=$(cat /proc/cpuinfo | grep 'siblings' | head -1 | awk '{print $3}')
CPU_FREQ_MHZ=$(cat /proc/cpuinfo | grep 'cpu MHz' | head -1 | awk '{print $4}')

echo "CPU型号: $CPU_MODEL"
echo "物理核心数: $CPU_CORES"
echo "线程数: $CPU_THREADS"
echo "CPU频率: ${CPU_FREQ_MHZ} MHz"
echo

# 检测CPU特性
CPU_FLAGS=$(cat /proc/cpuinfo | grep 'flags' | head -1)
HAS_AVX2=0
HAS_FMA=0
HAS_AVX512=0

if [[ $CPU_FLAGS == *"avx2"* ]]; then
    HAS_AVX2=1
    echo "✓ 支持AVX2指令集"
fi

if [[ $CPU_FLAGS == *"fma"* ]]; then
    HAS_FMA=1
    echo "✓ 支持FMA (融合乘加)指令"
fi

if [[ $CPU_FLAGS == *"avx512"* ]]; then
    HAS_AVX512=1
    echo "✓ 支持AVX512指令集"
fi

echo

# 计算理论峰值
CPU_FREQ_GHZ=$(echo "scale=3; $CPU_FREQ_MHZ / 1000" | bc)

echo "=== 理论峰值计算 ==="

# 不同指令集的计算能力
if [ $HAS_AVX512 -eq 1 ]; then
    # AVX512: 16个单精度浮点数，如果支持FMA则每时钟32次运算
    if [ $HAS_FMA -eq 1 ]; then
        OPS_PER_CLOCK=32
        INSTRUCTION_SET="AVX512 + FMA"
    else
        OPS_PER_CLOCK=16
        INSTRUCTION_SET="AVX512"
    fi
elif [ $HAS_AVX2 -eq 1 ]; then
    # AVX2: 8个单精度浮点数，如果支持FMA则每时钟16次运算
    if [ $HAS_FMA -eq 1 ]; then
        OPS_PER_CLOCK=16
        INSTRUCTION_SET="AVX2 + FMA"
    else
        OPS_PER_CLOCK=8
        INSTRUCTION_SET="AVX2"
    fi
else
    # SSE: 4个单精度浮点数，每时钟4次运算
    OPS_PER_CLOCK=4
    INSTRUCTION_SET="SSE"
fi

echo "使用指令集: $INSTRUCTION_SET"
echo "每时钟周期运算数: $OPS_PER_CLOCK"
echo

# 理论峰值计算
THEORETICAL_PEAK=$(echo "scale=3; $CPU_CORES * $CPU_FREQ_GHZ * $OPS_PER_CLOCK" | bc)

echo "理论峰值计算:"
echo "理论峰值 = 核心数 × 频率 × 每时钟运算数"
echo "理论峰值 = $CPU_CORES × ${CPU_FREQ_GHZ}GHz × $OPS_PER_CLOCK"
echo "理论峰值 = $THEORETICAL_PEAK GFLOPS"
echo


# 保存结果到文件
RESULT_FILE="theoretical_peak_$(date +%Y%m%d_%H%M%S).txt"
{
    echo "系统理论峰值分析报告"
    echo "生成时间: $(date)"
    echo
    echo "CPU信息:"
    echo "  型号: $CPU_MODEL"
    echo "  核心数: $CPU_CORES"
    echo "  频率: ${CPU_FREQ_GHZ}GHz"
    echo "  指令集: $INSTRUCTION_SET"
    echo
    echo "理论峰值: $THEORETICAL_PEAK GFLOPS"
    echo
} > $RESULT_FILE

echo "理论峰值分析结果已保存到: $RESULT_FILE"
