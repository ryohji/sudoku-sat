/**
 * sudoku sat DIMACS builder
 */
#include "sudoku-sat.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define nC2 (N * (N - 1) / 2)

#define LINE_WIDTH 76

/** caluculate the end of the array */
#define END_OF(a) ((a) + LENGTH_OF(a))

/** caluculate the length of the array */
#define LENGTH_OF(a) (sizeof(a) / sizeof((a)[0]))

/** bunch of the indices. to return as the value of functions */
typedef struct indices indices_t;
struct indices {
  index_t index[N];
};

/** read the numbers of sudoku */
static int* read_sudoku(FILE* fp, int sudoku[N * N]);

/** count the number of constraints of specified sudoku */
static int count_constraint(const int sudoku[N * N]);

/**
 * convert specified sudoku to the constraint into specified FILE
 *
 * @param fp is the FILE, converted constraint output to.
 * @param n is the column offset, previous output ended on.
 * @param sudoku is the sudoku problem to solve.
 * @return the column offset this function output ended on.
 */
static int write_specific_constraint(FILE* fp, int n, const int sudoku[N * N]);

/** write sudoku universal constraint into specified FILE */
static int write_generic_constraint(FILE* fp, int n);

/**
 * entry point
 *
 * read sudoku and convert it into DIMACS
 */
int main() {
  const int* sudoku = read_sudoku(stdin, (int[N * N]){0});

  // output DIMACS problem definition line
  fprintf(stdout, "p cnf %d %d\n", N * N * N,
          count_constraint(sudoku) + N * N * (1 + nC2) * 4);

  // convert sudoku into DIMACS
  int c = write_specific_constraint(stdout, 0, sudoku);
  (void)write_generic_constraint(stdout, c);

  return 0;
}

/*
 reader part
 */
int* read_sudoku(FILE* fp, int buffer[N * N]) {
  int* p = buffer;
  while (p != buffer + N * N) {
    fscanf(fp, "%d", p);
    p += 1;
  }
  return buffer;
}

/*
 writer part
 */
int count_constraint(const int buffer[N * N]) {
  int n = 0;
  for (const int* p = buffer; p != buffer + N * N; p += 1) {
    n += 0 < *p && *p <= N;
  }
  return n;
}

/** current column aware formatter of index_t */
static int fput_index(FILE* fp, int n, index_t i);

// fixed number gives powerful constraint.
int write_specific_constraint(FILE* fp, int col, const int sudoku[N * N]) {
  for (int i = 0; i < N * N; i += 1) {
    const unsigned n = sudoku[i];
    if (n - 1 < N) {
      predicate_t pred = {.row = i / N, .col = i % N, .num = n - 1};
      col = fput_index(fp, col, index_of(pred));
      col = fput_index(fp, col, 0);
    }
  }
  return col;
}

/*
 generic constraint constructed with 4 part; cell, row, column, and box.
 */
static int write_constraints_of(FILE* fp, int n, indices_t (*)(int, int));

static indices_t cell_for(int row, int col);
static indices_t row_for(int row, int num);
static indices_t col_for(int col, int num);
static indices_t box_for(int box, int num);

int write_generic_constraint(FILE* fp, int n) {
  n = write_constraints_of(stdout, n, cell_for);
  n = write_constraints_of(stdout, n, row_for);
  n = write_constraints_of(stdout, n, col_for);
  n = write_constraints_of(stdout, n, box_for);
  return n;
}

/*
 each generic constraint constraint builder; it makes exactly one proposition
 true. in another word, it choose at least one proposition and at most one
 proposition.
 */
static void pair_initialize_exec(int (*pairs)[2]);
static int at_least_one_of(FILE* fp, int n, indices_t indices);
static int at_most_one_of(FILE* fp, int n, indices_t indices);

int write_constraints_of(FILE* fp, int n, indices_t (*calculate)(int, int)) {
  static int pairs[N * N][2];
  static void (*pair_initialize)(int(*pairs)[2]) = pair_initialize_exec;
  if (pair_initialize) {
    pair_initialize(pairs);
    pair_initialize = NULL;
  }

  for (int(*p)[2] = pairs; p != END_OF(pairs); p += 1) {
    indices_t indices = calculate((*p)[0], (*p)[1]);
    n = at_least_one_of(stdout, n, indices);
    n = at_most_one_of(stdout, n, indices);
  }

  return n;
}

void pair_initialize_exec(int (*pairs)[2]) {
  int(*p)[2] = pairs;
  for (int y = 0; y < N; y += 1) {
    for (int x = 0; x < N; x += 1) {
      (*p)[0] = x;
      (*p)[1] = y;
      p += 1;
    }
  }
}

/** convert given indices into `at least one` constraint */
int at_least_one_of(FILE* fp, int n, indices_t indices) {
  for (int i = 0; i < N; i += 1) {
    n = fput_index(fp, n, indices.index[i]);
  }
  return fput_index(fp, n, 0);
}

/** convert given indices into `at most one` constraint */
int at_most_one_of(FILE* fp, int n, indices_t indices) {
  for (int i = 1; i < N; i += 1) {
    for (int j = i; j < N; j += 1) {
      n = fput_index(fp, n, -indices.index[i - 1]);
      n = fput_index(fp, n, -indices.index[j]);
      n = fput_index(fp, n, 0);
    }
  }
  return n;
}

static indices_t make_indices(void* cookie, index_t (*calculate)(void*, int)) {
  indices_t v;
  for (int i = 0; i < N; i += 1) {
    v.index[i] = calculate(cookie, i);
  }
  return v;
}

static index_t calculate_num(void* cookie, int i) {
  return index_of((predicate_t){
      .row = ((int*)cookie)[0], .col = ((int*)cookie)[1], .num = i});
}

indices_t cell_for(int row, int col) {
  return make_indices((int[2]){row, col}, calculate_num);
}

static index_t calculate_row(void* cookie, int i) {
  return index_of((predicate_t){
      .row = ((int*)cookie)[0], .col = i, .num = ((int*)cookie)[1]});
}

indices_t row_for(int row, int num) {
  return make_indices((int[2]){row, num}, calculate_row);
}

static index_t calculate_col(void* cookie, int i) {
  return index_of((predicate_t){
      .row = i, .col = ((int*)cookie)[0], .num = ((int*)cookie)[1]});
}

indices_t col_for(int col, int num) {
  return make_indices((int[2]){col, num}, calculate_row);
}

static index_t calculate_box(void* cookie, int i) {
  const int* params = (int*)cookie;
  const int n = params[3];
  return index_of((predicate_t){
      .row = params[0] + i / n, .col = params[1] + i % n, .num = params[2]});
}

indices_t box_for(int box, int num) {
  const int n = (int)sqrt(N);
  return make_indices((int[4]){box / n * n, box % n * n, num, n},
                      calculate_box);
}

int fput_index(FILE* fp, int col, index_t i) {
  static char format[16] = {0};
  if (!format[0]) {
    sprintf(format, "-%d", N * N * N);
    sprintf(format, "%%%lud", strlen(format));
  }

  char s[16];
  int n = sprintf(s, format, i);
  if (col != 0) {
    int c;
    if (col + 1 + n > LINE_WIDTH) {
      c = '\n';
    } else {
      c = ' ';
      n += col + 1;
    }
    fputc(c, fp);
  }
  fputs(s, fp);
  return n;
}
