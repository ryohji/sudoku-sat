#ifndef SUDOKU_SAT_H_
#define SUDOKU_SAT_H_

#ifndef N
/** THE WIDTH OF SUDOKU PROBLEM */
#define N 9
#endif

/** The type of variable index */
typedef int index_t;

/**
 * Specified number placed at the column of the row
 *
 * means "the specified `number` is placed at the `column` of the `row`."
 * note: each field start with 0.
 */
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

/** Calculate the variable index */
static inline index_t index_of(const predicate_t p) {
  return 1 + p.col + N * (p.row + N * p.num);
}

#endif
