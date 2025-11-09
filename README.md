# Linear Regression in C

Since I'm studying machine learning, I'm curious how does machine learning
model work. After learning how one of the basic of machine learning model work,
the linear regression, I decided to challenge myself by building a linear
regression model in C. Why C? It's always fun to build something in C, the
memory management, the peformance, the difficulty, etc.
*The harder the process, the bigger the prize*.

## How Does This Work

All the source code are provided in `src/` folder. In short, the program will
**read a CSV file**, **convert it into a matrix** of float, and then
**calculate the intercept and slope** assuming this is a linear data
**using gradient descent**.

> [!NOTE]
> Unfortunately, the gradient descent is not great to handle large different
> between the intercept and the slope. 1 learning rate is used for both
> intercept and slope, but if those variable has a big gap, it can be a stable
> rate for one of the variable but can be unstable/bad to the other that makes
> the calculation to divergence.

## Prerequisites/Installation

Install the C/C++ compiler to your computer, either GCC or LLVM, and clone this
repository to your computer.

## Usage

To compile the program, type one of these command to the terminal.

For GCC:
```bash
gcc src/main.c -o main
```

For LLVM:
```bash
clang src/main.c -o main
```

> [!TIP]
> To optimize the peformance consider using `-O` flag.

And then run the program using with one of these command.

For Windows:
```bash
./main.exe <csv-file> <target-column-index>
```

For Linux:
```bash
./main <csv-filepath> <target-column-index>
```

The program accept 2 argument, the CSV file path, and target column index.

> [!WARNING]
> The program should take **less than 10 seconds** to finish full iteration. If the
> progam takes longer than expected, **that could indicate** the gradient descent
> **divergence**, if so terminate the program and consider to tweak the 
> **learning rate** and **maximum iteration**.

The program prints the linear model in the following form:
```
{intercept} + {slope-col-1}{var-col-1} + ... + {slope-col-n}{var-col-n}
```

Where {var-col-x} represents an automatically assigned variable name (a, b, c, …) corresponding to each feature column from left to right, excluding the target column.

### Example

#### Command line:
```bash
gcc main.c -o main
./main input.csv 2
```

#### Output:
```
14.570945 + 1.468263a
```

## Future

If I have some free time, I might update the code to fix a few issues with the
gradient descent part 🤔.