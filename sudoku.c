#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "random_generator.h"
#include "functions.h"

#define Point struct SPoint
#define SIM_SIZE 100
//#define Kb (0.000086173325)
#define PI (3.14159265359)

#define GRID_SIZE 81
#define MAX_STEPS 100000000

unsigned long long seed[4]	= {576ULL, 379775ULL, 8246ULL, 3675683ULL};

//unsigned long long seed[4]	= {5578ULL, 3786472ULL, 246ULL, 36765683ULL};

typedef enum GridFlags{

  DEFAULT = 0,
  FIXED = 1,
  EMPTY = 2
  

} GridFlags;


char          grid[GRID_SIZE];
GridFlags     grid_flags[GRID_SIZE];
int           last_swap1, last_swap2;
double        (*rndgen)();     //Random generator function to use

//Saw like function with the shape of a 4th power polynomial, oscillates 1000 times in MAX_STEPS
double osc_decay_1000(double x)
{
  if (MAX_STEPS < 1000)
  {
    return linear_decay(x);
  }
  else
  {
    return osc_decay(x, 1000);
  }
}

//Initialize the sudoku grid with empty cells
void init_grid()
{
  int i;
  
  for (i = 0; i < GRID_SIZE; i++)
  {
    grid[i] = 0;
    grid_flags[i] = EMPTY;
  }
}

//Read the initial grid from a file
//NOTE: The input digits (except empty cells) will be marked as unmovable,
//      so the simulation does not modify their position
void read_grid(char* filename)
{
  int c, i;
  FILE *file = fopen(filename, "r");
  
  if (file == NULL){printf("Error: the file '%s' does not exist.\n", filename); exit(0);}
  
  i = 0;
 
  while (i < GRID_SIZE && ((c = fgetc(file)) != EOF))
  {
    if (c >= '1' && c <= '9') //A number
    {
      grid[i] = c;
      grid_flags[i] = FIXED;
      i++;
    }
    else if (c == '.') //An empty cell
    {
      grid[i] = 0;
      grid_flags[i] = EMPTY;
      i++;
    }
    //Ignore any other character
  }
  
  fclose(file);
  
  if (i != GRID_SIZE) {printf("Error: Bad input file format.\n"); exit(0);}
}


//Check if the cell "index" is empty
int is_empty(int index)
{
  return (grid_flags[index] & EMPTY) != 0;
}

//Check if the cell "index" is fixed (non movable)
int is_fixed(int index)
{
  return (grid_flags[index] & FIXED) != 0;
}

//Return the index of a cell by providing the index of the big square(0..8) and
//the relative index of the cell inside the big square (0..8)
int index_by_square(int square, int cell)
{
  int sr, sc; //Square row, square column (can be 0, 1 or 2)
  int cr, cc; //Cell row, cell column (inside the square)
  int index;
  
  sr = square / 3;
  sc = square % 3;
  
  cr = cell / 3;
  cc = cell % 3;
  
  index = sr * (9*3) + sc * (3) + cr * (9) + cc;
  
  return index;
}

//Return the index of a cell by providing row and column indices
int index_by_rc(int row, int col)
{
  return row * 9 + col;
}

