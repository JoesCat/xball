/**********************************************************************
 * item.c - defines the item object
 *
 * Copyright 1993, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

/* System Headers */
#include <stdlib.h>
#include <X11/StringDefs.h>

#ifdef MOTIF
#include <Xm/ToggleB.h>
#endif

/* Local headers */
#include "rgbtxt_loc.h" /* RGB_TXT=rgb.txt file location(s) */
#include "item.h"
#include "room.h"
#include "scrollbar.h"
#include "color_l.h"
#include "menu.h"
#include "misc.h"
#include "stipple.xbm"

/* Macros */
#define SHADES      4   /* Number of shades for colors in the rgb.txt file */
#define ITEM_WIDTH  10  /* The item's default size */
#define ITEM_HEIGHT 10

/* put_pixmap - Draws the item's pixmap to the specified location */
#define put_pixmap( item, x, y) \
    XCopyArea(item->display, item->pixmap, item->window, \
              item->static_data->gc, \
              0, 0, \
              item->static_data->width, \
              item->static_data->height, \
              intf2int(x) - item->static_data->half_width, \
              intf2int(y) - item->static_data->half_height)

/* File scope variables */
static item_static_struct_type static_struct;  /* Item object static data */
item_static_type static_data = &static_struct;
static Bool static_data_initted = False;

/* Resources */
typedef struct {
    int    item_width;
    int    item_height;
    char * rgb_txt;
    char * rgb_txt2;
    int    elasticity;
} app_data,*app_data_ptr;
static app_data res_data;
static XtResource resources[] = {
    { "itemWidth", "ItemWidth", XtRInt, sizeof(int),
        XtOffset(app_data_ptr,item_width), XtRImmediate, (caddr_t)ITEM_WIDTH },
    { "itemHeight", "ItemHeight", XtRInt, sizeof(int),
        XtOffset(app_data_ptr,item_height),XtRImmediate, (caddr_t)ITEM_HEIGHT},
    { "rgbTxt", "RgbTxt", XtRString, sizeof(char *),
        XtOffset(app_data_ptr,rgb_txt), XtRString, RGB1_TXT},
    { "rgbTxt2", "RgbTxt2", XtRString, sizeof(char *),
        XtOffset(app_data_ptr,rgb_txt2), XtRString, RGB2_TXT}
};

/* Functions */
static item_static_type static_data__create(/*display, window, background*/);
static void             draw_item(/* static_data, display, clear_gc, colors,
                                     pixmap, background*/);
static void             draw_circle(/* display, pixmap,
                                       x_offset, y_offset,
                                       width, height*/);
static void             draw_black_ball(/* display, background*/);
static void             draw_ball(/* display, pixmap, x_offset, y_offset,
                                     width, height*/);


/* Public object methods */

/* Create the item object.  The item is the thing that bounces on the screen */
/* The item is drawn to the screen after it is created */
item_type item__create( x, y, x_vel, y_vel)
int x;
int y;
int x_vel;
int y_vel;
{
    item_type item;


    item = (item_type)malloc( sizeof( item_struct_type));

    item->static_data = static_data;

    item->display = static_data->display;
    item->window  = static_data->window;

    /* Draw with the next available pixmap image (color) */
    static_data->curr_pixmap =
        (static_data->curr_pixmap + 1) % static_data->num_colors;

    item->pixmap = static_data->pixmaps[ static_data->curr_pixmap];

    /* Position are stored as intf (int-floats) to get more precision
       out of an int */
    item->x     = int2intf( x);
    item->y     = int2intf( y);
    item->x_vel = int2intf( x_vel);
    item->y_vel = int2intf( y_vel);
    item->shown = False;

    item->rebounded = False;

    item__draw( item);

#ifdef DEBUG
    /* To generate demos */
    printf("create x=%d, y=%d, x_vel=%d, y_vel=%d\n",
           intf2int(item->x),intf2int(item->y),
           intf2int(item->x_vel),intf2int(item->y_vel));
#endif

    return item;
}


/* Initializes the item's static data.  This can only be done once
   there is a window since we have to get the background color.
   Thus, we are called from room's first refresh callback */
void item__init(Widget toplevel, Display * display, Window window, Pixel background) {
    static_data = static_data__create(toplevel, display, window, background);
    if (!static_data)
        static_data_initted = True;
}

/* Returns true of the item's static data is initialized */
Bool item__initted() {
    return static_data_initted;
}


/* Destroys an item */
void item__destroy( item)
item_type item;
{
    item__undraw( item, item->x, item->y); /* Erase item from screen */

    free( (char *)item);
}


