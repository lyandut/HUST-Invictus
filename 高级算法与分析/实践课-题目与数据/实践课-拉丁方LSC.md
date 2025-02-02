# 拉丁方补全算法训练

## 问题概述

给定一个 N * N 方阵, 部分单元格已经填充了数字且不得修改, 请为剩余的每个单元格填充一个 0 到 N-1 的整数, 使任意一行和任意一列内的数字均不相同.
为提高不同算法效果的区分度, 测试平台将松弛同行同列数字不同的约束, 并将存在相同数字的行或列的数量作为优化目标.

- 参考文献.
  - [1] S. Pan, Y. Wang, M. Yin, “A Fast Local Search Algorithm for the Latin Square Completion Problem,” AAAI 2022, vol. 36, no. 9, pp. 10327-10335, doi: 10.1609/aaai.v36i9.21274.
  - [2] Y. Jin and J.-K. Hao, “Solving the Latin Square Completion Problem by Memetic Graph Coloring,” IEEE Transactions on Evolutionary Computation, vol. 23, no. 6, pp. 1015-1028,  2019, doi: 10.1109/TEVC.2019.2899053.
  - [3] Z. Lü and J.-K. Hao, “A memetic algorithm for graph coloring,” European Journal of Operational Research, vol. 203, no. 1, pp. 241–250, 2010, doi: 10.1016/j.ejor.2009.07.016.
  - [4] L. Moalic and A. Gondran, “Variations on memetic algorithms for graph coloring problems,” Journal of Heuristics, vol. 24, no. 1, pp. 1–24, 2018, doi: 10.1007/s10732-017-9354-9.


## 命令行参数

请大家编写程序时支持两个命令行参数, 依次为运行时间上限 (单位为秒) 和随机种子 (0-65535).
算例文件已重定向至标准输入 `stdin`/`cin`, 标准输出 `stdout`/`cout` 已重定向至解文件 (如需打印调试信息, 请使用标准错误输出 `stderr`/`cerr`).
例如, 在控制台运行以下命令表示调用可执行文件 `lsc.exe` 在限时 600 秒, 随机种子为 12345 的情况下求解路径为 `../data/LSC.n50f750.00.txt` 的算例, 解文件输出至 `sln.LSC.n50f750.00.txt`:
```
lsc.exe 600 123456 <../data/LSC.n50f750.00.txt >sln.LSC.n50f750.00.txt
```

- 运行时间上限.
  - 超出运行时间上限后测试程序会强行终止算法, 请确保在此之前已输出解 (最好还能自行正常退出).
  - 建议的计算时间随数据规模不同为数秒到数分钟.
- 随机种子设置.
  - 使用 C 语言随机数生成器请用 `srand`.
  - 使用 C++ 随机数生成器 (如 `mt19937`) 请在构造时传参或调用 `seed()` 方法设置.


## 输入的算例文件格式

所有算例的行列与可填充数字均从 0 开始连续编号.

第一行给出 1 个整数, 表示方阵维度 N, 也即可填充的数字范围.
接下来连续若干行, 给出已填充固定整数的单元格信息.
每行包含 3 个由空白字符分隔的整数, 分别表示单元格的所在行 R, 单元格的所在列 C, 单元格填充整数 I.

例如, 以下算例文件表示方阵维度为 4; 其中:  
第 0 行第 1 列固定填充整数 2;  
第 2 行第 0 列固定填充整数 3.
```
4
0 1 2
2 0 3
```


## 输出的解文件格式

输出 N $\times$ N 个用空白字符分隔整数表示方阵中 N 行 N 列单元格填充的整数 (包括固定单元格), 第 i $\times$ N + j 个整数表示第 i 行第 j 列单元格填充的整数.

例如, 以下解文件表示第 1 行填充整数 0, 1, 2, 3, 第 2 行填充整数 1, 2, 3, 0, ...:
```
0 1 2 3
1 2 3 0
2 3 0 1
3 0 1 2

```


## 提交要求

- 提交单个压缩包 (文件大小 2M 以内), 文件名为 "**学号-姓名-LSC**", 其内包含下列文件.
  - **必要** 算法的可执行文件.
  - **必要** 算法源码.
  - **可选** 算法在各算例上的运行情况概要, 至少包括以下几项信息.
    - 算例名.
    - 计算耗时.
  - **可选** 算法在各算例上求得的冲突最少的解文件.

例如:
```
D0123456789-苏宙行-LSC.zip
|   lsc.exe
|   results.csv
|
+---src
|       main.cpp
|       algorithm.cpp
|       algorithm.h
|
+---results
        LSC.n50f750.00.txt
        LSC.n50f750.01.txt
        ...
```