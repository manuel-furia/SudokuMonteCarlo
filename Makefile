CC=gcc
CCFLAGS=-O3
LIBS=-lm


EXE1=sudoku
OBJS1=mt19937-64.o random_generator.o sudoku.o functions.o

all: $(EXE1)

mt19937-64.o:		mt19937-64.c mt64.h
random_generator.o:	random_generator.c seed.h random_generator.h
random_walker.o:	sudoku.c mt64.h random_generator.h 
functions.o: functions.c functions.h

.PHONY: $(EXE1)

$(EXE1): $(OBJS1) .ALWAYSEXEC
	$(CC) $(CCFLAGS) $(OBJS1) -o $@ $(LIBS)

%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	-/bin/rm -f $(EXE1) a.out *.o 

.ALWAYSEXEC:
