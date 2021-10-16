#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "track.h"
#include "trackpoint.h"
#include "location.h"


/**
 * Prints location of single trackpoint to stdout.
 *
 * @param pt pointer to a valid trackpoint, non-NULL
 */
void print_point(const trackpoint *pt, void *info)
{
    printf("%.6f %.6f %.6f\n",
	    trackpoint_get_latitude(pt),
	    trackpoint_get_longitude(pt),
	    trackpoint_get_time(pt));
}


int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, "GPS: missing filename(s)\n");
        return 0;
    }

    FILE *input1 = fopen(argv[2], "r");
    FILE *input2 = fopen(argv[3], "r");

    if (input1 == NULL || input2 == NULL)
    {
        fprintf(stderr, "GPS: could not open file(s)\n");
        return 0;
    }

    double *lat = malloc(sizeof(*lat));
    double *lon = malloc(sizeof(*lon));
    double *time = malloc(sizeof(*time));

    track *track1 = track_create();
    int scan = fscanf(input1, "%lf %lf %lf\n", lat, lon, time);
    while (scan != EOF)
    {
        if (scan < 3)
        {
            fscanf(input1, "%*[^\n]\n");
            scan = fscanf(input1, "%lf %lf %lf\n", lat, lon, time);
            continue;
        }
        location *loc = location_create(*lat, *lon);
        if (loc != NULL)
        {
            track_add_point(track1, trackpoint_create(loc, *time));
        }
        scan = fscanf(input1, "%lf %lf %lf\n", lat, lon, time);
    }

    track *track2 = track_create();
    scan = fscanf(input2, "%lf %lf %lf\n", lat, lon, time);
    while (scan != EOF)
    {
        if (scan < 3)
        {
            fscanf(input2, "%*[^\n]\n");
            scan = fscanf(input1, "%lf %lf %lf\n", lat, lon, time);
            continue;
        }
        location *loc = location_create(*lat, *lon);
        if (loc != NULL)
        {
            track_add_point(track2, trackpoint_create(loc, *time));
        }
        scan = fscanf(input2, "%lf %lf %lf\n", lat, lon, time);
    }

    free(lat);
    free(lon);
    free(time);
    fclose(input1);
    fclose(input2);

    if (strcmp(argv[1], "-combine") == 0)
    {
        track_merge(track1, track2);
        track_for_each(track1, print_point, NULL);
        track_destroy(track1);
    }
    else if (strcmp(argv[1], "-closest") == 0)
    {
        printf("%0.lf\n", round(track_closest_approach(track1, track2)));
        track_destroy(track1);
        track_destroy(track2);
    }
    else
    {
        fprintf(stderr, "GPS: invalid arugment\n");
        return 0;
    }

    return 1;
}