# Matrix Multiplication Optimization

## Representations

Given three $n \times n$ matrices $\boldsymbol{A}$, $\boldsymbol{B}$, and $\boldsymbol{C}$.

$$
\begin{align}
\boldsymbol{C} & = \boldsymbol{A}\boldsymbol{B}
\\

\begin{bmatrix}
  c_{11} & c_{12} & \cdots & c_{1n} \\
  c_{21} & c_{22} & \cdots & c_{2n} \\
  \vdots  & \vdots  & \ddots & \vdots  \\
  c_{n1} & c_{n2} & \cdots & c_{nn} 
\end{bmatrix} & = 
\begin{bmatrix}
  a_{11} & a_{12} & \cdots & a_{1n} \\
  a_{21} & a_{22} & \cdots & a_{2n} \\
  \vdots  & \vdots  & \ddots & \vdots  \\
  a_{n1} & a_{n2} & \cdots & a_{nn} 
\end{bmatrix}
\begin{bmatrix}
  b_{11} & b_{12} & \cdots & b_{1n} \\
  b_{21} & b_{22} & \cdots & b_{2n} \\
  \vdots  & \vdots  & \ddots & \vdots  \\
  b_{n1} & b_{n2} & \cdots & b_{nn} 
\end{bmatrix}
\end{align}
$$

The multiplication can be achieved in a divide and conquer manner which operates on the submatrices.

$$
\begin{bmatrix}
  \boldsymbol{c}_{11} & \boldsymbol{c}_{12} & \cdots & \boldsymbol{c}_{1m} \\
  \boldsymbol{c}_{21} & \boldsymbol{c}_{22} & \cdots & \boldsymbol{c}_{2m} \\
  \vdots  & \vdots  & \ddots & \vdots  \\
  \boldsymbol{c}_{m1} & \boldsymbol{c}_{m2} & \cdots & \boldsymbol{c}_{mm} 
\end{bmatrix} = 
\begin{bmatrix}
  \boldsymbol{a}_{11} & \boldsymbol{a}_{12} & \cdots & \boldsymbol{a}_{1m} \\
  \boldsymbol{a}_{21} & \boldsymbol{a}_{22} & \cdots & \boldsymbol{a}_{2m} \\
  \vdots  & \vdots  & \ddots & \vdots  \\
  \boldsymbol{a}_{m1} & \boldsymbol{a}_{m2} & \cdots & \boldsymbol{a}_{mm} 
\end{bmatrix}
\begin{bmatrix}
  \boldsymbol{b}_{11} & \boldsymbol{b}_{12} & \cdots & \boldsymbol{b}_{1m} \\
  \boldsymbol{b}_{21} & \boldsymbol{b}_{22} & \cdots & \boldsymbol{b}_{2m} \\
  \vdots  & \vdots  & \ddots & \vdots  \\
  \boldsymbol{b}_{m1} & \boldsymbol{b}_{m2} & \cdots & \boldsymbol{b}_{mm} 
\end{bmatrix}
$$

The original calculation process of each submatrix $\boldsymbol{c}_{ij}$ is as follows.

$$
\boldsymbol{c}_{ij} = \sum^{m}_{k=1} \boldsymbol{a}_{ik} \boldsymbol{b}_{kj}, \quad \forall i, j \in [1, m]
\label{c_origin_calc}
$$


## Constructing Intermediates

We construct $u$ intermediates $\boldsymbol{M}_{v}, \forall 1 \le v \le u$, each of which is in the product-of-sum form and contains only single multiplication.
Let $p^{v}_{ij}, q^{v}_{ij} \in R, \forall v \in [1, u], \forall i, j \in [1, m]$ be proper constant scalar parameters to determine, indicating the coefficients of $\boldsymbol{a}_{ij}, \boldsymbol{b}_{ij}$ in the linear expressions for calculating $\boldsymbol{M}_{v}$, respectively.
As long as $u < m^{3}$ is true, there will be less multiplications.

$$
\boldsymbol{M}_{v} = \left( \sum^{m}_{i=1} \sum^{m}_{j=1} p^{v}_{ij} \boldsymbol{a}_{ij} \right) \left( \sum^{m}_{i=1} \sum^{m}_{j=1} q^{v}_{ij} \boldsymbol{b}_{ij} \right), \quad \forall v \in [1, u]
$$


