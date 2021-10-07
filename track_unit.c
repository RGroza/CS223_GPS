#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "track.h"

/**
 * Prints the location and timestamp of the given trackpoint
 * to standard output.
 *
 * @param p a pointer to a trackpoint, non-NULL
 */
void print_point(const trackpoint *p, void *);


/**
 * Creates a track and adds trackpoints to it using data in the
 * given array, adding the time shift to all the timestamps.
 * If there is an error creating the track or adding
 * the points then the return value is NULL.  If the return value
 * is not NULL, then it is the caller's responsibility to eventually
 * destroy the returned track.

 * @param n a nonnegative integer
 * @param an array of doubles of at least n rows with valid latitudes
 *        in column 0, valid longitudes in column 1, and strictly
 *        increasing times in column 2
 * @param time_shift a double
 */
track *build_track(size_t n, double pts[][3], double time_shift);


/**
 * Unit tests for the functions in the track module.
 * 
 * @param n a nonnegative integer
 * @param an array of doubles of at least n rows with valid latitudes
 *        in column 0, valid longitudes in column 1, and strictly
 *        increasing times in column 2
 */
void test_create_destroy_empty();
void test_add_get_exact(size_t n, double pts[][3]);
void test_add_fail(size_t n, double pts[][3]);
void test_size(size_t n, double pts[][3]);
void test_length(size_t n, double pts[][3]);
void test_get_interpolate(size_t n, double pts[][3], double start, double end, double delta);
void test_get_out_of_bounds(size_t n, double pts[][3]);
void test_for_each(size_t n, double pts[][3]);
void test_merge(size_t dest_n, double dest_pts[][3], size_t src_n, double src_pts[][3], double src_time_shift);
void test_closest(size_t dest_n, double dest_pts[][3], size_t src_n, double src_pts[][3]);
void test_timing_merge(size_t k, bool on);


int main(int argc, char *argv[])
{
  if (argc < 2)
    {
      fprintf(stderr, "USAGE: %s test-number [n 1|0]\n", argv[0]);
      return 1;
    }
  
  int n;
  bool on;
  if (argc >= 4)
    {
      // extra arguments are n and on/off for timing tests
      n = atoi(argv[2]);
      on = strcmp(argv[3], "1") == 0;

      if (n <= 0)
	{
	  fprintf(stderr, "%s: n must be positive\n", argv[0]);
	  return 1;
	}
    }

  double points[][3] = {
    {47.755155, -122.263435, 1000000},
    {47.639156, -122.338966, 1000300},
    {47.636959, -122.336048, 1000310}, 
    {47.639966, -122.334160, 1000320},
    {47.753770, -122.266010, 1000620}
  };
  size_t num_points = 5;

  double points2[][3] = {
    {47.640196, -122.335160, 1000315},
    {47.639966, -122.334160, 1000320},
    {47.763770, -122.267530, 1000630}
  };
  size_t num_points2 = 3;
      
  double points3[][3] = {
    {47.762155, -122.613435, 1000000},
    {47.757156, -122.628966, 1000300},
    {47.691959, -122.532048, 1000310}, 
    {47.714966, -122.535160, 1000320},
    {47.711770, -122.566010, 1000620}
  };
  size_t num_points3 = 5;

  int test = atoi(argv[1]);

  switch (test)
    {
    case 0:
      test_create_destroy_empty();
      break;

    case 1:
      test_add_get_exact(num_points, points);
      break;

    case 2:
      test_add_fail(1, points);
      break;
      
    case 3:
      test_size(num_points, points);
      break;

    case 4:
      test_length(num_points, points);
      break;

    case 5:
      // length of empty track
      test_length(0, points);
      break;

    case 6:
      test_get_interpolate(num_points, points, 1000290.0, 1000327.5, 12.5);
      break;

    case 7:
      test_get_out_of_bounds(num_points, points);
      break;

    case 8:
      test_for_each(num_points, points);
      break;

    case 9:
      // disjoint merge: first 3 from above and last 2, no skew
      test_merge(3, points, 2, points + 3, 0);
      break;

    case 10:
      // overlap but distinct timestamps (first 3 and last 3 shifted by 1)
      test_merge(3, points, 3, points + 2, 1);
      break;

    case 11:
      // duplicate points (points and points2)
      test_merge(num_points, points, num_points2, points2, 0);
      break;
      
    case 12:
      // conflicting points (first 3 and last 3 shifted by -20)
      test_merge(3, points, 3, points + 2, -20);
      break;

    case 13:
      // same timestamps on both tracks
      test_closest(num_points, points, num_points3, points3);
      break;

    case 100:
      // merge timing test to come
      test_timing_merge(n, on);
      break;
      
    default:
      fprintf(stderr, "%s: invalid test number %s\n", argv[0], argv[1]);
      return 1;
    }

  return 0;
}

