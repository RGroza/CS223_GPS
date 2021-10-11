#include "track.h"
#include "trackpoint.h"
#include "location.h"
#include <stdlib.h>

typedef struct _track
{
    trackpoint **pts;
    track_segment **segs;
    size_t size;
    size_t capacity;
} track;

#define INITIAL_CAPACITY 2


track *track_create()
{
    track *tr = malloc(sizeof(*tr));
    if (tr != NULL)
    {
        tr->pts = malloc(sizeof(*tr->pts) * INITIAL_CAPACITY);
        tr->segs = malloc(sizeof(*tr->segs) * INITIAL_CAPACITY);
        tr->size = 0;
        tr->capacity = (tr->pts != NULL && tr->segs != NULL) ? INITIAL_CAPACITY : 0;
    }
    return tr;
}


void track_expand(track *tr)
{
    if (tr->capacity > 0 && tr->size == tr->capacity)
    {
        trackpoint **expanded_pts = realloc(tr->pts, sizeof(*tr->pts) * tr->capacity * 2);
        track_segment **expanded_segs = realloc(tr->segs, sizeof(*tr->segs) * tr->capacity * 2);
        if (expanded_pts != NULL && expanded_pts != NULL)
	    {
            tr->pts = expanded_pts;
            tr->segs = expanded_segs;
            tr->capacity *= 2;
    	}
    }
}


void track_destroy(track *tr)
{
    if (tr != NULL)
    {
        for (int i = 0; i < tr->size; i++)
        {
            trackpoint_destroy(tr->pts[i]);
            track_segment_destroy(tr->segs[i]);
        }

        free(tr->pts);
        free(tr->segs);
        free(tr);
    }
}