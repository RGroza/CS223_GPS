#ifndef __MORE_MATH_H__
#define __MORE_MATH_H__

/**
 * Declarations of our functions are now in a separate header (.h)
 * file, which ONLY contains the declarations of the functions.  We
 * can now #include this header file in other files where we want to
 * use these functions.
 *
 * The #ifndef (if not defined), #define, and #endif ensure that this
 * header file is included only once (some things that go in header
 * files cause compiler errors if they are repeated, even if the
 * repeats are identical).  Without them, if x.c included y.h and z.h
 * and y.h also includes z.h, then x.c would include z.h twice -- once
 * through y.h and again explicity.  With them, when y.h includes z.h,
 * the guard (what's after the #ifndef) is undefined and so the code
 * between #ifndef and #endif is included.  That #define-s the guard,
 * so the second time z.h the #ifndef is false since the guard was defined
 * the first time z.h was included, and the code between #ifndef and #endif
 * is skipped.
 */

/**
 * Returns the colatitude of the given latitude.  The colatitude is the
 * degrees from the north pole to the given latitude.
 * 
 * @param lat a double between -90 and 90
 * @return the colatitude
 */
double colatitude(double lat);

/**
 * Returns given angle converted from degrees to radians.
 * 
 * @param a an angle measured in degrees
 * @return the angle in radians
 */
double to_radians(double a);

#endif
