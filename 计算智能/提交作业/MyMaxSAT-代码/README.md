# MyMaXSat

The C++ implementation of the four approximation algorithms for  **Maximum Satisfiability  Problem**.

| Symbol                      | Definition                | Description                                                  | Type             |
| --------------------------- | ------------------------- | ------------------------------------------------------------ | ---------------- |
| $x_{i}$                     | *Variable*                | 变元，$x_{i}+\bar x_{i}=1$                                   | bool             |
| $C_{i}$                     | *Clause*                  | 子句，$C_{i}=\left(\bigvee_{i \in S_{i}^{+}} x_{i}\right) \vee\left(\bigvee_{i \in S_{i}^{-}} \bar{x}_{i}\right), i=1,2,...,m$ | bool             |
| $W_{i}$                     | *Weight*                  | 权重，对应子句 $C_{i}$ 的权重                                | integer          |
| $CNF / WCNF$                | *Formula*                 | (带权)合取范式，$CNF=\bigwedge_{i \in S} C_{i}$， $WCNF=\bigwedge_{i \in S} C_{i} \left\{ W_{i} \right\}$ | /                |
| $E\left[ Z_{i} \right]$     | *Expectation*             | 期望，对应字句 $C_{i}$ 被满足的期望                          | floating         |
| $E\left[ Z | x_{i} \right]$ | *Conditional Expectation* | 条件期望，在 $x_{i}$ 确定取值前提下，$CNF$ 被满足的期望      | floating         |
| $E\left[ Z \right]$         | *Total Expectation*       | 总期望，$CNF$ 被满足的期望                                   | floating         |
| $y_{i}$                     | *Decision Variables*      | 决策变量，变元 $x_{i}$ 的 0/1 决策变量                       | bool -> floating |
| $q_{i}$                     | *Decision Variables*      | 决策变量，子句 $C_{i}$ 的 0/1 决策变量                       | bool -> floating |

- **Maximum Satisfiability**：找到一组 $x_{i}$ 的取值，使满足的子句数目 $\left| C_{i} \right|$ 最大。 
- **Maximum Weight Satisfiability**：找到一组 $x_{i}$ 的取值，使满足的子句权重之和 $\sum_{i \in S} \left| C_{i} \times W_{i} \right|$ 最大。
- **Hard Clause & Soft Clause**：在 *Maximum Weight Satisfiability* 中，必须满足的子句称为硬子句，允许部分满足的子句称为软子句；但求解器一般不会判断子句类型（增加无意义的时空开销），而是通过**赋予硬子句一个大于所有软子句权重之和的权重**（即使满足所有软子句都不如满足一个硬子句获取的权重收益大），使求解器能自适应满足软硬子句约束。



# Algorithms

##  Randomized Algorithm

- **算法描述：**

  将 $x_{i}$ 分别以 $1/2$ 的概率设置为 0 或 1，则 $C_{i}$ 被满足的期望为 $E\left[ Z_{i} \right] = 1-2^{- \left| C_{i} \right|}$，$CNF$ 被满足的期望为 $E\left[ Z \right] = \sum_{i=1}^{m} E\left[ Z_{i} \right] = \sum_{i=1}^{m} \big( 1-2^{- \left| C_{i} \right|} \big)$。

- **近似比：**

  设 $min \left| C_{i} \right| = K$，则有
  $$
  m\left(1-2^{-K}\right) \leq \mathrm{E}[Z] \leq O P T \leq m
  $$

- **核心代码：**

  ```c++
  void randomize(double __p__ = 0.5) {
      for (auto &var : formula.variables) {
          var.second = getProbRandomNumber(__p__);
  	}
  }
  ```

- **算法分析：**

  - 简单粗暴，易于理解；
  - 结果不可控，近似比只是给出理论上期望的上界，而未必每次都能得到相应质量的解；
- 无法求解 $WCNF$ 包含软硬子句的情况。
  
  

##  Derandomized Algorithm