//Check for duplicates in a row(if is_row != 0) or in a column
//ind -> row or column index
int duplicates_in(int ind, int is_row)
{
  //Set the n-th element of counter to 1 if the digit n is found for the first time
  //if the element is already 1 then there is a duplicate
  int duplicates, index, countindex, counter[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  int i;
  
  duplicates = 0;
  
  for (i = 0; i < 9; i++)
  {
    if (is_row)
      index = index_by_rc(ind, i);
    else
      index = index_by_rc(i, ind);
      
    if (!is_empty(index))
    {
      countindex = (int)(grid[index] - '1');
      
      if (!counter[countindex]) 
        counter[countindex]++;
      else 
        duplicates++;
    }
  }
  
  return duplicates;
}

int duplicates_in_row(int row)
{
  return duplicates_in(row, 1);
}

int duplicates_in_column(int col)
{
  return duplicates_in(col, 0);
}

//Quality factor = sums of duplicates in every row and column
int quality_factor()
{
  int i, j, index;
  int sum;
  
  sum = 0;
  
  for (i = 0; i < 9; i++)
  {
    sum += duplicates_in_row(i);
    sum += duplicates_in_column(i);
  }

  return sum;
}

//Fill the sudoku randomly by inserting in random position the missing digits in every squares
void random_fill()
{
  int i, j, k, swap;
  char nums[9] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
  char missing[9], tmp;
  int mc; //missing count
  int is_missing;
  
  //Find all the digits that are missing in a big square
  for (i = 0; i < 9; i++)
  {
    mc = 0;
    
    for (j = 0; j < 9; j++)
    {
      is_missing = 1;
      
      for (k = 0; k < 9; k++)
      {
        if (grid[index_by_square(i, k)] == nums[j])
        {
          is_missing = 0;
        }
      }
      
      if (is_missing)
      {
        missing[mc] = j;
        mc++;
      }
    }
    
    //Shuffle the list of missing numbers
    for (j = 0; j < mc; j++)
    {
      swap = (int)(rndgen() * mc);
      tmp = missing[j];
      missing[j] = missing[swap];
      missing[swap] = tmp;
    }
    
    mc = 0;
    
    //Fill the empty places with the shuffled list
    for (j = 0; j < 9; j++)
    {
      if (is_empty(index_by_square(i, j)) && !is_fixed(index_by_square(i, j)))
      {
        grid[index_by_square(i, j)] = nums[missing[mc]];
        grid_flags[index_by_square(i, j)] = DEFAULT;
        mc++;
      }
    }
  }
}

//Swap two non-fixed elements in a square
void random_swap()
{
  double u1, u2, u3;
  int i, si, ci1, ci2, i1, i2;
  int fixed_count;
  int free_indices[9], fi;
  char tmp;
  
  //Randomly select a square with at least two movable elements
  do {
    u1 = rndgen();
    si = (int)(u1 * 9);
  
    fixed_count = 0; fi = 0;
    
    for (i = 0; i < 9; i++)
    {
      if (is_fixed(index_by_square(si, i)))
        fixed_count++;
      else
      {
        free_indices[fi] = i;
        fi++;
      }
    }
  
  //Select another square is this one is full of fixed elements
  //NOTE: To not be considered completely fixed, the square has to contain
  //      at least two movable elements (maximum 7 fixed), because if there
  //      is only one movable, there is no element to swap it with (only one possible configuration)
  } while (fixed_count > 7); 
  
  //fi now contains the number of free indices (elements that can be moved)
  
  //Randomly get two movable indices (i1 and i2) and swap them:
  u2 = rndgen();
  ci1 = free_indices[(int)(u2 * fi)];
  i1 = index_by_square(si, ci1);
  
  u3 = rndgen();
  ci2 = free_indices[(int)(u3 * fi)];
  i2 = index_by_square(si, ci2);   
  
  tmp = grid[i1];
  grid[i1] = grid[i2];
  grid[i2] = tmp;
  
  //Save the swapping to allow the undo operation
  last_swap1 = i1;
  last_swap2 = i2;
}

//Undo the last swapping
void undo_last_swap()
{
  char tmp;
  
  tmp = grid[last_swap1];
  grid[last_swap1] = grid[last_swap2];
  grid[last_swap2] = tmp;
}

//Check if the change in configuration will be accepted
//1 -> Accept the change
//0 -> Refuse the change
int check(int dQ, double T)
{
  double u = rndgen();

  if (dQ <= 0)
  {
    return 1;
  }  
  
  if (T == 0) return 0;
  
  return u < exp(-((double)dQ) / (T)) ? 1 : 0;
}


//Print the sudoku grid in a nice conciguration
void print()
{
  int i, j;
  char c;
  
  printf("-------------------------------\n");
  
  for (i = 0; i < 9; i++)
  {    
    for(j = 0; j < 9; j++)
    {
      if (j % 3 == 0) printf("|");
      c = grid[index_by_rc(i, j)];
      printf(" ");
      printf("%c", c == 0 ? ' ' : c);
      printf(" ");
    }  
    printf("|\n");
    
    if ((i+1) % 3 == 0) 
      printf("-------------------------------\n");
  }
  
}

//Apply the simulated annealing to solve the sudoku
void solve(int N_maxsteps, double T_scale, double (*T_func)(double))
{
  int i, nr;
  int q_a, q_b, deltaq;
  double x;
  
  nr=0; //Number of rejected transitions
  
  for (i = 0; i < N_maxsteps; i++)
  {
  
    q_b = quality_factor();
    
    if (q_b == 0) break;
    
    //Trial transition:
    //Swap two movable elements in a square
    random_swap(); 
    
    q_a = quality_factor();
    
    //Every million iterations print something that shows that the program is working
    if (i % 1000000 == 0) printf("Iteration: %d; Quality factor: %d\n", i, q_b);
    
    deltaq = q_a - q_b;
    
    x = ((double) i) / ((double) N_maxsteps);

    //NOTE: Temperature := T_scale * T_func(x)
    //NOTE: x := current step / total max steps
    if (!check(deltaq, T_scale * T_func(x)))
    {
      //Check failed, reject the state
      undo_last_swap();
      nr++;
    }
    
  }
  
  if (i == N_maxsteps && q_b != 0)
  {
    //We reached the end without finding a solution
    printf("Failed to find a solution after the maximum amount of steps (%d).\n", N_maxsteps);
    printf("Final quality factor: %d\n", q_b);
  }
  else
  {
    //Solution found!
    printf("Solution found at iteration number: %d\n", i);
  }
  
  printf("Number of rejected transition: %d\n", nr);
  
}

int main(int argc, char* argv[])
{
  double (*T_func)(double);
  double T_scale;
  char temp_func_choice;
  char filename[1000];

  //Set the Mersenne-Twister in the range (0, 1) as rndgen
  rndgen = &random_double3;
  random_generator_initialization();
  
  if (argc != 4)
  {
    printf("Usage: /sudoku filename T_scale T_func\n");
    printf("T_func (decay function of the temperature) can be:\n");
    printf("s -> sqrt\n");
    printf("1 -> linear\n");
    printf("2 -> quadratic\n");
    printf("3 -> cubic\n");
    printf("4 -> 4th power\n");
    printf("o -> 4th power sawtooth shape, with 4-th power dampening\n");
    printf("And the temperature function used will be T(t) = T_scale * T_func(x)\n");
    printf("Use 'o' for the most difficult sudokus\n");
    exit(0);
  }
  
  //Read the inputs
  sscanf(argv[1], "%s", &filename);
  sscanf(argv[2], "%lf", &T_scale);
  sscanf(argv[3], "%c", &temp_func_choice);
  
  //Choose the function that the temperature will follow in simulation time
  switch (temp_func_choice)
  {
    case 's':
      T_func = &sqrt_decay;
    break;
    
    case '1':
      T_func = &linear_decay;
    break;
    
    case '2':
      T_func = &quadratic_decay;
    break;
    
    case '3':
      T_func = &cubic_decay;
    break;
    
    case '4':
      T_func = &quartic_decay;
    break;
    
    case 'o':
      //Oscillate in a sawtooth pattern, with global dampening
      //For details see the function osc_decay in functions.c
      T_func = &osc_decay_1000;
    break;
    
    default:
      T_func = &linear_decay;
    break;
      
  }
  
  //We have all the data we need, execute the simulation
  init_grid();
  read_grid(filename);
  print();
  printf("\n\n");
  random_fill();
  solve(MAX_STEPS, T_scale, T_func);
  print();
}

