#ifndef __TRACKPOINT_H__
#define __TRACKPOINT_H__

#include "location.h"

typedef struct _trackpoint trackpoint;

typedef struct _track_segment track_segment;

/**
 * Creates a new trackpoint with the given location and timestamp.
 * Ownership of the location is transferred to the trackpoint if it
 * was created successfully.  If it was not created successfully then
 * the return value is NULL.
 *
 * @param loc a pointer to a location, non-NULL
 * @param time a double for time since the epoch
 */
trackpoint *trackpoint_create(location *loc, double time);


/**
 * Destroys the given trackpoint and the location in it.
 * There is no effect if the parameter is NULL.
 *
 * @param pt a pointer to a trackpoint
 */
void trackpoint_destroy(trackpoint *pt);


/**
 * Returns the latitude of the given trackpoint.
 *
 * @param pt a pointer to a trackpoint, non-NULL
 * @return the latitude of pt
 */
double trackpoint_get_latitude(const trackpoint *pt);


/**
 * Returns the longitude of the given trackpoint.
 *
 * @param pt a pointer to a trackpoint, non-NULL
 * @return the longitude of pt
 */
double trackpoint_get_longitude(const trackpoint *pt);


/**
 * Returns the location of the given trackpoint.  The trackpoint
 * retains ownership of the location.
 *
 * @param pt a pointer to a trackpoint, non-NULL
 * @return a pointer to the location of that trackpoint
 */
const location *trackpoint_get_location(const trackpoint *pt);


/**
 * Returns the time in the given trackpoint.
 *
 * @param pt a pointer to a trackpoint, non-NULL
 * @return the time recorded in that trackpoint
 */
double trackpoint_get_time(const trackpoint *pt);


/**
 * Compares two trackpoints based on time.  The return value is
 * negative if the first point is chronologically before the second,
 * positive if the second is before the first, and zero if their
 * times are the same.
 *
 * @param p1 a pointer to a trackpoint, non-NULL
 * @param p2 a pointer to a trackpoint, non-NULL
 * @return an integer with the same sign as the time of the first point
 * minus the sign of the second point
 */
int trackpoint_compare(const trackpoint *p1, const trackpoint *p2);


/**
 * Creates a track segment with the given endpoints.  The caller retains
 * ownership of the endpoints, and the locations of those endpoints must
 * remain valid in order for the segment to remain valid.
 * If the segment was not created successfully then the return value is NULL.
 *
 * @param p1 a pointer to a trackpoint, non-NULL
 * @param p2 a pointer to a trackpoint, non-NULL
 * @return a pointer to the new track segment, or NULL
 */
track_segment *track_segment_create(trackpoint *p1, trackpoint *p2);


/**
 * Destroys the given track segment and the trackpoints located in it.
 * There is no effect if the parameter is NULL.
 *
 * @param seg a pointer to a track segment, or NULL
 */
void track_segment_destroy(track_segment *seg);


/**
 * Returns an approximation of the closest approach of the two given track
 * segments.  The closest approach is the minimum over any of the four
 * endpoints within the time range of each segment of the distance between
 * the corresponding interpolated points on the segments.  Interpolation
 * is done by linear interpolation on the latitudes and longitudes
 * separately.  If there is no overlap in time between the two segments,
 * the return value is INFINITY.
 *
 * @param seg1 a pointer to a track segment, non-NULL
 * @param seg2 a pointer to a track segment, non-NULL
 * @return the distance in km of the closest approach, or INFINITY
 */
double track_segment_closest(const track_segment *seg1, const track_segment *seg2);


#endif