void print_point(const trackpoint *p, void *info)
{
  printf("%.6f %.6f %.1f\n",
	 trackpoint_get_latitude(p),
	 trackpoint_get_longitude(p),
	 trackpoint_get_time(p));
}

void test_create_destroy_empty()
{
  track *trk = track_create();
  if (trk == NULL)
    {
      printf("FAILED: could not initialize track\n");
      return;
    }
  track_destroy(trk);
  printf("PASSED\n");
}

void test_add_get_exact(size_t n, double pts[][3])
{
  track *trk = build_track(n, pts, 0);
  if (trk == NULL)
    {
      printf("FAILED: could not initialize track\n");
      return;
    }

  for (size_t i = 0; i < n; i++)
    {
      location *loc = track_get(trk, pts[i][2]);
      if (loc == NULL)
	{
	  printf("FAILED: could not get trackpoint at time %lf\n", pts[i][2]);
	  track_destroy(trk);
	  return;
	}
      else
	{
	  printf("%.6lf %.6lf %1lf\n",
		 location_get_latitude(loc),
		 location_get_longitude(loc),
		 pts[i][2]);
	  location_destroy(loc);
	}
    }
  track_destroy(trk);
}


void test_get_interpolate(size_t n, double pts[][3], double start, double end, double delta)
{
  track *trk = build_track(n, pts, 0);
  if (trk == NULL)
    {
      printf("FAILED: could not initialize track\n");
      return;
    }

  for (double t = start; t <= end; t += delta)
    {
      location *loc = track_get(trk, t);
      if (loc == NULL)
	{
	  printf("FAILED: could not interpolate at time %lf\n", t);
	  track_destroy(trk);
	  return;
	}
      else
	{
	  printf("%.5lf %.5lf %1lf\n",
		 location_get_latitude(loc), location_get_longitude(loc), t);
	  location_destroy(loc);
	}
    }
  track_destroy(trk);
}


void test_get_out_of_bounds(size_t n, double pts[][3])
{
  track *trk = build_track(n, pts, 0);
  if (trk == NULL)
    {
      printf("FAILED: could not initialize track\n");
      return;
    }

  location *loc = track_get(trk, pts[0][2] - 1.0);
  if (loc != NULL)
    {
      // want NULL rather than extrapolated point
      printf("FAILED: extrapolated to time time %lf\n", pts[0][2] - 1.0);
      location_destroy(loc);
    }
  else
    {
      printf("PASSED\n");
    }

  track_destroy(trk);
}


void test_add_fail(size_t n, double pts[][3])
{
  track *trk = build_track(n, pts, 0);
  if (trk == NULL)
    {
      printf("FAILED: could not initialize track\n");
      return;
    }

  location *loc = location_create(pts[n - 1][0], pts[n - 1][1]);
  if (loc == NULL)
    {
      printf("FAILED: could not allocate location\n");
      track_destroy(trk);
      return;
    }

  trackpoint *pt = trackpoint_create(loc, pts[n - 1][2] - 1);
  if (pt == NULL)
    {
      printf("FAILED: could not initialize trackpoint\n");
      location_destroy(loc);
      track_destroy(trk);
      return;
    }

  if (track_add_point(trk, pt))
    {
      printf("FAILED: added trackpoint with invalid timestamp\n");
    }
  else
    {
      printf("PASSED\n");
      trackpoint_destroy(pt);
    }
  
  track_destroy(trk);
}

void test_size(size_t n, double pts[][3])
{
  track *trk = build_track(n, pts, 0);
  if (trk == NULL)
    {
      printf("FAILED: could not initialize track\n");
      return;
    }

  if (track_size(trk) != n)
    {
      printf("FAILED: track_size reports %zu after adding %zu points\n",
	     track_size(trk),
	     n);
    }
  else
    {
      printf("PASSED\n");
    }
  track_destroy(trk);
}

void test_length(size_t n, double pts[][3])
{
  double correct_length = n > 0 ? pts[n - 1][2] - pts[0][2] : 0.0;
  
  track *trk = build_track(n, pts, 0);
  if (trk == NULL)
    {
      printf("FAILED: could not initialize track\n");
      return;
    }

  if (track_length(trk) != correct_length)
    {
      printf("FAILED: track_length reports %lf for track of length %lf\n",
	     track_length(trk),
	     correct_length);
    }
  else
    {
      printf("PASSED\n");
    }
  track_destroy(trk);
}

