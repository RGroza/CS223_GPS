#include "trackpoint.h"

#include <stdlib.h>
#include <math.h>

struct _trackpoint
{
  location *loc;
  double time;
};

struct _track_segment
{
  trackpoint start;
  trackpoint end;
};

trackpoint *trackpoint_create(location *loc, double time)
{
  trackpoint *pt = malloc(sizeof(*pt));
  if (pt != NULL)
    {
      pt->loc = loc;
      pt->time = time;
    }
  return pt;
}


void trackpoint_destroy(trackpoint *pt)
{
  if (pt != NULL)
    {
      location_destroy(pt->loc);
      free(pt);
    }
}

double trackpoint_get_latitude(const trackpoint *pt)
{
  return location_get_latitude(pt->loc);
}


double trackpoint_get_longitude(const trackpoint *pt)
{
  return location_get_longitude(pt->loc);
}


const location *trackpoint_get_location(const trackpoint *pt)
{
  return pt->loc;
}


double trackpoint_get_time(const trackpoint *pt)
{
  return pt->time;
}


int trackpoint_compare(const trackpoint *p1, const trackpoint *p2)
{
  if (p1->time < p2->time)
    {
      return -1;
    }
  else if (p1->time > p2->time)
    {
      return 1;
    }
  else
    {
      return 0;
    }
}


track_segment *track_segment_create(trackpoint *p1, trackpoint *p2)
{
  track_segment *seg = malloc(sizeof(*seg));
  if (seg != NULL)
    {
      seg->start = *p1;
      seg->end = *p2;
    }
  return seg;
}


void track_segment_destroy(track_segment *seg)
{
  free(seg);
}


/**
 * Returns the location along the given segment at the given time.  If
 * the time is not the time of one of the endpoints, then the location
 * is determined by linear interpolation separately on the latitides
 * and the longitudes.  The return value will be NULL if there is an
 * allocation error creating the new location.  It is the caller's
 * responsibility to destroy the returned location if it is not NULL.
 *
 * @param seg a pointer to a valid track segment
 * @param time a timestamp between the timestamps of the segment endpoints
 * (inclusive)
 * @return a pointer to the interpolated location
 */
location *track_segment_get_location(const track_segment *seg, double time)
{
  return location_interpolate(seg->start.loc, seg->end.loc, (time - seg->start.time) / (seg->end.time - seg->start.time));
}


double track_segment_closest(const track_segment *seg1, const track_segment *seg2)
{
  // make an array containing endpoint times so we can iterate through them
  double times[] = {seg1->start.time, seg2->start.time, seg1->end.time, seg2->end.time};
  
  double min = INFINITY;
  for (size_t t = 0; t < 4; t++)
    {
      if (times[t] >= seg1->start.time
      && times[t] <= seg1->end.time
      && times[t] >= seg2->start.time
      && times[t] <= seg2->end.time)
	{
	  // interpolate locations (one of these will be a segment endpoint)
	  location *loc1 = track_segment_get_location(seg1, times[t]);
	  location *loc2 = track_segment_get_location(seg2, times[t]);

	  // compute distance and keep track of min so far
	  double dist = location_distance(loc1, loc2);
	  if (dist < min)
	    {
	      min = dist;
	    }
	  location_destroy(loc1);
	  location_destroy(loc2);
	}
    }
      
  return min;
}
