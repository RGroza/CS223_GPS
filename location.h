#ifndef __LOCATION_H__
#define __LOCATION_H__

#include <stdbool.h>

typedef struct _location location;

/**
 * Creates a location with the given latitude and longitude.  If the
 * latitude is invalid or there is a memory allocation error then the
 * return value is NULL.  It is the caller's responsibility to
 * eventually destroy the returned location if it is not NULL.
 * 
 * @param lat a value between -90 and 90, inclusive
 * @param lon a longitude
 * @return a pointer to the new location, or NULL
 */
location *location_create(double lat, double lon);


/**
 * Destroys the given location.  There is no effect if the parameter is NULL.
 *
 * @param loc a pointer to a location, or NULL
 */
void location_destroy(location *loc);


/**
 * Returns the latitude of the given location.
 *
 * @param loc a pointer to a location, non-NULL
 * @return the latitude of loc
 */
double location_get_latitude(const location *loc);


/**
 * Returns the longitude of the given location.
 *
 * @param loc a pointer to a location, non-NULL
 * @return the longitude of loc
 */
double location_get_longitude(const location *loc);


/**
 * Returns a location interpolated by the given amount between the two
 * given locations.  The latitude and longitude are interoplated
 * separately using linear interpolation between the two locations.
 * The return value is NULL if there is a memory allocation error
 * creating the new location. It is the caller's responsibility to
 * eventually destroy the returned location if it is not NULL.
 *
 * @param start a pointer to a location, non-NULL
 * @param end a pointer to a location, non-NULL
 * @param a double between 0.0 and 1.0 inclusive for the weight of the two
 * endpoints: 0.0 for the start only and 1.0 for the end only
 * @return a pointer to the interpolated location, or NULL
 */
location *location_interpolate(const location *start, const location *end, double w);


/**
 * Compares two locations by longitude, breaking ties by latitude.
 * The return value is positive if the first location is greater
 * than the second (has a greater longitude), negative if the first
 * is lesser, and 0 if they are equal.
 * 
 * @param l1 a pointer to a location, non-NULL
 * @param l2 a pointer to a location, non-NULL
 * @return an integer whose sign reflects the result of the comparison
 */
int location_compare(const location *l1, const location *l2);


/**
 * Returns the distance between the two locations on the Earth's surface,
 * assuming a spherical earth with radius 6378.1 km.
 *
 * @param l1 a pointer to a location, non-NULL
 * @param l2 a pointer to a location, non-NULL
 * @return the distance between those points
 */
double location_distance(const location *l1, const location *l2);
double location_distance_spherical(const location *l1, const location *l2);
double location_distance_oblate(const location *l1, const location *l2);


/**
 * Determines if the given location is valid.  A location is valid if the
 * latitude is between -90 and 90 degrees.
 *
 * @param l a pointer to a location, non-NULL
 * @return true if the location is valid, false otherwise
 */
bool location_validate(const location *l);

#endif
