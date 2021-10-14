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
    while (fscanf(input1, "%lf %lf %lf\n", lat, lon, time) != EOF)
    {
        location *loc = location_create(*lat, *lon);
        if (loc != NULL)
        {
            track_add_point(track1, trackpoint_create(loc, *time));
        }
    }

    track *track2 = track_create();
    while (fscanf(input2, "%lf %lf %lf\n", lat, lon, time) != EOF)
    {
        location *loc = location_create(*lat, *lon);
        if (loc != NULL)
        {
            track_add_point(track2, trackpoint_create(loc, *time));
        }
    }

    free(lat);
    free(lon);
    free(time);

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