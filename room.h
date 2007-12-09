/**********************************************************************
 * room.h - room object declaration
 *
 * Copyright 1993, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

#ifndef __ROOM_H__
#define __ROOM_H__

#include <X11/Intrinsic.h>

#include "item.h"
#include "items.h"
#include "file_sel.h"
#include "intf.h"

typedef struct {
    Widget      canvas;
    Pixel       background;

    int         gravity;

    items_type  items;       /* List of items in the rooom */
    item_type   item;        /* Item currently being created by user */
    file_sel_type file_sel;  /* Used to query for a bitmap file */

    /* Input state */
    int         oldx,oldy;
    int         newx,newy;
    Bool        button1,button2,button3;

    /* Locations of room structures */
    intf        right_wall; 
    intf        left_wall;
    intf        ceiling;
    intf        floor;

    /* Used to wait for window resize when caused by the program */
    Boolean resized;
} room_struct_type, *room_type;


#ifndef GRAVITY
#define GRAVITY      8          /* Default gravity */
#endif

room_type room__create(/* form_w */);
void      room__destroy(/* room */);
void      room__set_gravity_cb(/*w, room, call_data */);
void      room__clear_mcb(/*w, room, call_data, extra */);
void      room__randomize_mcb(/*w, room, call_data, extra */);
void      room__load_bitmap_mcb(/*w, room, call_data, extra */);
void      room__load_bitmap(/*room, filename, pos_x, pos_y*/);
void      room__set_height(/*room, height*/);
void      room__set_width(/*room, width*/);

/* Inline functions */
/* Note that values passed and returned are of type intf */
#define room__get_item( room)       ((room)->item)
#define room__get_ceiling( room)    ((room)->ceiling)
#define room__get_left_wall( room)  ((room)->left_wall)
#define room__get_right_wall( room) ((room)->right_wall)
#define room__get_floor( room)      ((room)->floor)
#define room__flush( room)          (XFlush( XtDisplay(room->canvas)))


/* gravity is of type int */
#define room__get_gravity( room)       ((room)->gravity)
#define room__set_gravity( room, grav) ((room)->gravity = (grav))


#endif
