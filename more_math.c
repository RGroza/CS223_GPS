#include "more_math.h"

/* The #include above isn't necessary here since we don't use the functions
 * in this file outside of their definitions, but it doesn't hurt and you'd
 * usually #include the corresponding .h file in a .c file just in case the
 * functions in the .c file call each other.
 */

double colatitude(double lat)
{
  return 90 - lat;
}

double to_radians(double a)
{
  return (3.1415926535897932384 / 180.0) * a;
}