## Reformulating Expressions

We calculate each submatrix $\boldsymbol{c}_{ij}$ by adding the intermediates only.
Let $r^{v}_{ij} \in R, \forall v \in [1, u], \forall i, j \in [1, m]$ be proper constant scalar parameters to determine, indicating the coefficient of intermediate matrix $v$ in the linear expression for calculating $\boldsymbol{c}_{ij}$.

$$
\boldsymbol{c}_{ij} = \sum^{u}_{v=1} r^{v}_{ij} \boldsymbol{M}_{v}, \quad \forall i, j \in [1, m]
\label{c_new_calc}
$$


## Determining Parameters

In order to determine the proper constant scalar parameters $p^{v}_{ij}, q^{v}_{ij}, r^{v}_{ij} \in R, \forall v \in [1, u], \forall i, j \in [1, m]$, we combine Equations $(\ref{c_origin_calc})$ and $(\ref{c_new_calc})$.

$$
\sum^{m}_{k=1} \boldsymbol{a}_{ik} \boldsymbol{b}_{kj} = \sum^{u}_{v=1} r^{v}_{ij} \boldsymbol{M}_{v}, \quad \forall i, j \in [1, m]
$$

For each term $\boldsymbol{a}_{ik} \boldsymbol{b}_{kj}$ on the left-hand side, there are corresponding ones $r^{v}_{ij} p^{v}_{ik} q^{v}_{kj} \boldsymbol{a}_{ik} \boldsymbol{b}_{kj}, \forall v \in [1, u]$ on the right-hand side. Apparently, their coefficients must be equal.

$$
\sum^{u}_{v=1} r^{v}_{ij} p^{v}_{ik} q^{v}_{kj} = 1, \quad \forall i, j, k \in [1, m]
\label{coef_exist}
$$

Furthermore, for each term which does not appear on the left-hand side, there are still corresponding ones on the right-hand side, whose coefficients must be zero.