- **算法描述：**

  在算法①的基础上，每个变元 $x_{i}$ 都有 $1/2$ 的概率取 0 或 1，即 $E\left[ Z \right] = \frac{1}{2} E\left[ Z | x_{i}=1 \right] + \frac{1}{2} E\left[ Z | x_{i}=0 \right]$。对于每个变元 $x_{i}$，比较 $\frac{1}{2} E\left[ Z | x_{i} = 1 \right]$ 和 $\frac{1}{2} E\left[ Z | x_{i} = 0 \right]$ 的大小，选择二者中较大的期望值，取其对应的 $x_{i}$ 取值。在此基础上，进行下一步迭代。

- **近似比：**
  $$
  \mathrm{E}[Z | x] \geq \mathrm{E}[Z] \geq m\left(1-2^{-K}\right)
  $$

- **核心代码：**

  ```c++
  void derandomize() {
      for (auto &v : formula.variables) {
          double expectedTrue = expectedConditional(true);
          double expectedFalse = expectedConditional(false);
          if (expectedTrue > expectedFalse) {
              v.second = true;
              expectedUpdate(true);
          } else {
              v.second = false;
              expectedUpdate(false);
          }
      }
  }
  ```

- **算法分析：**

  - 结果可控，在变元顺序确定的情况下能保证结果一致性；
  - 结果与变元顺序有关，没有回溯，变元的值一旦确定便不能再更改；
  - 算法复杂度较高，计算条件期望比较耗时，通过多线程并行和设置计时器检测超时（30分钟）来控制算法结束；
  - 可能的优化方向：使用 *branch-and-bound* 的树搜索框架，通过维护一个全局的条件期望实现剪枝和加速搜索。

  

##  LP Rounding Algorithm 

- **算法描述：**

  建立 MaxSAT 混合整数规划模型如下（下面给出 $WCNF$ 的数学模型，对于 $CNF$，${\forall} W_{i} = 1$）：

$$
\begin{array}{ll}{\text { maximize: }} & {\sum_{i=1}^{m} q_{i} \times W_{i}} \\ {\text { s.t. }} & {q_{i} \leq \sum_{j \in S_{i}^{+}} y_{j}+\sum_{j \in S_{i}^{-}}\left(1-y_{j}\right) \quad \forall i} \\ {} & {q_{i}, y_{j} \in\{0,1\}}\end{array}
$$

松弛最后一条布尔约束为 $0 \leq q_{i}, y_{j} \leq 1$，得到线性规划模型。

