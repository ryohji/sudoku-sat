/**
 * sudoku sat output interpreter.
 */
#include "sudoku-sat.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* make_format(char* buf);

int main() {
  int sudoku[N * N] = {0};
  char buf[64];

  scanf("%s", buf);
  if (strcmp(buf, "s") != 0) {
    printf("unexpected %s (not 's').", buf);
    return 1;
  }

  scanf("%s", buf);
  if (strcmp(buf, "SATISFIABLE") != 0) {
    printf("output is not 'SATICEFIABLE'. (%s)", buf);
    return 1;
  }

  while (scanf("%s", buf)) {
    if (strcmp(buf, "v") != 0) {
      index_t i = atoi(buf);
      if (i == 0) {
        break;
      } else {
        if (i > 0) {
          predicate_t pred = predicate_from(i);
          sudoku[pred.col + N * pred.row] = pred.num + 1;
        }
      }
    }
  }

  const char* fmt = make_format((char[32]){0});
  for (int i = 0; i != N * N; i += 1) {
    printf(fmt, sudoku[i], (i + 1) % N ? ' ' : '\n');
  }

  return 0;
}

char* make_format(char* buf) {
  sprintf(buf, "%d", N);
  int width = strlen(buf);
  sprintf(buf, "%%%dd%%c", width);
  return buf;
}
