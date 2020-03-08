all: sudoku-sat sudoku-sat-interp

clean:
	$(RM) sudoku-sat sudoku-sat-interp *.o *~

sudoku-sat.o: sudoku-sat.c sudoku-sat.h

sudoku-sat-interp.o: sudoku-sat-interp.c sudoku-sat.h
