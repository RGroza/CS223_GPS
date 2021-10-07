#ifndef __TRACK_H__
#define __TRACK_H__

#include <stdio.h>
#include <stdbool.h>

#include "location.h"
#include "trackpoint.h"

typedef struct _track track;


/**
 * Creates an empty GPS track.
 * 
 * @return a pointer to the track, or NULL if allocation was unsuccessful
 */
track *track_create();


/**
 * Destroys the given track.  The track is invalid after it has been destroyed.
 * There is no effect if the parameter is NULL.
 *
 * @param l a pointer to a track
 */
void track_destroy(track *l);


/**
 * Returns the number of trackpoints on the given track.
 *
 * @param t a pointer to a valid track, non-NULL
 * @return the number of points in t
 */
size_t track_size(const track *t);


/**
 * Add a the given trackpoint to the end of the given track.  The
 * add is successful if the time of the new trackpoint is strictly
 * after the time of the last trackpoint in the track (or that track
 * was empty) and there were no allocation errors.  If the add was
 * successful, then ownership of the trackpoint is transferred
 * to the track.
 *
 * @param t a pointer to a track, non-NULL
 * @param p a pointer to a trackpoint, non-NULL
 * @return true if and only if the point was successfully added
 */
bool track_add_point(track *t, trackpoint *p);


/**
 * Returns the time between the first trackpoint and the last trackpoint in
 * the given track, in seconds.  Returns zero if the track is empty.
 *
 * @param t a pointer to a track, non-NULL
 * @return the time between the first and last trackpoints on that track
 */
double track_length(const track *t);


/**
 * Returns a copy of the location of the trackpoint on this track with
 * the given time.  If there is no such trackpoint then the returned
 * location is determined by linear interpolation between the first
 * trackpoints before and after the given time.  If the track is empty
 * or if the given time is strictly before the time of the first
 * trackpoint or strictly after the time of the last trackpoint, then
 * the returned value is NULL.  If there is an error allocating the
 * location to return then the returned value is NULL.  Otherwise, it
 * is the caller's responsibility to destroy the returned location.
 *
 * @param t a pointer to a track, non-NULL
 * @param time a double
 * @return a copy of the trackpoint with the given time, or a new interpolated
 * trackpoint, or NULL
 */
location *track_get(const track *t, double time);


/**
 * Passes each point in the given track to the given function.  Points
 * are passed to the function in increasing chronological order.
 *
 * @param t a pointer to a valid track, non-NULL
 * @param f the function to call for each point
 * @param arg an extra argument to pass to the function
 */
void track_for_each(const track *t, void (*f)(const trackpoint *, void *), void *arg);


/**
 * Merges the second tracks into the first track.  All points from the
 * second track are added to the first track, except that a trackpoint
 * already present in the first track is not duplicated if also
 * present in the second track, and conflicting trackpoints (two trackpoints,
 * one from the first track and one from the second, with the same
 * timestamp but different locations) are removed from the merged
 * track and destroyed.  The second track is destroyed by
 * this operation, with all of its trackpoints either transferred to the
 * first track or destroyed.  The resulting first track has all trackpoints
 * in strictly increasing chronological order.
 *
 * @param dest a pointer to a track, non-NULL
 * @param src a pointer to a track, non-NULL
 */
void track_merge(track *dest, track *src);


/**
 * Returns an approximation of the closest approach of the two tracks.
 * The approximation is obtained by a process equivalent in result
 * to the following:
 * 1) get the timestamps of all trackpoints in both tracks combined
 * 2) eliminate those timestamps not between the start and end of both tracks
 * 3) determine the location at each remaining time on both tracks,
 *    where if one track has no trackpoint with a matching timestamp,
 *    the location on that track at that time is determined by
 *    linear interpolation on the latitude and longitude (separately)
 *    of the next earliest and next latest trackpoints
 * 4) for each pair of locations corresponding to match times,
 *    compute the distance between those locations using location_distance
 * 5) return the minimum of all the distances from the previous step,
 *    or INFINITY if there were no remaining timestamps after step 2
 * 
 * @param dest a pointer to a track, non-NULL
 * @param src a pointer to a track, non-NULL
 * @return a double giving the distance in km of the closest approach,
 * or INFINITY
 */
double track_closest_approach(const track *track1, const track *track2);

#endif