使用线性规划求解器（Gurobi）求解模型，并按照下述策略设置变元取值：
$$
\begin{array}{l}{\text { for } j=1 \text { to } n \text { do }} \\ {\text { Independently set } x_{j}=\left\{\begin{array}{l}{1: \text { with probability } y_{j}^{*}} \\ {0: \text { with probability } 1-y_{j}^{*}}\end{array}\right.} \\ {\text { end for }}\end{array}
$$

- **近似比：**

  以 $C_{1}=x_{1} \vee \ldots \vee x_{k}$ 为例，利用算数-几何平均不等式，得到 $C_{1}$ 的部分期望如下
  $$
  \begin{aligned} \operatorname{Pr}\left[C_{1}\right] &=1-\prod_{j=1}^{k}\left(1-y_{j}^{*}\right) \\ & \geq 1-\left(\frac{1}{k} \sum_{j=1}^{k}\left(1-y_{j}^{*}\right)\right)^{k} \\ & \geq 1-\left(1-\frac{q_{1}^{*}}{k}\right)^{k} \\ & \geq 1-\left(1-\frac{1}{k}\right)^{k} \\ & \geq q_{1}\left(1-\left(1-\frac{1}{k}\right)^{k}\right) \\ & \geq q_{1}(1-1 / e) \end{aligned}
  $$
  设 $min \left| C_{i} \right| = K$，则
  $$
  \mathrm{E}[Z]=\sum_{i=1}^{m} \mathrm{E}\left[Z_{i}\right] \geq \sum_{i=1}^{m} \left(1-\left(1-\frac{1}{\left|C_{i}\right|}\right)^{\left|C_{i}\right|}\right) \geq m\left(1- \left( 1- \frac{1}{K} \right) ^ {K} \right)
  $$
  

- **核心代码：**

  ```c++
  List<double> gurobiModel() {
      List<double> p_list(formula.variables.size(), 0.5);
  
      // Initialize environment & empty model
      GRBEnv env = GRBEnv(true);
      env.start();
      GRBModel gm = GRBModel(env);
  
      /*
       * Decision Variables
       * 1. Bool: y_j correspond to the values of each boolean variable x_j.
       * 2. Bool: q_i correspond to the truth value of each clause C_i.
       * 3. Relax: 0 <= y_i, q_i <= 1.
       */
      List<GRBVar> y(formula.variables.size());
      List<GRBVar> q(formula.clauses.size());
      for (const auto &v : formula.variables)
          y[v.first] = gm.addVar(0, 1, 0, GRB_CONTINUOUS);
      for (int i = 0; i < formula.clauses.size(); ++i)
          q[i] = gm.addVar(0, 1, 0, GRB_CONTINUOUS);
  
      /*
       * Constraint
       * q_i <= Sum(y_j) + Sum(1 - ~y_j)
       */
      for (int i = 0; i < formula.clauses.size(); ++i) {
          GRBLinExpr sum_variables = 0;
          for (const auto &v : formula.clauses[i].variables) {
              if (v.type == Variable::VarType::positive)
                  sum_variables += y.at(v.id);
              else
                  sum_variables += (1 - y.at(v.id));
          }
          gm.addConstr(q.at(i) <= sum_variables);
      }
  
      /*
       * Objective Function
       * maximize Sum(q_i * W_i)
       */
      GRBLinExpr obj = 0;
      for (int i = 0; i < formula.clauses.size(); ++i)
          obj += q.at(i) * formula.clauses.at(i).weight;
      gm.setObjective(obj, GRB_MAXIMIZE);
  
      // Optimize model
      gm.optimize();
      std::cout << "Obj: " << gm.get(GRB_DoubleAttr_ObjVal) << std::endl;
      for (const auto &v : formula.variables)
          p_list[v.first] = y.at(v.first).get(GRB_DoubleAttr_X);
      
      return p_list;
  }
  ```

- **算法分析：**

  - 结果相对可控，较算法①求解质量有较大提升；
  - 随着变元数目增多，求解时间也逐渐延长（设置计时器为30分钟控制超时）；
  - 伴随出现的还有内存溢出、浮点数溢出等问题，需要增加相应的异常处理代码；
  - 无法求解 $WCNF$ 包含软硬子句的情况。



## LP Derandomized Algorithm

- **算法描述：**

  在算法③的基础上去随机化，每个变元 $x_{i}$ 都有 $y_{i}^{*}$ 的概率取1，$1-y_{i}^{*}$的概率取0，即 $E\left[ Z \right] = y_{i}^{*} \cdot E\left[ Z | x_{i}=1 \right] + (1-y_{i}^{*}) \cdot E\left[ Z | x_{i}=0 \right]$。对于每个变元 $x_{i}$，比较 $y_{i}^{*} \cdot E\left[ Z | x_{i} = 1 \right]$ 和 $(1-y_{i}^{*}) \cdot E\left[ Z | x_{i} = 0 \right]$ 的大小，选择二者中较大的期望值，取其对应的 $x_{i}$ 取值。在此基础上，进行下一步迭代。

- **近似比：**
  $$
  \mathrm{E}[Z | x] \geq \mathrm{E}[Z] \geq m\left(1- \left( 1- \frac{1}{K} \right) ^ {K} \right)
  $$

- **核心代码：**

  ```c++
  void solve() {
      List<double> p_list = gurobiModel(); // LP Rounding Algorithm
      derandomize(p_list);                 // Derandomized Algorithm
  }
  ```

- **算法分析：**

  - 结果可控，且与变元顺序无关；

  - 继承了算法②③的优缺点，四类算法中求解质量最好的，也是耗时最多的；

  - 可能的优化方向：将算法②和算法④结合可以给出一个 $3/4$ 近似比的算法（每次从两个算法中随机挑选一个执行）
    $$
    \mathrm{E}[Z]=\sum_{i=1}^{m} \mathrm{E}\left[Z_{i}\right] \geq \sum_{i=1}^{m} \frac{1}{2}\left(\left(1-2^{-\left|C_{i}\right|}\right)+\left(1-\left(1-\frac{1}{\left|C_{i}\right|}\right)^{\left|C_{i}\right|}\right)\right) \geq(3 / 4) m
    $$



