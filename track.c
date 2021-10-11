#include "track.h"
#include "trackpoint.h"
#include "location.h"
#include <stdlib.h>

typedef struct _track_node
{
    trackpoint *pt;
    struct _track_node *next;
    struct _track_node *prev;
} track_node;

struct _track
{
    track_node head;
    track_node tail;
    size_t size;
};

// size_t track_size(track *tr);
// double track_length(track *tr);
// void track_add_point(track *tr, track_node *to_add);
// void track_destroy(track *tr);
// location *track_get(track *tr, double time);
// void track_for_each(const track *t, void (*f)(const trackpoint *, void *), void *arg);
void track_merge(track *dest, track *src);
double track_closest_approach(const track *track1, const track *track2);


track *track_create()
{
    track *result = malloc(sizeof(*result));

    if (result != NULL)
    {
        result->head.pt = NULL;
        result->tail.pt = NULL;
        result->head.next = &result->tail;
        result->tail.prev = &result->head;
        result->head.prev = NULL;
        result->tail.next = NULL;
        result->size = 0;
    }
    return result;
}


size_t track_size(const track *tr)
{
    return tr->size;
}


double track_length(const track *tr)
{
    return trackpoint_get_time(tr->tail.pt) - trackpoint_get_time(tr->head.pt);
}


bool track_add_point(track *tr, trackpoint *new_pt)
{
    track_node *new_node = malloc(sizeof(*new_node));

    if (new_node != NULL)
    {
        new_node->next = &tr->tail;
        new_node->prev = tr->tail.prev;
        new_node->next->prev = new_node;
        new_node->prev->next = new_node;

        tr->size++;
        return true;
    }
    return false;
}


void track_destroy(track *tr)
{
    track_node *curr = tr->head.next;
    while (curr != &tr->tail)
    {
        track_node *old_next = curr->next;
        trackpoint_destroy(curr->pt);
        free(curr);
        curr = old_next;
    }

    tr->size = 0;
    free(tr);
}


location *track_get(const track *tr, double time)
{
    if (tr == NULL || time < trackpoint_get_time(tr->head.pt) || time > trackpoint_get_time(tr->tail.pt))
    {
        return NULL;
    }

    const track_node *curr = &tr->head;
    location *result;
    do
    {
        if (trackpoint_get_time(curr->pt) == time)
        {
            result = location_create(trackpoint_get_latitude(curr->pt), trackpoint_get_longitude(curr->pt));

            return result;
        }
        else if (trackpoint_get_time(curr->pt) > time)
        {
            // (time - t1) / (t2 - t1)
            double time_frac = (time - trackpoint_get_time(curr->prev->pt)) / (trackpoint_get_time(curr->pt) - trackpoint_get_time(curr->prev->pt));
            // time_frac * (l2 - l1) + l1
            double result_lat = time_frac * (trackpoint_get_latitude(curr->pt) - trackpoint_get_latitude(curr->prev->pt)) + trackpoint_get_latitude(curr->prev->pt);
            double result_lon = time_frac * (trackpoint_get_longitude(curr->pt) - trackpoint_get_longitude(curr->prev->pt)) + trackpoint_get_longitude(curr->prev->pt);

            result = location_create(result_lat, result_lon);

            return result;
        }
    } while (curr != &tr->tail);

    return NULL;
}


void track_for_each(const track *tr, void (*f)(const trackpoint *, void *), void *arg)
{
    track_node *curr = tr->head.next;
    while (curr != &tr->tail)
    {
        f(curr->pt, arg);
        curr = curr->next;
    }
}


void track_merge(track *dest, track *src)
{
    return 0;
}


double track_closest_approach(const track *track1, const track *track2)
{
    return 0;
}