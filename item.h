/**********************************************************************
 * item.h - item (ball) declaration
 *
 * Copyright 1993, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

#ifndef __ITEM_H__
#define __ITEM_H__

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

#include "misc.h"
#include "intf.h"

#define MAX_COLORS 256

#ifndef ELASTICITY
#define ELASTICITY  90  /* Default */
#endif


/* Static data shared for all items */
typedef struct {
    int    width;
    int    height;
    int    half_width;  /* Precalculated for copying pixmap offset */
    int    half_height;
    double elasticity;
    Bool   ball_collide;
    Bool   perpetual;
    Bool   visible;
    char * rgb_txt;

    Display * display;
    Window    window;
    Pixel     background;
    GC        gc;
    Pixmap    pixmaps[ MAX_COLORS];/* Holds all item drawings */
    int       num_colors;          /* Holds number of colors/pixmaps used */
    int       curr_pixmap;         /* Holds pixmap index to use for next item*/
} item_static_struct_type, *item_static_type;


typedef struct {
    item_static_type static_data;

    Display *display;
    Window   window;
    intf     x,y;              /* Location of items center */
    intf     y_vel;            /* vel < 0 means dropping, > 0 means climbing */
    intf     x_vel;            /* vel < 0 means to left, > 0 means to right */
    int      rebounded;        /* Used to determine if item collision */
                               /* had already been calculated for this item */
    Bool     shown;            /* True if items image is visible */
    Pixmap   pixmap;           /* Pixel map to use for drawing item */
} item_struct_type, *item_type;


item_type item__create(/* display, window, x, y, x_vel, y_vel*/);
void      item__init(/*display, window, background*/);
Bool      item__initted(/**/);
void      item__destroy(/* item*/);
void      item__draw(/* item*/);
void      item__undraw(/* item, x, y*/);
void      item__redraw(/* item*/);
void      item__erase(/* item*/);
void      item__redraw(/* item*/);
void      item__move_pos(/* item, x, y*/);
void      item__randomize(/* item, min_x_vel, max_x_vel, 
			     min_y_vel, max_y_vel*/);
void      item__move(/* item, room, items*/);
void      item__rebound_item(/* moved_item, fixed_item*/);
void      item__set_elasticity_cb(/*w, datap_ptr, call_data*/);
int       item__get_width(/**/);
int       item__get_height(/**/);
void      item__perpetual_mcb(/*w, item, call_data, extra*/);
void      item__collide_mcb(/*w, item, call_data, extra*/);
void      item__set_perpetual(/* set */);
void      item__set_ball_collide(/* set*/);
void      item__set_visible(/*visible*/);;


/* Inline functions */
#define item__set_x_vel( item, vel) ((item)->x_vel = int2intf(vel))
#define item__set_y_vel( item, vel) ((item)->y_vel = int2intf(vel))
#define item__get_x_vel( item) (intf2int((item)->x_vel))
#define item__get_y_vel( item) (intf2int((item)->y_vel))
#define item__get_x( item) (intf2int((item)->x))
#define item__get_y( item) (intf2int((item)->y))
#define item__erase( item) (item__undraw( (item), (item)->x, (item)->y))
#define item__stopped(item, room) \
     (item->y_vel == 0)  && \
     (ABS(item->y) >= room__get_floor(room) - room__get_gravity(room)) && \
     (item->x_vel == 0)


#endif
