#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "track.h"
#include "trackpoint.h"
#include "location.h"


void print_point(const trackpoint *p, void *info)
{
    printf("%.6f %.6f %.6f\n",
	    trackpoint_get_latitude(p),
	    trackpoint_get_longitude(p),
	    trackpoint_get_time(p));
}


int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "GPS: missing filename(s)\n");
        return 0;
    }

    FILE *input1 = fopen(argv[1], "r");
    FILE *input2 = fopen(argv[2], "r");

    if (input1 == NULL || input2 == NULL)
    {
        fprintf(stderr, "GPS: could not open file(s)\n");
        return 0;
    }

    double *lat = 0;
    double *lon = 0;
    double *time = 0;

    track *track1 = track_create();
    while (fscanf(input1, "%lf %lf %lf[^\n]", lat, lon, time) != EOF)
    {
        track_add_point(track1, trackpoint_create(location_create(*lat, *lon), *time));
    }

    track *track2 = track_create();
    while (fscanf(input2, "%lf %lf %lf[^\n]", lat, lon, time) != EOF)
    {
        track_add_point(track2, trackpoint_create(location_create(*lat, *lon), *time));
    }

    if (strcmp(argv[0], "-combine") == 0)
    {
        track_merge(track1, track2);
        track_for_each(track1, print_point, NULL);
        return 1;
    }
    else if (strcmp(argv[0], "-closest") == 0)
    {
        printf("%lf", track_closest_approach(track1, track2));
        return 1;
    }
    else
    {
        fprintf(stderr, "GPS: invalid arugment\n");
        return 0;
    }

    return 1;
}