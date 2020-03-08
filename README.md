# Sudoku converter for SAT solver
Convert given sudoku problem into DIMACS format which is common input of SAT solvers.

## Modules
This repository contains 2 modules: `sudoku-sat` and `sudoku-sat-interp`.

### sudoku-sat
Makes DIMACS from given sudoku (read from stdin).

for example:
```
$ ./sudoku-sat <<EOF >problem.dimacs
0 0 0 0 0 5 0 8 0
0 0 3 0 7 0 4 0 0
0 5 0 9 0 0 0 0 6
0 0 6 2 0 0 0 0 9
0 0 0 0 4 0 7 0 0
1 0 0 0 0 6 0 0 0
4 0 0 0 0 9 6 0 0
0 9 0 8 0 0 0 2 0
0 0 1 0 3 0 0 0 0
EOF
$ 
```

Generated output is ready for use the input of SAT solver. (`minisat`, `picosat`, etc.)

### sudoku-sat-interp
Interpret SAT solver's output into solved sudoku.

for example:
```
$ cat problem.dimacs |picosat |./sudoku-sat-interp
9 7 4 6 3 5 2 8 1
8 6 3 2 7 1 4 9 5
2 5 1 9 8 4 7 3 6
7 4 6 2 8 5 3 1 9
9 8 5 3 4 1 7 6 2
1 3 2 9 7 6 8 5 4
4 8 7 5 2 9 6 3 1
6 9 3 8 7 1 5 2 4
5 2 1 6 3 4 9 8 7
$ 
```

## How to build
Simply use `make`.

You can get ANY size converter defining `N` preprocessor. For example, if you want to get 16 x 16 sized sudoku DIMACS generator and interpreter:
```
$ make clean && make CPPFLAGS=-DN=16
```