/* Draws the item to the screen if it is visible.  Handles the case
   where the item was already visible on the screen. */
void item__draw( item)
item_type item;
{
    if (!item->shown && item->static_data->visible) {
        put_pixmap( item, item->x, item->y);
        item->shown = True;
    }
}


/* Erases the item from the screen */

void item__undraw( item, x, y)
item_type item;
intf      x,y;
{
    if (item->shown && item->static_data->visible) {
        put_pixmap( item, x, y);
        item->shown = False;
    }
}


/* Redraws the item on the screen, regardless of whether it is shown.
   This method should be used when the window has to be refreshed.
   invisible items will not be drawn. */
void item__redraw( item)
item_type item;
{
    if (item->static_data->visible) {
        put_pixmap( item, item->x, item->y);
        item->shown = True;
    }
}


/* Move an item's image to a new location */
void item__move_pos( item, x, y)
item_type item;
int x,y;
{
    item__undraw( item, item->x, item->y);

    item->x = int2intf(x);
    item->y = int2intf(y);

    item__draw( item);
}


/* Give an item a random velocity, based on the passed ranges */
void item__randomize( item, min_x_vel, max_x_vel, min_y_vel, max_y_vel)
item_type item;
int       min_x_vel;
int       max_x_vel;
int       min_y_vel;
int       max_y_vel;
{
    item->x_vel = int2intf(rand_range( min_x_vel, max_x_vel));
    item->y_vel = int2intf(rand_range( min_y_vel, max_y_vel));
}


/* Perform one item movement iteration, taking into account other
   balls, gravity, elasticity, and the room boundaries.  The item's new
   position is calculated and the item's image is moved to the new
   location.  Invisible items are not redrawn. */
void item__move( item, room, items)
item_type  item;
room_type  room;
items_type items;
{
    /* Calculate new position of item */

    intf oldx = item->x;
    intf oldy = item->y;

    /* Calculate new y position */
    /* Gravity adds to velocity */
    if ((item->y < room__get_floor(room)) || (item->y_vel != 0)) {
        item->y_vel += room__get_gravity( room);
    }


    /* Move vertically based on velocity */
    item->y += item->y_vel;

    if (item->y >= room__get_floor(room)) {
        /* item hit floor -- bounce off floor */

        /* Reverse ball velocity */
        item->y_vel = -item->y_vel;

        item->y += item->y_vel;

        if (ABS(item->y_vel) < room__get_gravity(room))
            /* This helps dampen rounding errors that cause balls to
               bounce forever */
            item->y_vel = 0;
        else {
            /* Ball velocity is reduced by the percentage elasticity */
            item->y_vel *= item->static_data->elasticity;
#if 1
            item->y = room__get_floor( room) -
                (room__get_floor( room) - item->y) *
                    item->static_data->elasticity;
#else
            item->y = -item->y + room__get_floor( room)*2;
#endif
        }
    } else if (item->y < room__get_ceiling(room)) {
        /* Reverse ball velocity */
        item->y_vel = -item->y_vel;

        /* Ball velocity is reduced by the percentage elasticity */
        item->y_vel *= item->static_data->elasticity;

        /* Bounce off the wall */
        item->y = -item->y + room__get_ceiling( room)*2;
    }


    /* Calculate new x position */
    /* Move horizontally based on velocity */
    item->x += item->x_vel;

    if (item->x > room__get_right_wall(room)) {
        /* Hit right wall */
        /* Reverse ball velocity */
        item->x_vel = -item->x_vel;

        /* Ball velocity is reduced by the percentage elasticity */
        item->x_vel *= item->static_data->elasticity;

        /* Bounce off the wall */
        item->x = -item->x + room__get_right_wall( room)*2;
    } else if (item->x < room__get_left_wall(room)) {
        /* Hit left wall */
        /* Reverse ball velocity */
        item->x_vel = -item->x_vel;

        /* Ball velocity is reduced by the percentage elasticity */
        item->x_vel *= item->static_data->elasticity;

        /* Bounce off the wall */
        item->x = -item->x + room__get_left_wall( room)*2;
    }


    /* Slow ball if it is rolling on the floor */

    if (ABS(item->y) >= room__get_floor(room) - room__get_gravity(room) &&
        item->y_vel == 0)
        item->x_vel *= item->static_data->elasticity;


    /* Collide with other balls or ball being created */
    if (item->static_data->ball_collide) {
        /* See if collided with the ball under the pointer being created */
        item_type room_item = room__get_item( room);
        if (room_item != (item_type)0) {
            item__rebound_item(item, room_item);
        }

        /* See if collided with another item */
        items__rebound_items( item, items);
    }

    /* See if item has come to a peaceful rest */
    if (item__stopped(item, room)) {
        /* on floor && Not bouncing */
        if (item->static_data->perpetual) {
            /* Just drop the ball from the ceiling */
            item->y = room__get_ceiling( room);
        } else {
            if (item__stopped(item, room)) {
                item__undraw( item, oldx, oldy);
                return;       /* Don't draw item */
            }

            /* Slow down velocity once rolling based on elasticity */
            if (item->x_vel > 0)
                item->x_vel *= item->static_data->elasticity;
        }
    }

    /* If item moved, redraw it in the new position */
    if ((item->y != oldy || item->x != oldx) && static_data->visible) {
        /* Erase old object */
        item__undraw( item, oldx, oldy);
        item__draw( item);
    }
}



