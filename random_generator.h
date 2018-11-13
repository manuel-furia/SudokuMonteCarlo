/*
 * Random generator functions header file
 * 
 * Monte Carlo methods in Physics
 * 
 * Created by Ville Jansson, PhD <ville.b.c.jansson@gmail.com> 2016-02-27.
 *
 */



/* Initialization of the random number generator with seed */
void random_generator_initialization();

/* Generates random integer [0, 2^63-1] */
long long random_int();

/* Generates random double on [0,1]-real-interval */
double random_double1();

/* Generates random double on [0,1)-real-interval */
double random_double2();

/* Generates random double on (0,1)-real-interva */
double random_double3();

/* Generate integers on interval [0,N] */
int random_n(int N);