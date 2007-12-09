/**********************************************************************
 * list.c - defines a void * list object
 *
 * Copyright 1993, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

/* System Headers */
#include <malloc.h>

/* Local headers */
#include "list.h"

/* Functions */
static void list_double(/* list*/);
static void *get_item(/* list, index*/);



/* Public object methods */

/* Create the list object */
list_type list__create()
{
    list_type list;


    list = (list_type)malloc( sizeof( list_struct));

    list->count     = 0;
    list->curr_item = 0;
    list->max_count = LIST_INITIAL_SIZE;
    list->data = (void **)malloc( sizeof( void *) * list->max_count);

    return list;
}


/* Destroy the list object.  Items in the list are not freed */
void list__destroy( list)
list_type list;
{
    free( list->data);
    free( list);
}


/* Add an item to the start of the list */
void *list__add_start( list, data_item)
list_type list;
void *data_item;
{
    int i;


    if (list->count == list->max_count)
        list_double( list);

    /* Copy all item down by one */
    for (i = list->count-1; i > 0; i--)
        list->data[ i] = list->data[ i - 1];

    list->data[ 0] = data_item;

    list->count++;

    return data_item;
}


/* Add an item to the end of the list */
void *list__add_end( list, data_item)
list_type list;
void *data_item;
{
    if (list->count == list->max_count)
        list_double( list);

    list->data[ list->count] = data_item;

    list->count++;

    return data_item;
}


/* Get the first item in the list */
void *list__get_first( list)
list_type list;
{
    list->curr_item = 0;
    return get_item( list, list->curr_item);
}


/* Get the next item in the list.  NULL is returned if there are no more
   items in the list */
void *list__get_next( list)
list_type list;
{
    list->curr_item++;
    return get_item( list, list->curr_item);
}


/* Return the last item in the list */
void *list__get_last( list)
list_type list;
{
    list->curr_item = list->count - 1;
    return get_item( list, list->curr_item);
}


/* Return the previous item in the list.  If there are no more items,
   return NULL. */
void *list__get_prev( list)
list_type list;
{
    if (list->curr_item > 0)
    {
        list->curr_item--;
        return get_item( list, list->curr_item);
    }
    else
        return (void *)0;
}


/* Remove the last item in the list.  The item removed is returned. */
void *list__remove_last( list)
list_type list;
{
    void *data_item = (void *)0;


    if (list->count > 0)
    {
        list->count--;
        data_item = list->data[ list->count];
        list->data[ list->count] = (void *)0;
    }

    return data_item;
}


/* Remove the first item in the list.  The item removed is returned. */
void *list__remove_first( list)
list_type list;
{
    int i;
    void *data_item = (void *)0;


    if (list->count > 0)
    {
        list->count--;
        data_item = list->data[ 0];

        /* Shift data pointers up one */
        for (i = 0; i < list->count; i++)
            list->data[ i] = list->data[ i+1];

        list->data[ list->count] = (void *)0;
    }

    return data_item;
}


/* Remove the current item in the list, as positioned by get_first, next, 
   last, prev.   The current item is now the item after the one removed */
void *list__remove_curr( list)
list_type list;
{
    int i;
    void *data_item = (void *)0;

    if (list->curr_item < list->count)
    {
        data_item = list->data[ list->curr_item];

        for (i = list->curr_item; i < list->count; i++)
            list->data[ i] = list->data[ i+1];

        list->data[ list->count] = (void *)0;
    }

    return data_item;
}


/* Remove an item at the specified position in the list */
void *list__remove( list, ind)
list_type list;
int       ind;
{
    int i;
    void *data_item = (void *)0;

    if (ind >= 0 && ind < list->count)
    {
        data_item = list->data[ ind];

        for (i = ind; i < list->count - 1; i++)
            list->data[ i] = list->data[ i+1];

        list->data[ list->count] = (void *)0;
    }

    list->count--;

    return data_item;
}


/* Remove all items in the list.  The objects are not freed */
void list__remove_all( list)
list_type list;
{
    int i;


    for (i = 0; i < list->count; i++)
        list->data[ i] = (void *)0;

    list->count = 0;
}


/* Private object methods */

/* Return the specified object at a specified position */
static void *get_item( list, ind)
list_type list;
int       ind;
{
    if (ind < list->count)
        return list->data[ ind];
    else
        return (void *)0;
}


/* Double the size of the list */
static void list_double( list)
list_type list;
{
    int i;


    /* Double the list's size */
    void **tmp_data = list->data;

    list->max_count *= 2;
    list->data = (void **)malloc( sizeof( void *) * list->max_count);

    for (i = 0; i < list->count; i++)
        list->data[ i] = tmp_data[ i];

    free( tmp_data);
}
