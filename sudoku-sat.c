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

typedef struct indices indices_t;

/** bunch of the indices. to return as the value of functions */
struct indices {
  index_t index[N];
};

/** read the values of sudoku */
static int* read_sudoku(FILE* fp, int sudoku[N * N]);

/** count the number of constraints of specified sudoku */
static int count_constraint(const int sudoku[N * N]);

/**
 * convert specified sudoku to the constraint into specified FILE
 *
 * @param fp is the FILE, converted constraint output to.
 * @param sudoku is the sudoku problem to solve.
 * @param n is the column offset, previous output ended on.
 * @return the column offset this function output ended on.
 */
static int to_constraint(FILE* fp, const int sudoku[N * N], int n);

/**
 * convert given indices to `at least one` constraint into specified FILE
 *
 * @param fp is the FILE, converted constraint output to.
 * @param indices turned into `at least one` constraint.
 * @param n is the column offset, previous output ended on.
 * @return the column offset this function output ended on.
 */
static int at_least_one_of(FILE* fp, indices_t indices, int n);

/**
 * convert given indices to `at most one` constraint into specified FILE
 *
 * @param fp is the FILE, converted constraint output to.
 * @param indices turned into `at most one` constraint.
 * @param n is the column offset, previous output ended on.
 * @return the column offset this function output ended on.
 */
static int at_most_one_of(FILE* fp, indices_t indices, int n);

static indices_t num_for(int row, int col);
static indices_t row_for(int row, int num);
static indices_t col_for(int col, int num);
static indices_t box_for(int box, int num);

static int fput_index(FILE* fp, index_t i, int n);

int main() {
  int pairs[N * N][2];
  int(*p)[2] = pairs;
  for (int y = 0; y < N; y += 1) {
    for (int x = 0; x < N; x += 1) {
      (*p)[0] = x;
      (*p)[1] = y;
      p += 1;
    }
  }

  int* sudoku = read_sudoku(stdin, (int[N * N]){0});

  fprintf(stdout, "p cnf %d %d\n", N * N * N,
          count_constraint(sudoku) + N * N * (1 + nC2) * 4);

  int c = to_constraint(stdout, sudoku, 0);

  for (p = pairs; p != END_OF(pairs); p += 1) {
    indices_t indices = num_for((*p)[0], (*p)[1]);
    c = at_least_one_of(stdout, indices, c);
    c = at_most_one_of(stdout, indices, c);
  }

  for (p = pairs; p != END_OF(pairs); p += 1) {
    indices_t indices = row_for((*p)[0], (*p)[1]);
    c = at_least_one_of(stdout, indices, c);
    c = at_most_one_of(stdout, indices, c);
  }

  for (p = pairs; p != END_OF(pairs); p += 1) {
    indices_t indices = col_for((*p)[0], (*p)[1]);
    c = at_least_one_of(stdout, indices, c);
    c = at_most_one_of(stdout, indices, c);
  }

  for (p = pairs; p != END_OF(pairs); p += 1) {
    indices_t indices = box_for((*p)[0], (*p)[1]);
    c = at_least_one_of(stdout, indices, c);
    c = at_most_one_of(stdout, indices, c);
  }

  return 0;
}

int* read_sudoku(FILE* fp, int buffer[N * N]) {
  int* p = buffer;
  while (p != buffer + N * N) {
    fscanf(fp, "%d", p);
    p += 1;
  }
  return buffer;
}

int count_constraint(const int buffer[N * N]) {
  int n = 0;
  for (const int* p = buffer; p != buffer + N * N; p += 1) {
    n += 0 < *p && *p <= N;
  }
  return n;
}

int to_constraint(FILE* fp, const int sudoku[N * N], int col) {
  for (int i = 0; i < N * N; i += 1) {
    const int n = sudoku[i];
    if (0 < n && n <= N) {
      col = fput_index(fp, index_of(i / N, i % N, n - 1), col);
      col = fput_index(fp, 0, col);
    }
  }
  return col;
}

int at_least_one_of(FILE* fp, indices_t indices, int n) {
  for (int i = 0; i < N; i += 1) {
    n = fput_index(fp, indices.index[i], n);
  }
  return fput_index(fp, 0, n);
}

int at_most_one_of(FILE* fp, indices_t indices, int n) {
  for (int i = 1; i < N; i += 1) {
    for (int j = i; j < N; j += 1) {
      n = fput_index(fp, -indices.index[i - 1], n);
      n = fput_index(fp, -indices.index[j], n);
      n = fput_index(fp, 0, n);
    }
  }
  return n;
}

static indices_t make_indeices(void* cookie, index_t (*calculate)(void*, int)) {
  indices_t v;
  for (int i = 0; i < N; i += 1) {
    v.index[i] = calculate(cookie, i);
  }
  return v;
}

static index_t calculate_num(void* cookie, int i) {
  return index_of(((int*)cookie)[0], ((int*)cookie)[1], i);
}

indices_t num_for(int row, int col) {
  return make_indeices((int[2]){row, col}, calculate_num);
}

static index_t calculate_row(void* cookie, int i) {
  return index_of(((int*)cookie)[0], i, ((int*)cookie)[1]);
}

indices_t row_for(int row, int num) {
  return make_indeices((int[2]){row, num}, calculate_row);
}

static index_t calculate_col(void* cookie, int i) {
  return index_of(i, ((int*)cookie)[0], ((int*)cookie)[1]);
}

indices_t col_for(int col, int num) {
  return make_indeices((int[2]){col, num}, calculate_row);
}

static index_t calculate_box(void* cookie, int i) {
  const int* params = (int*)cookie;
  const int n = params[3];
  return index_of(params[0] + i / n, params[1] + i % n, params[2]);
}

indices_t box_for(int box, int num) {
  const int n = (int)sqrt(N);
  return make_indeices((int[4]){box / n * n, box % n * n, num, n},
                       calculate_box);
}

static char* make_format();
static char* refer_format();
static char format_[16];
static char* (*format)() = make_format;

int fput_index(FILE* fp, index_t i, int col) {
  char s[16];
  int n = sprintf(s, format(), i);
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

char* make_format() {
  format = refer_format;
  sprintf(format_, "%d", -N * N * N);
  sprintf(format_, "%%%lud", strlen(format_));
  return format_;
}

char* refer_format() {
  return format_;
}
