#ifndef SUDOKU_SAT_H_
#define SUDOKU_SAT_H_

#ifndef N
/** THE WIDTH OF SUDOKU PROBLEM */
#define N 9
#endif

/** The type of variable index */
typedef int index_t;

/**
 * Calculate the variable index
 *
 * means "the specified `number` is placed at the `column` of the `row`."
 */
static inline index_t index_of(int row, int col, int num) {
  return 1 + col + N * (row + N * num);
}

/** Specified number placed at the column of the row */
typedef struct predicate predicate_t;
struct predicate {
  int row;
  int col;
  int num;
};

/** Get the predicate from respective index */
static inline predicate_t predicate_from(index_t index) {
  return (predicate_t){.col = (index - 1) % N,
                       .row = (index - 1) / N % N,
                       .num = (index - 1) / N / N % N};
}

#endif
