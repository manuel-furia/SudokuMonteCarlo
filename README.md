# SudokuMonteCarlo
Monte carlo simulation to solve sudoku. Written in C in 2016 as a university assignment.

The random number generator used was provided as part of the course. See the files mt64.h, mt19937-64.c, random_generator.h and random_generator.c for more info and copyright notice from the original authors.


-- Algorithm:

The program solves most of the sudoku puzzles by annealing simulation.
It can solve normal difficulty sudokus in 1000 to 1000000 steps, and really hard sudokus in less than 100 millions steps if the right temperature is found.
The sudoku is read from a file, and the empty spaces are filled with the missing numbers in each square, in random positions.
NOTE: In this way, a square can not have duplicates, but a row or a columns can have duplicates.
The transition used consists in swapping two elements in a square.
The quality function is the sum of the duplicates in every row and column, and the aim is to minimize that number to 0.

-- How to use:

./sudoku filename T_scale T_func

T_func (decay function of the temperature) can be:
s -> sqrt
1 -> linear
2 -> quadratic
3 -> cubic
4 -> 4th power
o -> 4th power sawtooth shape, with 4-th power dampening
And the temperature function used will be T(t) = T_scale * T_func(x)
Use 'o' for the most difficult sudokus.

-- What does the program show:

At first, it prints the sudoku as it is read from the file. Then runs the algorithm and print the solution (and the number of steps used) if found. It also prints the number of steps and quality function every 1 million steps.
If a solution is not found, the program keeps going up to 100 million steps.
It also always prints the number of rejected transitions, which is helpful for understanding how the temperature is affecting the algorithm.

