/**********************************************************************
 * file - description
 *
 * Copyright 1993, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

#ifndef __ITEMS_H__
#define __ITEMS_H__

#include "list.h"
#include "item.h"

typedef struct {
    list_type list;
    int current;  /* Used to increase efficiency */
} items_struct_type, *items_type;

items_type items__create(/**/);
items_type items__destroy(/*items*/);
item_type  items__get_first(/* items */);
item_type  items__get_next(/* items */);
void       items__add(/* items, item */);
void       items__destroy_all(/* items*/);
void       items__new_item(/* items, x, y, x_vel, y_vel */);
void       items__move_items(/* items, room*/);
void       items__move_items_backwards(/* items, room, count*/);
void       items__rebound_items(/* items, item*/);
void       items__set_visible( /*items, visible*/);

/* Inline functions */
#define items__get_count( items) (list__get_count((items)->list))

#endif