void test_for_each(size_t n, double pts[][3])
{
  track *trk = build_track(n, pts, 0);
  if (trk == NULL)
    {
      printf("FAILED: could not initialize track\n");
      return;
    }

  track_for_each(trk, print_point, NULL);
  
  track_destroy(trk);
}


track *build_track(size_t n, double pts[][3], double time_shift)
{
  track *trk = track_create();
  if (trk != NULL)
    {
      for (size_t i = 0; i < n; i++)
	{
	  location *loc = location_create(pts[i][0], pts[i][1]);
	  if (loc == NULL)
	    {
	      track_destroy(trk);
	      return NULL;
	    }
	  trackpoint *pt = trackpoint_create(loc, pts[i][2] + time_shift);
	  if (pt == NULL)
	    {
	      location_destroy(loc);
	      track_destroy(trk);
	      return NULL;
	    }
	  if (!track_add_point(trk, pt))
	    {
	      trackpoint_destroy(pt);
	      track_destroy(trk);
	      return NULL;
	    }
	}
    }
  
  return trk;
}


void test_merge(size_t dest_n, double dest_pts[][3], size_t src_n, double src_pts[][3], double src_time_shift)
{
  track *dest = build_track(dest_n, dest_pts, 0);
  track *src = build_track(src_n, src_pts, src_time_shift);
  if (dest == NULL || src == NULL)
    {
      track_destroy(dest);
      track_destroy(src);
      printf("FAILED: could not initialize track\n");
      return;
    }

  track_merge(dest, src);
  track_for_each(dest, print_point, NULL);
  track_destroy(dest);
  // src destroyed by merge
}


void test_closest(size_t n1, double pts1[][3], size_t n2, double pts2[][3])
{
  track *trk1 = build_track(n1, pts1, 0);
  track *trk2 = build_track(n2, pts2, 0);
  if (trk1 == NULL || trk2 == NULL)
    {
      track_destroy(trk1);
      track_destroy(trk2);
      printf("FAILED: could not initialize track\n");
      return;
    }
  printf("%.1lf\n", track_closest_approach(trk1, trk2));

  track_destroy(trk1);
  track_destroy(trk2);
}


void test_timing_merge(size_t k, bool on)
{
  // This will be executed for several different values of k,
  // counting instructions executed using valgrind.
  // For each n, we run the test once without running the
  // function in question in order to get a baseline instruction count,
  // and then with running the function in question so we can see
  // how many more instructions it takes to run the function.
  // Another program takes the instruction counts and divides them
  // by the target asymptotic running time.  If your function
  // does not meet the target, then that quotient will get bigger and
  // bigger as n increases and will eventually exceed a carefully
  // chosen cutoff, which triggers a failure notice.

  // k is the size of the overlap; we will build two tracks of size k^2
  // that overlap in their last k points in order to easily distinguish
  // an O(n+m) = O(k^2+k^2) = O(k^2) implementation from an O(k)
  // implementation.

  track *dest = track_create();
  track *src = track_create();
  if (dest == NULL || src == NULL)
    {
      printf("FAILED -- could not allocate tracks\n");
      track_destroy(dest);
      track_destroy(src);
      return;
    }

  for (size_t i = 0; i < k * k; i++)
    {
      location *loc1 = location_create(41.6, -76.7);
      trackpoint *pt1 = loc1 != NULL ? trackpoint_create(loc1, i) : NULL;
      if (pt1 != NULL)
	{
	  track_add_point(dest, pt1);
	}
      else
	{
	  if (pt1 != NULL)
	    {
	      trackpoint_destroy(pt1);
	    }
	  else
	    {
	      location_destroy(loc1);
	    }
	}
      
      location *loc2 = location_create(41.6, -76.7);
      trackpoint *pt2 = loc2 != NULL ? trackpoint_create(loc2, i + k * k - k) : NULL;
      if (pt2 != NULL)
	{
	  track_add_point(src, pt2);
	}
      else
	{
	  if (pt2 != NULL)
	    {
	      trackpoint_destroy(pt2);
	    }
	  else
	    {
	      location_destroy(loc2);
	    }
	}
    }

  if (track_size(dest) != k * k || track_size(src) != k * k)
    {
      printf("FAILED -- could not add points\n");
      track_destroy(dest);
      track_destroy(src);
      return;
    }
  
  if (on)
    {
      track_merge(dest, src);
      // src is destroyed by the merge
      if (track_size(dest) != 2 * k * k - k)
	{
	  printf("FAILED -- merged track has wrong size %zu != %zu\n",
		 track_size(dest),
		 2 * k * k - k);
	}
    }
  else
    {
      track_destroy(src);
    }
  
  track_destroy(dest);
}