$$
\sum^{u}_{v=1} r^{v}_{ij} p^{v}_{i'k} q^{v}_{k'j'} = 0, \quad \forall i, i', j, j', k, k' \in [1, m], (i \ne i') \vee (j \ne j') \vee (k \ne k')
\label{coef_non_exist}
$$

In the literature, Equations $(\ref{coef_exist})$ and $(\ref{coef_non_exist})$ are usually combined into single formulation (Brent equations) by introducing $\delta_{ij} \leftarrow bool(i = j)$.

$$
\sum^{u}_{v=1} r^{v}_{ij} p^{v}_{i'k} q^{v}_{k'j'} = \delta_{ii'} \delta_{jj'} \delta_{kk'}, \quad \forall i, i', j, j', k, k' \in [1, m]
\label{coef_combine}
$$

Interestingly, most attempts for solving Equations $(\ref{coef_combine})$ when $m = 2, 3$ obtain ternary solutions in the literature, i.e., $p^{v}_{ij}, q^{v}_{ij}, r^{v}_{ij} \in \{-1, 0, 1\}, \forall v \in [1, u], \forall i, j \in [1, m]$.


## SAT Encoding

### Relaxed Model

It leads to a straightforward encoding if we only consider the state of "is involved or not" for each coefficient.
...

### Complete Model

In order to encode Equations $(\ref{coef_exist})$ and $(\ref{coef_non_exist})$ into Boolean expressions, we need to use a pair of binary variables to simulate the ternary values.

$$
\begin{array}{lll}
r^{v+}_{ij} = \text{bool}(r^{v}_{ij} = 1)  \quad & p^{v+}_{ij} = \text{bool}(p^{v}_{ij} = 1)  \quad & q^{v+}_{ij} = \text{bool}(q^{v}_{ij} = 1)\\
r^{v-}_{ij} = \text{bool}(r^{v}_{ij} = -1) \quad & p^{v-}_{ij} = \text{bool}(p^{v}_{ij} = -1) \quad & q^{v-}_{ij} = \text{bool}(q^{v}_{ij} = -1)
\end{array}
, \quad \forall v \in [1, u], \forall i, j \in [1, m]
$$

Naturally, the values of these pairs of binary variables are exclusive with each other.

$$
\bigwedge_{\substack{v \in [1, u] \\ i, j \in [1, m]}} \left( (\neg r^{v+}_{ij} \vee \neg r^{v-}_{ij}) \wedge  (\neg p^{v+}_{ij} \vee \neg p^{v-}_{ij}) \wedge (\neg q^{v+}_{ij} \vee \neg q^{v-}_{ij}) \right)
\label{exclusive}
$$

For each cubic product term, it is non-zero iff all of the three variables are non-zero.

$$
\text{NonZero}(v, i, i', j, j', k, k') = (r^{v+}_{ij} \vee r^{v-}_{ij}) \wedge (p^{v+}_{i'k} \vee p^{v-}_{i'k}) \wedge (q^{v+}_{k'j'} \vee q^{v-}_{k'j'})
$$

For a non-zero term, its sign depends on if the number of $-1$ is even or odd.

$$
\text{OddNegOne}(v, i, i', j, j', k, k') = r^{v-}_{ij} \oplus p^{v-}_{i'k} \oplus q^{v-}_{k'j'}
$$

$$
\text{EvenNegOne}(v, i, i', j, j', k, k') = r^{v-}_{ij} \oplus p^{v-}_{i'k} \oplus q^{v-}_{k'j'} \oplus 1
$$

Then, the numbers of positive ones $\text{Ones}(i, i', j, j', k, k')$ and negative ones $\text{NegOnes}(i, i', j, j', k, k')$ in each equation among Equations $(\ref{coef_exist})$ and $(\ref{coef_non_exist})$ can be counted as follows.

$$
\text{NegOnes}(i, i', j, j', k, k') = \sum^{u}_{v=1} \big( \text{NonZero}(v, i, i', j, j', k, k') \wedge \text{OddNegOne}(v, i, i', j, j', k, k') \big)
$$

$$
\text{Ones}(i, i', j, j', k, k') = \sum^{u}_{v=1} \big( \text{NonZero}(v, i, i', j, j', k, k') \wedge \text{EvenNegOne}(v, i, i', j, j', k, k') \big)
$$

In order to make both sides equal in Equations $(\ref{coef_exist})$ and $(\ref{coef_non_exist})$, the number of the ones and the negative ones must satisfy the following criteria.

$$
\text{Ones}(i, i, j, j, k, k) - \text{NegOnes}(i, i, j, j, k, k) = 1, \quad \forall i, j, k \in [1, m]
\label{coef_exist_cnf}
$$

$$
\text{Ones}(i, i', j, j', k, k') = \text{NegOnes}(i, i', j, j', k, k'), \quad \forall i, i', j, j', k, k' \in [1, m], (i \ne i') \vee (j \ne j') \vee (k \ne k')
\label{coef_non_exist_cnf}
$$

Then, Equations $(\ref{coef_exist})$ and $(\ref{coef_non_exist})$ are encoded into Equations $(\ref{exclusive})$,  $(\ref{coef_exist_cnf})$, and $(\ref{coef_non_exist_cnf})$.


## Mixed-Integer Programming

### Formulating Exclusive Or Operator

It is easy to convert Equations $(\ref{exclusive})$,  $(\ref{coef_exist_cnf})$, and $(\ref{coef_non_exist_cnf})$ into an MIP model. The only obstacle is the XOR operator.
Let $o^{v}_{ii'jj'kk'} \in \{0, 1\}, \forall v \in [1, u], \forall i, i', j, j', k, k' \in [1, m]$ indicate whether $(r^{v-}_{ij} + p^{v-}_{i'k} + q^{v-}_{k'j'}) \in \{1, 3\}$, i.e., $o^{v}_{ii'jj'kk'} = \text{OddNegOne}(v, i, i', j, j', k, k')$. Then $1 - o^{v}_{ii'jj'kk'}$ indicate whether $(r^{v-}_{ij} + p^{v-}_{i'k} + q^{v-}_{k'j'}) \in \{0, 2\}$, i.e., $1 - o^{v}_{ii'jj'kk'} = \text{EvenNegOne}(v, i, i', j, j', k, k')$, since a number must be either even or odd.
Let $y^{v}_{ii'jj'kk'} \in \{0, 1\}, \forall v \in [1, u], \forall i, i', j, j', k, k' \in [1, m]$ be the auxiliary variables for linearizing the or logic.

$$
r^{v-}_{ij} + p^{v-}_{i'k} + q^{v-}_{k'j'} = 2 y^{v}_{ii'jj'kk'} + o^{v}_{ii'jj'kk'}, \quad \forall v \in [1, u], \forall i, i', j, j', k, k' \in [1, m]
$$

### Other Logical Operators

For the rest parts, $\text{NonZero}(v, i, i', j, j', k, k')$ can be easily replaced with variables $z^{v}_{ii'jj'kk'} \in \{0, 1\}, \forall v \in [1, u], \forall i, i', j, j', k, k' \in [1, m]$ by applying the technique that $\left( y = \bigwedge^{n}_{i=1} x_{i} \right) \Leftrightarrow \left( 0 \le \sum^{n}_{i=1} x_{i} - n y \le n - 1 \right)$ (note that $x, y, n, i$ in this sentence are irrelevant to other contents in this document).

$$
3 z^{v}_{ii'jj'kk'} \le r^{v+}_{ij} + r^{v-}_{ij} + p^{v+}_{i'k} + p^{v-}_{i'k} + q^{v+}_{k'j'} + q^{v-}_{k'j'} \le 2 + z^{v}_{ii'jj'kk'}, \quad \forall v \in [1, u], \forall i, i', j, j', k, k' \in [1, m]
\label{non_zero_var}
$$

Equations $(\ref{non_zero_var})$ are sufficient to derive $z^{v}_{ii'jj'kk'} = \text{NonZero}(v, i, i', j, j', k, k')$ due to Equations $(\ref{exclusive})$, i.e., we are summing 6 variables but using 3 as the bound.

We can formulate $x^{v-}_{ii'jj'kk'} = \text{NonZero}(v, i, i', j, j', k, k') \wedge \text{OddNegOne}(v, i, i', j, j', k, k')$ and $x^{v+}_{ii'jj'kk'} = \text{NonZero}(v, i, i', j, j', k, k') \wedge \text{EvenNegOne}(v, i, i', j, j', k, k')$ by the same technique.

$$
2 x^{v-}_{ii'jj'kk'} \le z^{v}_{ii'jj'kk'} + o^{v}_{ii'jj'kk'} \le 1 + x^{v-}_{ii'jj'kk'}, \quad \forall v \in [1, u], \forall i, i', j, j', k, k' \in [1, m]
$$

$$
2 x^{v+}_{ii'jj'kk'} \le z^{v}_{ii'jj'kk'} + (1 - o^{v}_{ii'jj'kk'}) \le 1 + x^{v+}_{ii'jj'kk'}, \quad \forall v \in [1, u], \forall i, i', j, j', k, k' \in [1, m]
$$

### Cuts

There should be at least one negative term.

$$
\begin{align}
\sum^{u}_{v=1} \sum^{m}_{i=1} \sum^{m}_{j=1} r^{v-}_{ij} \ge 1\\
\sum^{u}_{v=1} \sum^{m}_{i=1} \sum^{m}_{k=1} p^{v-}_{ik} \ge 1\\
\sum^{u}_{v=1} \sum^{m}_{k=1} \sum^{m}_{j=1} q^{v-}_{kj} \ge 1
\end{align}
$$

### Complete Plain Model

| Variable              | Description                               | Remark |
| --------------------- | ----------------------------------------- | ------ |
| $r^{v+}_{ij}$         | $\text{bool}(r^{v}_{ij} = 1)$             |        |
| $r^{v-}_{ij}$         | $\text{bool}(r^{v}_{ij} = -1)$            |        |
| $p^{v+}_{ij}$         | $\text{bool}(p^{v}_{ij} = 1)$             |        |
| $p^{v-}_{ij}$         | $\text{bool}(p^{v}_{ij} = -1)$            |        |
| $q^{v+}_{ij}$         | $\text{bool}(q^{v}_{ij} = 1)$             |        |
| $q^{v-}_{ij}$         | $\text{bool}(q^{v}_{ij} = -1)$            |        |
|                       | define alias $\pi^{v}_{ii'jj'kk'} = r^{v}_{ij} p^{v}_{i'k} q^{v}_{k'j'}$ to be used later |  |
| $o^{v}_{ii'jj'kk'}$   | $\left( (o = 1) \Leftrightarrow (\pi \ne 1) \right) \wedge \left( (o = 0) \Leftrightarrow (\pi \ne -1) \right)$ | $o^{v}_{ii'jj'kk'} = \text{OddNegOne}(v, i, i', j, j', k, k')$ |
| $y^{v}_{ii'jj'kk'}$   | auxiliary variable                        |        |
| $z^{v}_{ii'jj'kk'}$   | $\text{bool}(\pi \ne 0)$  | $z^{v}_{ii'jj'kk'} = \text{NonZero}(v, i, i', j, j', k, k')$ |
| $x^{v+}_{ii'jj'kk'}$   | $\text{bool}(\pi = 1)$ | $x^{v+}_{ii'jj'kk'} = z^{v}_{ii'jj'kk'} \wedge e^{v}_{ii'jj'kk'}$ |
| $x^{v-}_{ii'jj'kk'}$   | $\text{bool}(\pi = -1)$   | $x^{v-}_{ii'jj'kk'} = z^{v}_{ii'jj'kk'} \wedge \neg e^{v}_{ii'jj'kk'}$ |

$$
r^{v+}_{ij}, r^{v-}_{ij}, p^{v+}_{ij}, p^{v-}_{ij}, q^{v+}_{ij}, q^{v-}_{ij} \in \{0, 1\}, \quad \forall v \in [1, u], \forall i, j \in [1, m]
$$

$$
e^{v}_{ii'jj'kk'}, y^{v}_{ii'jj'kk'}, z^{v}_{ii'jj'kk'}, x^{v+}_{ii'jj'kk'}, x^{v-}_{ii'jj'kk'} \in \{0, 1\}, \forall v \in [1, u], \forall i, i', j, j', k, k' \in [1, m]
$$

$$
\begin{align}
r^{v+}_{ij} + r^{v-}_{ij} \le 1, \quad \forall v \in [1, u], \forall i, j \in [1, m]\\
p^{v+}_{ij} + p^{v-}_{ij} \le 1, \quad \forall v \in [1, u], \forall i, j \in [1, m]\\
q^{v+}_{ij} + q^{v-}_{ij} \le 1, \quad \forall v \in [1, u], \forall i, j \in [1, m]
\end{align}
$$

$$
r^{v-}_{ij} + p^{v-}_{i'k} + q^{v-}_{k'j'} = 2 y^{v}_{ii'jj'kk'} + o^{v}_{ii'jj'kk'}, \quad \forall v \in [1, u], \forall i, i', j, j', k, k' \in [1, m]
$$

$$
3 z^{v}_{ii'jj'kk'} \le r^{v+}_{ij} + r^{v-}_{ij} + p^{v+}_{i'k} + p^{v-}_{i'k} + q^{v+}_{k'j'} + q^{v-}_{k'j'} \le 2 + z^{v}_{ii'jj'kk'}, \quad \forall v \in [1, u], \forall i, i', j, j', k, k' \in [1, m]
$$

$$
2 x^{v-}_{ii'jj'kk'} \le z^{v}_{ii'jj'kk'} + o^{v}_{ii'jj'kk'} \le 1 + x^{v-}_{ii'jj'kk'}, \quad \forall v \in [1, u], \forall i, i', j, j', k, k' \in [1, m]
$$

$$
2 x^{v+}_{ii'jj'kk'} \le z^{v}_{ii'jj'kk'} + (1 - o^{v}_{ii'jj'kk'}) \le 1 + x^{v+}_{ii'jj'kk'}, \quad \forall v \in [1, u], \forall i, i', j, j', k, k' \in [1, m]
$$

$$
\sum^{u}_{v=1} x^{v+}_{iijjkk} - \sum^{u}_{v=1} x^{v-}_{iijjkk} = 1, \quad \forall i, j, k \in [1, m]
$$

$$
\sum^{u}_{v=1} x^{v+}_{ii'jj'kk'} - \sum^{u}_{v=1} x^{v-}_{ii'jj'kk'} = 0, \quad \forall i, i', j, j', k, k' \in [1, m], (i \ne i') \vee (j \ne j') \vee (k \ne k')
$$

### Linear Pattern Picking Model

Instead of determine the pattern of $u$ intermediate matrices, we are going to select $u$ intermediate matrices from a pool of $w$ distinct patterns.
In this way, $p^{v}_{ik}, q^{v}_{kj}$ become constants instead of decision variables, and we can regard $r^{v}_{ij}$ as continuous variables whose domains are $[\alpha, \beta]$.
A candidate pattern $v$ is selected if its coefficient is non-zero in any one of the linear expressions for calculating $\boldsymbol{c}_{ij}$.
Let $r^{v} \in \{0, 1\}, \forall v \in [1, w]$ indicate whether intermediate matrix $v$ is used somewhere.

$$
\min \sum^{w}_{v=1} r^{v}
\label{min_m_num}
$$

$$
\sum^{w}_{v=1} r^{v} \le u
\label{m_num_ub}
$$

$$
\alpha r^{v} \le r^{v}_{ij} \le \beta r^{v}, \quad \forall v \in [1, w], \forall i, j \in [1, m]
$$

$$
\sum^{w}_{v=1} p^{v}_{i'k} q^{v}_{k'j'} r^{v}_{ij} = \delta_{ii'} \delta_{jj'} \delta_{kk'}, \quad \forall i, i', j, j', k, k' \in [1, m]
$$

Note that the model is complete if it includes either one of Equations $(\ref{min_m_num})$ and $(\ref{m_num_ub})$.

### Quadratic Pattern Picking Model

Another alternative is to select $m^{2}$ patterns from a pool of $\mu$ linear expressions composed of intermediate matrices.
In other words, $r^{v}_{ij}$ become constants but $p^{v}_{ik}, q^{v}_{kj}$ are still variables.

However, there is a huge problem that the number of columns are too big.
Let $d$ be the size of the domain, e.g., $d = 3$ if $r^{v}_{ij}, p^{v}_{ik}, q^{v}_{kj} \in \{-1, 0, 1\}, \forall v \in [1, u], \forall i, j, k \in [1, m]$.

- Naive complexity.
  - The combination of $\boldsymbol{p}$ produces $d^{m^{2}} - 1$ columns.
  - The combination of $\boldsymbol{q}$ produces $d^{m^{2}} - 1$ columns.
  - The combination of $\boldsymbol{r}$ produces $d^{u} - 1$ columns.
- Empirical complexity.
  - 3x3 case.
    - Assume each sum in each intermediate matrix consists of up to 8 terms.
      - $\sum^{8}_{k=1} ((d - 1)^{k} C^{k}_{m^{2}})$ columns for $\boldsymbol{p}$ and $\boldsymbol{q}$, respectively.
    - Assume each resulting sub-matrix contains up to 8 intermediate matrices.
      - $\sum^{8}_{k=1} ((d - 1)^{k} C^{k}_{u})$ columns for $\boldsymbol{r}$.

The distribution of the column number by term number is as follows.

|        | 1    | 2    | 3     | 4      | 5       | 6       | 7        | 8         | 9         |
| ------ | ---- | ---- | ----- | ------ | ------- | ------- | -------- | --------- | --------- |
| $p, q$ | 18   | 144  | 672   | 2016   | 4032    | 5376    | 4608     | 2304      | 512       |
| $r$    | 46   | 1012 | 14168 | 141680 | 1076768 | 6460608 | 31380096 | 125520384 | 418401280 |

So, how to skip useless patterns so that the pool size will not blow up?
Or, can we iteratively identify useful columns and lazily add them to the model?

Maybe we could focus on the symmetric patterns?
Even if this is a promising direction, how do we define symmetric patterns?

Column generation?
The LP relaxation of the master problem could be extremely poor.


## CNF Encoding

### Encoding Exclusive Or

In order to utilize the state-of-the-art SAT solvers, we need to encode this problem in CNF, where the XOR operators are forbidden.

$$
\begin{align}
r \oplus p \oplus q & \iff (r \wedge p \wedge q) \vee (r \wedge \neg p \wedge \neg q) \vee (\neg r \wedge p \wedge \neg q) \vee (\neg r \wedge \neg p \wedge q)\\
                    & \iff (r \vee p \vee q) \wedge (r \vee \neg p \vee \neg q) \wedge (\neg r \vee p \vee \neg q) \wedge (\neg r \vee \neg p \vee q)
\label{xor2cnf}
\end{align}
$$

Then, we introduce an auxiliary variable $o$ for each XOR expression $r \oplus p \oplus q$ so that we can refer to its result with $o$ instead of applying Equation $(\ref{xor2cnf})$ everywhere.

$$
\begin{align}
(a \rightarrow b)     & \iff (\neg a \vee b)\\
(a \leftrightarrow b) & \iff (\neg a \vee b) \wedge (a \vee \neg b)\\
(a \leftrightarrow b) & \iff (a \odot b) \iff (a \wedge b) \vee (\neg a \wedge \neg b)
\end{align}
$$

$$
\begin{align}
o = r \oplus p \oplus q & \iff (\neg o \vee (r \oplus p \oplus q)) \wedge (o \vee \neg (r \oplus p \oplus q))\\
                        & \iff (o \wedge (r \oplus p \oplus q)) \vee (\neg o \wedge \neg (r \oplus p \oplus q))
\end{align}
$$

### Encoding Counter and Comparer Circuits

...

### Complete CNF Expression

...


## To-do List

- [ ] Encoder.
- [ ] Checker.
  - [ ] Correctness.
  - [ ] Equivalence or isomorphism.


## Future Works

- Try greater $u$.
- Try to minimize the number of addition operations.
- Try to use relax-and-fix method.
  - First determine each term is involved or not, then decide their coefficients.
- Try wider domains for the parameters.
  - Especially power of two, e.g., $..., \pm 4, \pm 2, \pm 0.5, \pm 0.25, ...$
- Try to let the calculation of the intermediates can utilize other intermediates.
  - There must not be cyclic references.
- Try to break the symmetry among different solutions.
  - By swapping the occurrences and the definitions of a pair of intermediate matrices $M_{v}$ and $M_{v'}$, a homogenous solution is found.
    - For each pair of $v, v' \in [1, u]$, do the value swap as $r^{v}_{ij} \leftrightarrow r^{v'}_{ij}, p^{v}_{ik} \leftrightarrow p^{v'}_{ik}, q^{v}_{kj} \leftrightarrow q^{v'}_{kj}, \forall i, j, k \in [1, m]$.
  - By performing additive inverse to the occurrences and the definitions of a intermediate matrix, a homogenous solution is found.
    - For each $v \in [1, u]$, reverse the signs as $r^{v}_{ij} \leftarrow -r^{v}_{ij}, p^{v}_{ik} \leftarrow -p^{v}_{ik}, q^{v}_{kj} \leftarrow -q^{v}_{kj}, \forall i, j, k \in [1, m]$.
- Try to use the symmetry within single solution.
  - There is a corresponding index pattern $(\boldsymbol{a}_{ij} + ... +\boldsymbol{a}_{i'j'})$ if there is a index pattern $(\boldsymbol{b}_{ij} + ... + \boldsymbol{b}_{i'j'})$ in Strassen's construction.
    - This assumption does not hold in 3x3 case.



# Unused Materials

## Another Perspective

Let us review the $\boldsymbol{r}, \boldsymbol{p}$, $\boldsymbol{q}$ in matrix forms.

$$
\boldsymbol{r}^{v} =
\begin{bmatrix}
  r^{v}_{11} & r^{v}_{12} & \cdots & r^{v}_{1m} \\
  r^{v}_{21} & r^{v}_{22} & \cdots & r^{v}_{2m} \\
  \vdots  & \vdots  & \ddots & \vdots  \\
  r^{v}_{m1} & r^{v}_{m2} & \cdots & r^{v}_{mm} 
\end{bmatrix}
\quad
\boldsymbol{p}^{v} =
\begin{bmatrix}
  p^{v}_{11} & p^{v}_{12} & \cdots & p^{v}_{1m} \\
  p^{v}_{21} & p^{v}_{22} & \cdots & p^{v}_{2m} \\
  \vdots  & \vdots  & \ddots & \vdots  \\
  p^{v}_{m1} & p^{v}_{m2} & \cdots & p^{v}_{mm} 
\end{bmatrix}
\quad
\boldsymbol{q}^{v} =
\begin{bmatrix}
  q^{v}_{11} & q^{v}_{12} & \cdots & q^{v}_{1m} \\
  q^{v}_{21} & q^{v}_{22} & \cdots & q^{v}_{2m} \\
  \vdots  & \vdots  & \ddots & \vdots  \\
  q^{v}_{m1} & q^{v}_{m2} & \cdots & q^{v}_{mm} 
\end{bmatrix}
$$

Then, we can rewrite $\boldsymbol{M}_{v}$ in a simpler way.

$$
\boldsymbol{M}_{v} = \text{Tr}\left( \boldsymbol{p}^{v} \boldsymbol{A}^{T} \right) \text{Tr}\left( \boldsymbol{q}^{v} \boldsymbol{B}^{T} \right)
$$

## Trace

Considering the trace of the product of three matrices $\boldsymbol{A} \boldsymbol{B} \boldsymbol{C}$ where $\boldsymbol{A}$ is $\alpha \times \beta$, $\boldsymbol{B}$ is $\beta \times \gamma$, and $\boldsymbol{C}$ is $\gamma \times \alpha$.

$$
\begin{align}
\text{Tr}(\boldsymbol{A} \boldsymbol{B} \boldsymbol{C}) & = \text{Tr}\left(
\begin{bmatrix}
  a_{11} & a_{12} & \cdots & a_{1 \beta} \\
  a_{21} & a_{22} & \cdots & a_{2 \beta} \\
  \vdots  & \vdots  & \ddots & \vdots  \\
  a_{\alpha 1} & a_{\alpha 2} & \cdots & a_{\alpha \beta} 
\end{bmatrix}
\begin{bmatrix}
  b_{11} & b_{12} & \cdots & b_{1 \gamma} \\
  b_{21} & b_{22} & \cdots & b_{2 \gamma} \\
  \vdots  & \vdots  & \ddots & \vdots  \\
  b_{\beta 1} & b_{\beta 2} & \cdots & b_{\beta \gamma} 
\end{bmatrix}
\begin{bmatrix}
  c_{11} & c_{12} & \cdots & c_{1 \alpha} \\
  c_{21} & c_{22} & \cdots & c_{2 \alpha} \\
  \vdots  & \vdots  & \ddots & \vdots  \\
  c_{\gamma 1} & c_{\gamma 2} & \cdots & c_{\gamma \alpha} 
\end{bmatrix}
\right)
\\
& = \text{Tr}\left(
\begin{bmatrix}
  \sum\limits^{\beta}_{j=1} a_{1j} b_{j1} & \sum\limits^{\beta}_{j=1} a_{1j} b_{j2} & \cdots & \sum\limits^{\beta}_{j=1} a_{1j} b_{j \gamma} \\
  \sum\limits^{\beta}_{j=1} a_{2j} b_{j1} & \sum\limits^{\beta}_{j=1} a_{2j} b_{j2} & \cdots & \sum\limits^{\beta}_{j=1} a_{2j} b_{j \gamma} \\
  \vdots  & \vdots  & \ddots & \vdots  \\
  \sum\limits^{\beta}_{j=1} a_{\alpha j} b_{j1} & \sum\limits^{\beta}_{j=1} a_{\alpha j} b_{j2} & \cdots & \sum\limits^{\beta}_{j=1} a_{\alpha j} b_{j \gamma} 
\end{bmatrix}
\begin{bmatrix}
  c_{11} & c_{12} & \cdots & c_{1 \alpha} \\
  c_{21} & c_{22} & \cdots & c_{2 \alpha} \\
  \vdots  & \vdots  & \ddots & \vdots  \\
  c_{\gamma 1} & c_{\gamma 2} & \cdots & c_{\gamma \alpha} 
\end{bmatrix}
\right)
\\
& = \text{Tr}\left(
\begin{bmatrix}
  \sum\limits^{\beta}_{j=1} \sum\limits^{\gamma}_{k=1} a_{1j} b_{jk} c_{k1} & \cdots & \cdots & \cdots \\
  \vdots & \sum\limits^{\beta}_{j=1} \sum\limits^{\gamma}_{k=1} a_{2j} b_{jk} c_{k2} & \cdots & \cdots \\
  \vdots  & \vdots  & \ddots & \vdots  \\
  \vdots & \vdots & \cdots & \sum\limits^{\beta}_{j=1} \sum\limits^{\gamma}_{k=1} a_{\alpha j} b_{jk} c_{k \alpha} 
\end{bmatrix}
\right)
\\
& = \sum^{\alpha}_{i=1} \sum^{\beta}_{j=1} \sum^{\gamma}_{k=1} a_{ij} b_{jk} c_{ki}
\end{align}
$$
