/*
 * CSC 369 Fall 2010 - Assignment 1
 *
 * $Id$
 */

#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "rv.h"

/* ====================================================
 * Returns a geometric distributed non-negative integer.
 * The geometric distribution is a discrete analog to the
 * exponential distribution.
 * NOTE: use 0.0 < p < 1.0
 * ====================================================
 */
long Geometric(double p) {
  double k = random() / (double)INT32_MAX;
  return ((long) (log(1.0 - k) / log(p)));
}

/* ===================================================================
 * Returns an equilikely distributed integer between a and b inclusive. 
 * NOTE: use a < b
 * ===================================================================
 */
long Equilikely(long a, long b) {
  return (a + (long) ((b - a + 1) * (random() / (double)INT32_MAX)));
}