/* See if items have hit and rebound them if they have.
   Moved_item is assumed to have just been moved.
   Only the moved item will have its x,y positions potentially changed. */
void item__rebound_item( moved_item, fixed_item)
item_type moved_item;
item_type fixed_item;
{
    intf xdiff,ydiff;

    xdiff = moved_item->x - fixed_item->x;
    ydiff = moved_item->y - fixed_item->y;

    if (ABS(xdiff) < int2intf(moved_item->static_data->width) &&
        ABS(ydiff) < int2intf(moved_item->static_data->height))
    {
        SWAP( moved_item->x_vel, fixed_item->x_vel, intf);
        SWAP( moved_item->y_vel, fixed_item->y_vel, intf);

        if (moved_item->y_vel <= int2intf(moved_item->static_data->height) &&
            fixed_item->y_vel <= int2intf(moved_item->static_data->height))
        {
            moved_item->y += moved_item->y_vel;
            moved_item->y_vel += ydiff * moved_item->static_data->elasticity/10;
            fixed_item->y_vel -= ydiff * moved_item->static_data->elasticity/10;
        }
        if (moved_item->x_vel <= int2intf(moved_item->static_data->width) &&
            fixed_item->x_vel <= int2intf(moved_item->static_data->width))
        {
            moved_item->x += moved_item->x_vel;
            moved_item->x_vel += xdiff * moved_item->static_data->elasticity/10;
            fixed_item->x_vel -= xdiff * moved_item->static_data->elasticity/10;
        }
    }
}


/* A scrollbar callback method that sets all the balls' elasticity */
void item__set_elasticity_cb(w, closure, call_data)
Widget                    w;
caddr_t                   closure;
scrollbarCallbackStruct * call_data;
{
    static_data->elasticity = call_data->value / 100.0;
}


/* Return all items' width */
int item__get_width()
{
    return static_data->width;
}


/* Return all items' height */
int item__get_height()
{
    return static_data->height;
}


/* A menu callback (mcb) that sets all items' perpetual status */
void item__perpetual_mcb(w, closure, call_data, extra)
Widget               w;
caddr_t              closure;
menuCallbackStruct * call_data;
char *               extra;
{
    static_data->perpetual = call_data->set;
}


/* A menu callback (mcb) that sets all items' collide status */
void item__collide_mcb(w, closure, call_data, extra)
Widget w;
caddr_t closure;
menuCallbackStruct * call_data;
char *extra;
{
    static_data->ball_collide = call_data->set;
}


/* Semi-private object methods */

/* Sets all items' perpetual status */
/* Note that the toggle must be set properly also */
void item__set_perpetual(set)
Bool set;
{
    static_data->perpetual = set;
}


/* Sets all items' collide status */
/* Note that the toggle must be set properly also */
void item__set_ball_collide(set)
Bool set;
{
    static_data->ball_collide = set;
}


/* Set all item's visible status */
/* Note that all items must be undrawn from the screen */
void item__set_visible(visible)
Bool visible;
{
    static_data->visible = visible;
}


/* Private object methods */

/* Load items resources */
static void get_resources(
       Widget toplevel, Display *display, Window window, Pixel background) {
    XtGetApplicationResources(toplevel, (XtPointer)&res_data,
                              resources, XtNumber(resources),
                              (ArgList)NULL,(Cardinal)0);

    static_data->width       = res_data.item_width;
    static_data->height      = res_data.item_height;
    static_data->half_width  = static_data->width  / 2;
    static_data->half_height = static_data->height / 2;
    static_data->rgb_txt     = res_data.rgb_txt;
    static_data->rgb_txt2    = res_data.rgb_txt2;
}


