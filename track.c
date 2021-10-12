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
    if (tr->size < 2)
    {
        return 0;
    }
    return trackpoint_get_time(tr->tail.prev->pt) - trackpoint_get_time(tr->head.next->pt);
}


bool track_add_point(track *tr, trackpoint *new_pt)
{
    if (tr == NULL && new_pt == NULL && trackpoint_get_time(new_pt) > trackpoint_get_time(tr->tail.prev->pt))
    {
        return false;
    }

    track_node *new_node = malloc(sizeof(*new_node));

    if (new_node != NULL)
    {
        new_node->pt = new_pt;
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
    // printf("%zu\n", tr->size);

    if (tr == NULL || time < trackpoint_get_time(tr->head.next->pt) || time > trackpoint_get_time(tr->tail.prev->pt))
    {
        return NULL;
    }

    const track_node *curr = tr->head.next;
    for (size_t i = 0; i < tr->size; i++)
    {
        // printf("%lf %lf %lf\n", trackpoint_get_latitude(curr->pt), trackpoint_get_longitude(curr->pt), trackpoint_get_time(curr->pt));
        if (trackpoint_get_time(curr->pt) == time)
        {
            return location_create(trackpoint_get_latitude(curr->pt), trackpoint_get_longitude(curr->pt));
        }
        else if (trackpoint_get_time(curr->pt) > time)
        {
            double time_frac = (time - trackpoint_get_time(curr->prev->pt)) / (trackpoint_get_time(curr->pt) - trackpoint_get_time(curr->prev->pt));

            return location_interpolate(trackpoint_get_location(curr->prev->pt), trackpoint_get_location(curr->pt), time_frac);
        }
        curr = curr->next;
    }

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


void track_add_node_end(track *tr, track_node *to_add)
{
    to_add->next = &tr->tail;
    to_add->prev = tr->tail.prev;
    to_add->next->prev = to_add;
    to_add->prev->next = to_add;
    
    tr->size++;
}


void track_remove_node(track *tr, track_node *to_remove)
{
    track_node *before = to_remove->prev;
    track_node *after = to_remove->next;

    to_remove->next = NULL;
    to_remove->prev = NULL;
    before->next = after;
    after->prev = before;

    tr->size--;
}


void track_split(track *src, track *dest1, track *dest2)
{
    while (src->size > 0)
    {
        track_node *curr = src->head.next;
        track_remove_node(src, curr);
        if (dest1->size > dest2->size)
        {
            track_add_node_end(dest2, curr);
        }
        else
        {
            track_add_node_end(dest1, curr);
        }
    }
}


void track_merge(track *dest, track *src)
{
    track_node *curr_dest = dest->head.next;
    track_node *curr_src = src->head.next;
    while (src->size > 0)
    {
        if (&curr_src == &src->tail.next || &curr_dest == &dest->tail.next)
        {
            break;
        }

        track_node *next_dest = curr_dest->next;
        track_node *next_src = curr_src->next;
        if (trackpoint_get_time(curr_dest->pt) < trackpoint_get_time(curr_src->pt))
        {
            printf("<\n");
            if (curr_dest->next == &dest->tail)
            {
                curr_dest->next = curr_src;
                src->tail = dest->tail;

                dest->size += src->size;
                break;
            }
            curr_dest = next_dest;
        }
        else if (trackpoint_get_time(curr_dest->pt) > trackpoint_get_time(curr_src->pt))
        {
            printf(">\n");
            curr_src->next = curr_dest;
            curr_src->prev = curr_dest->prev;
            curr_dest->prev->next = curr_src;
            curr_dest->prev = curr_src;

            curr_src = next_src;
            dest->size++;
            src->size--;
        }
        else
        {
            printf("=\n");
            track_remove_node(src, curr_src);
            trackpoint_destroy(curr_src->pt);
            if (location_compare(trackpoint_get_location(curr_dest->pt), trackpoint_get_location(curr_src->pt)) == 0)
            {
                track_remove_node(dest, curr_dest);
                trackpoint_destroy(curr_dest->pt);
            }
        }
    }
}


double track_closest_approach(const track *track1, const track *track2)
{
    return 0;
}