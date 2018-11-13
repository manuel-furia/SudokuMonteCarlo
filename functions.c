#include <math.h>
#include "functions.h"

#define PI 3.1415

double linear_decay(double x)
{
  return (1.0 - x);
}

double quadratic_decay(double x)
{
  return (1.0 - x) * (1.0 - x);
}

double cubic_decay(double x)
{
  return (1.0 - x) * (1.0 - x) * (1.0 - x);
}

double quartic_decay(double x)
{
  return (1.0 - x) * (1.0 - x) * (1.0 - x) * (1.0 - x);
}

double sqrt_decay(double x)
{
  return sqrt(1.0 - x);
}

double sqr(double x)
{
  return x * x;
}

double osc_decay(double x, double freq)
{
  double range = 1.0 / freq;
  double rem = fmod(x, range);
  
  return quartic_decay(x) * ((1.0 - rem * freq) * (1.0 - rem * freq) * (1.0 - rem * freq) * (1.0 - rem * freq));
}