/* Initialize the item's static data object */
static item_static_type static_data__create(
       Widget toplevel, Display * display, Window window, Pixel background) {
    GC clear_gc;

    /* The following already set in xball_sys object:
       static_data->elasticity
       static_data->ball_collide
       static_data->perpetual */

    get_resources(toplevel, display, window, background);

    static_data->visible    = True;
    static_data->background = background;


    static_data->display = display;
    static_data->window = window;
    static_data->gc = XCreateGC(display, window, 0, 0);
    clear_gc = XCreateGC(display, window, 0, 0);
    static_data->curr_pixmap = 0;


    if (DisplayCells(display, DefaultScreen(display)) > 2) {
        /* Non-monochrome system */
        color_list_type color_list;
        color_type      color;
        Colormap        colormap;
        XColor          colors[SHADES]; /* The color shades */
        Pixel           cells[SHADES];
        int             pixmap_index;
        int             shade;

        color_list = color_list__create(static_data->rgb_txt);
        if (!color_list) {
            color_list = color_list__create(static_data->rgb_txt2);
            if (!color_list) {
                fprintf(stderr, "item::failed to get file rgbTxt1=%s\n", static_data->rgb_txt);
                fprintf(stderr, "item::failed to get file rgbTxt2=%s\n", static_data->rgb_txt2);
                return 0;
            }
        }
        colormap = DefaultColormap(display, DefaultScreen(display));

        for (shade = 0; shade < SHADES; shade++)
            colors[ shade].flags = DoRed | DoGreen | DoBlue;

        for (color = color_list__get_last( color_list), pixmap_index = 0;
             color != (color_type)0;
             pixmap_index++) {
            if (!XAllocColorCells(display, colormap,     /*contig =*/True,
                                  /*plane_masks=*/NULL,  /*nplanes=*/0,
                                  /*pixels     =*/cells, /*ncolors=*/SHADES)) {
                /* If you are here, there is not enough colors, or XAllocColorCells
                 * may be ignored by modern hardware. Most modern display cards are
                 * now 24b or 32b color capable. These are TrueColor type cards. */
		/* TODO: Create colored balls for TrueColor Type cards */
#ifdef DEBUG
                printf("Color pixmap is at %d, now running break).\n",pixmap_index);
#endif
                break;          /* All out of colors */
            }

            for (shade = SHADES-1; shade >= 0; shade--) {
                colors[ shade].red   = color->r * 65535 / 255;
                colors[ shade].green = color->g * 65535 / 255;
                colors[ shade].blue  = color->b * 65535 / 255;
                colors[ shade].pixel = cells[ shade];

                color = color_list__get_prev(color_list);
            }

            if (shade == -1) {
                XStoreColors( display, colormap, colors, SHADES);
            } else {
                /* Problem with color list */
                pixmap_index--;
                XFreeColors( display, colormap, cells, SHADES, /*nplanes=*/0);
                break;
            }

            static_data->pixmaps[pixmap_index] =
                XCreatePixmap(display, window,
                              static_data->width,static_data->height,
                              DefaultDepth(display, DefaultScreen(display)));

            /* Draw the item onto the pixmap */
            draw_item(static_data, display, clear_gc, colors,
                      static_data->pixmaps[ pixmap_index], background);
        }

#ifdef DEBUG
        printf("Created %d different colored balls (%d possible).\n",
                pixmap_index, color_list__get_count(color_list) / 4);
#endif

        color_list__destroy( color_list);

        static_data->num_colors = pixmap_index;
    }


    if ((static_data->num_colors == 0) ||
        (DisplayCells( display, DefaultScreen(display)) == 2)) {
        /* Monochrome system or can't allocate colors - just have 1 pixmap of a black ball */
#ifdef DEBUG
        printf("Created monochrome ball.\n");
#endif
        draw_black_ball( display, background);
    }


    XFreeGC( display, clear_gc);

    XSetFunction( display, static_data->gc, GXxor);

    return static_data;
}




/* Draw an item in the passed colors on the passed pixmap */
static void draw_item( static_data, display, clear_gc, colors, pixmap,
                       background)
