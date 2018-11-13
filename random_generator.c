/*
 * Random generator functions
 * 
 * Monte Carlo methods in Physics
 * 
 * Created by Ville Jansson, PhD <ville.b.c.jansson@gmail.com> 2016-02-27.
 *
 */

#include <stdio.h>		// For fprintf and stderr
#include <stdlib.h>		// For exit
#include "mt64.h"		// Mersenne Twister random generator
#include "seed.h"	// Needed to get the global variable seed.





/** Functions **/

/* Define random number genrator functions. The idea is that the generators should be easy to change in this function,
 * so that not the whole code need to be altered. These functions are thus interfaces between the generator and the simulation
 * code. As generator, the Mersenne Twister will be used.
 */

/* Initialization of the random number generator with seed */
void random_generator_initialization() {
  int length = 4;
  init_by_array64(seed, length);	// Initialization of the Mersenne Twister
  //printf("# Random generator initialized with seed = %llu %llu %llu %llu\n",seed[0],seed[1],seed[2],seed[3]);
}

/* Generates random integer in the [0, 2^63-1] */
long long random_int() {
  return (long long) genrand64_int63();
}

/* Generates random double in the [0,1]-real-interval */
double random_double1() {
  return (double) genrand64_real1();
}

/* Generates random double in the [0,1)-real-interval */
double random_double2() {
  return (double) genrand64_real2();
}

/* Generates random double in the (0,1)-real-interval */
double random_double3() {
  return (double) genrand64_real3();
}

/* Generate integers in the interval [0,N] */
int random_n(int N) {
  int tmp;
  tmp = (int) (N + 1) * random_double2();
  return tmp;
}
