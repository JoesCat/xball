/**********************************************************************
 * items.c - definition of item list object.
 *
 * Copyright 1993, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

/* System Headers */
#include <string.h>

/* Local headers */
#include "items.h"
#include "room.h"

#if 0
/* External variables */
extern item_static_type static_data;
#endif


/* Create the items object */
items_type items__create()
{
    items_type items = (items_type)malloc( sizeof( items_struct_type));

    items->list = list__create();

    return items;
}


/* Destroy the items object */
items_type items__destroy(items)
items_type items;
{
    items__destroy_all( items);

    list__destroy( items->list);

    free( items);
}


/* Return the first item in the list */
item_type items__get_first( items)
items_type items;
{
    return (item_type)list__get_first(items->list);
}


/* Return the next item in the list */
item_type items__get_next( items)
items_type items;
{
    return (item_type)list__get_next(items->list);
}


/* Add an item to the list of items */
void items__add( items, item)
items_type      items;
item_type       item;
{
    list__add_end( items->list, (void *)item);
}


/* Destroy all items in the item list */
void items__destroy_all( items)
items_type items;
{
    item_type item;


    for (item = (item_type)list__remove_last( items->list);
         item != (item_type)0;
         item = (item_type)list__remove_last( items->list))
    {
        item__destroy( item);
    }
}


/* Create a new item and add it to the item list */
void items__new_item( items, x, y, x_vel, y_vel)
items_type items;
int      x,y;
int      x_vel,y_vel;
{
    list__add_end( items->list, (void *)item__create(x, y, x_vel, y_vel));
}


/* Perform the iterative simulation movement for all items on the list */
/* If an item dies, destroy it */
void items__move_items( items, room)
items_type items;
room_type room;
{
    item_type item;


    /* Move each item on the screen */
    for (items->current = 0;
	 items->current < list__get_count( items->list); 
         items->current++)
    {
        item = (item_type)list__get_cell(items->list, items->current);

        item__move( item, room, items);

        if (item__stopped( item, room))
        {
            list__remove( items->list, items->current);
            items->current--;
            item__destroy( item);
        }
    }
}



/* Simulate moving the items backwards count iterations */
/* The iterations are not shown in the screen */
/* Note that gravity and elastivity are not handled */
void items__move_items_backwards( items, room, count)
items_type items;
room_type room;
int count;
{
    item_type item;


    /* Reverse all items' velocities and erase them from the screen */
    for (items->current = 0;
         items->current < list__get_count( items->list); 
         items->current++)
    {
        item = (item_type)list__get_cell(items->list, items->current);

        item->x_vel = -item->x_vel;
        item->y_vel = -item->y_vel;

        item__erase( item);
    }

    /* Move them all count iterations (they will go backwards) */
    while (count-- > 0)
    {
        /* Move each item on the screen */
        for (items->current = 0;
             items->current < list__get_count( items->list); 
             items->current++)
        {
            item = (item_type)list__get_cell(items->list, items->current);

            item__move( item, room, items);
        }
    }


    /* Reverse all the items' velocities again and draw them */
    for (items->current = 0;
         items->current < list__get_count( items->list); 
         items->current++)
    {
        item = (item_type)list__get_cell(items->list, items->current);

        item->x_vel = -item->x_vel;
        item->y_vel = -item->y_vel;

        item__draw( item);
    }

}


/* Make all items visible or invisible */
void items__set_visible( items, visible)
items_type items;
Bool       visible;
{
    if (visible)
        item__set_visible( True);

    for (items->current = 0;
         items->current < list__get_count( items->list); 
         items->current++)
    {
        if (visible)
            item__draw( (item_type)list__get_cell(items->list,items->current));
        else
            item__erase((item_type)list__get_cell(items->list,items->current));
    }

    if (!visible)
        item__set_visible( False);
}


/* Rebound the given item again all items in the item list */
void items__rebound_items( item, items)
item_type item;
items_type items;
{
    int i;

    for (i = items->current+1; i < list__get_count( items->list); i++)
    {
        item__rebound_item(item, 
                           (item_type)list__get_cell(items->list, i));
    }
}


#ifdef DEBUG

void items__print( items)
items_type items;
{
    int i;

    for (i = 0; i < list__get_count( items->list); i++)
    {
        printf("%4d: ", i);
        item__print( (item_type)list__get_cell(items->list, i));
    }
}

#endif