item_static_type static_data;
Display         *display;
GC               clear_gc;
XColor          *colors;
Pixmap           pixmap;
Pixel            background;
{
    static struct {
        double x_offset, y_offset; /* The circle's offset on the item  */
        double width, height;      /* The width & height of the circle */
    } offsets[SHADES-1] = {
        0.2, 0.2, 0.3, 0.3,
        0.1, 0.1, 0.5, 0.5,
        0.0, 0.0, 0.8, 0.8
    };
    int x;


    /* Clear pixmap */
    XFillRectangle(display, pixmap, clear_gc, 0, 0,
                   static_data->width,
                   static_data->height);

    XSetForeground(display, static_data->gc,
                    colors[SHADES-1].pixel ^ background);
    draw_circle(display, pixmap, static_data->gc,
                0, 0, static_data->width-1, static_data->height-1);

    /* Draw the circles in the different shades.  Shade SHADES-1 is darkest. */
    for (x = SHADES-2; x >= 0; x--) {
        XSetForeground(display, static_data->gc, colors[x].pixel ^ background);
        draw_ball( display, pixmap,
                   offsets[x].x_offset, offsets[x].y_offset,
                   offsets[x].width,    offsets[x].height);
    }

    /* Draw a white specularity smallest and offset */
    XSetForeground(display, static_data->gc,
                   WhitePixel(display, DefaultScreen(display)) ^ background);
    draw_ball( display, pixmap, 0.3, 0.3, 0.1, 0.1);

    /* Draw a little black shadow in the lower right corner */
    XSetForeground(display, static_data->gc,
                   BlackPixel(display, DefaultScreen(display)) ^ background);
    XDrawArc(display, pixmap, static_data->gc, 0, 0,
             static_data->width - 1, static_data->height - 1,
             300*64, 35*64);
}


/* Draw a circle outline and filling at the size and offet determined by
   the ball's size */
static void draw_ball( display, pixmap, x_offset, y_offset, width, height)
Display * display;
Pixmap    pixmap;
double    x_offset, y_offset;
double    width, height;
{
    draw_circle(display, pixmap, static_data->gc,
                (int)(static_data->width  * x_offset),
                (int)(static_data->height * y_offset),
                (int)(static_data->width  * width),
                (int)(static_data->height * height));
}


/* Draw a filled circle of the specified size and position */
static void draw_circle( display, pixmap, gc, x, y, width, height)
Display * display;
Pixmap    pixmap;
GC        gc;
int       x, y;
int       width, height;
{
    XDrawArc(display, pixmap, static_data->gc, x, y, width, height, 0, 360*64);
    XFillArc(display, pixmap, static_data->gc, x, y, width, height, 0, 360*64);
}


/* Draw a black ball onto the first pixmap */
static void draw_black_ball( display, background)
Display * display;
Pixel background;
{
    Pixmap stipple_bitmap =
        XCreateBitmapFromData( display,
                               RootWindow( display, DefaultScreen( display)),
                               stipple_bits, stipple_width, stipple_height);

    static_data->pixmaps[ 0] =
        XCreatePixmap(display,
                      RootWindow(display, DefaultScreen(display)),
                      static_data->width, static_data->height,
                      DefaultDepth(display, DefaultScreen(display)));

    /* Reset all pixels in the background so they will not have
       any effect when the pixmap is xor-copied */
    XSetBackground(display, static_data->gc, 0);
    XFillRectangle(display, static_data->pixmaps[0], static_data->gc, 0, 0,
                   static_data->width, static_data->height);

    XSetForeground(display, static_data->gc,
                   BlackPixel(display,DefaultScreen(display)) ^background);

    /* Draw item */
    draw_circle(display, static_data->pixmaps[0], static_data->gc, 0, 0,
                static_data->width-1, static_data->height-1);

    /* draw a grey specularity small and offset */
    XSetStipple( display, static_data->gc, stipple_bitmap);
    XSetFillStyle( display, static_data->gc, FillOpaqueStippled);

    draw_ball( display, static_data->pixmaps[0], 0.2, 0.2, 0.3, 0.3);

    XSetFillStyle( display, static_data->gc, FillSolid);


    /* draw a white specularity smallest and offset */
    XSetForeground(display, static_data->gc,
                   WhitePixel(display,DefaultScreen(display)) ^background);

    draw_ball( display, static_data->pixmaps[0], 0.3, 0.3, 0.1, 0.1);


    static_data->num_colors = 1;

    XFreePixmap( display, stipple_bitmap);
}


#ifdef DEBUG

item__print( item)
item_type item;
{
    printf("x,y_pos: %4d,%4d; x,y_vel: %4d,%4d, collide = %d\n",
         item__get_x( item), item__get_y( item),
         item__get_x_vel( item), item__get_y_vel( item),
         (int)static_data->ball_collide);
}

#endif
