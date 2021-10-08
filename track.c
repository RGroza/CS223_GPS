#include "track.h"

typedef struct track
{
    trackpoint **pts;
    _track_segment *segs;
    size_t size;
} track;


track *track_create()
{
    track *tr = malloc(sizeof(*tr));
    if (tr != NULL)
    {
        tr->pts = malloc(sizeof(*tr->pts) * size);
        tr->segs = malloc(sizeof(*tr->segs) * size);
        tr->size = size;
    }
    return tr;
}


void track_destroy(track *l)
{
    if (l != NULL)
    {
        for (int i = 0; i < l.size; i++)
        {
            trackpoint_destroy(l->pts[i]);
            track_segment_destroy(l->segs[i]);
        }
        free(l->pts);
        free(l->segs);
        free(l);
    }
}