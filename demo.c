/**********************************************************************
 * demo.c - defines the demo object
 *
 * Copyright 1993, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

/* System Headers */
#include <stdio.h>
#include <unistd.h>
#include <X11/StringDefs.h>

#ifdef MOTIF
#include <Xm/Xm.h>
#endif

/* Local headers */
#include "demo.h"
#include "names.h"
#include "intf.h"
#include "miscx.h"

/* Structures */
typedef struct {
  char *demo_file;
} app_data,*app_data_ptr;
static app_data res_data;
static XtResource resources[] = {
  { "demoFile", "DemoFile", XtRString, sizeof(char *),
        XtOffset( app_data_ptr, demo_file), XtRString, NULL}
};

/* Functions */
static void demo_start(/* demo*/);


/* Public object methods */

/* Create the demo object */
demo_type demo__create( w, menu, gravity, elasticity, file_sel, items, room)
Widget          w;
menu_type       menu;
scrollbar_type  gravity;
scrollbar_type  elasticity;
file_sel_type   file_sel;
items_type      items;
room_type       room;
{
    demo_type demo = XtNew( demo_struct_type);

    demo->menu = menu;
    demo->gravity = gravity;
    demo->elasticity = elasticity;
    demo->file_sel = file_sel;
    demo->items = items;
    demo->room = room;

    XtGetApplicationResources(get_toplevel(w), (XtPointer)&res_data, 
                              resources, XtNumber(resources), 
                              (ArgList)NULL,(Cardinal)0);

    if (res_data.demo_file != 0)
    {
        demo->file_name  = XtNewString( res_data.demo_file);
        demo_start( demo);
        demo->interactive = False;
    }
    else
    {
        demo->file_name = (char *)0;
        demo->file_ptr   = (FILE *)0;
        demo->iterations = 0;
        demo->interactive = True;

        /* The 'Stop Demo' menu item is greyed since demo is not running */
        menu__set_sensitivity( demo->menu, STOP_DEMO, False);
    }

    return demo;
}


/* Destroy the demo object */
void demo__destroy( demo)
demo_type demo;
{
    demo__stop( demo);
    XtFree( (char *)demo);
}


/* A menu callback (mcb) which queries the user for a filename, and then */
/* loads and runs the selected demo.                                     */
void demo__run_mcb( w, demo, call_data, extra)
Widget    w;
demo_type demo;
XtPointer call_data;
char *    extra;
{
    char *file_name = file_sel__display( demo->file_sel, "Load Demo");
   
    if (file_name != (char *)0)
    {
        demo__stop(demo);        /* Stop a demo if one is in progress */

        demo->file_name = XtNewString(file_name);

        demo_start( demo);
    }
}


/* A menu callback (mcb) which stops the currently running demo */
void demo__stop_mcb( w, demo, call_data, extra)
Widget    w;
demo_type demo;
XtPointer call_data;
char *    extra;
{
    demo__stop( demo);
}


/* If a demo is running, stops it by reseting the menu sensitivity,
   and cleaning up the demo file */
void demo__stop( demo)
demo_type demo;
{
    if (!demo__running(demo))
        return;

    /* The 'Stop Demo' menu item is greyed since menu is not running */
    menu__set_sensitivity( demo->menu, STOP_DEMO, False);

    /* Set the file_name member to NULL to tell background_processing() that
     * there is not a demo in progress */
    if (demo->file_name != (char *)0)
    {
        XtFree( demo->file_name);
        demo->file_name = (char *)0;
    }

    /* Close the demo file */
    if (demo->file_ptr != (FILE *)0)
    {
        fclose(demo->file_ptr);
        demo->file_ptr = (FILE *)0;
    }
}


    
/* Returns True if a demo is currently running */
Bool demo__running(demo)
demo_type demo;
{
    return demo->file_name != (char *)0;
}



/*
  demo language: (have 'quit demo' button)
    create %d,%d,%d,%d: create a ball with x,y, x & y velocity
    randomize: randomize velocities of all balls on screen
    load_bitmap %s %d,%d: load a bitmap to an x,y on the screen
    set_velocity %s %d,%d: set ball velocities (rel or abs)
    gravity %d: set gravity to something
    elasticity %d: set elasticity to something
    iterate %d: pause for n iterations
    perpetual on|off: turn perpetual balls on or off
    collide on|off: turn ball collision on or off
    item_width %d: change item width
    item_height %d: change item height
    window_width %d: Change window width
    window_height %d: Change window height
    clear: clear screen
    create_random %d,%d: create count random balls with a velocity_factor
    label %s:  A label
    goto %s: Goto label
    exit: quit the xball program
    quit: quit the xball program
    stop: stop demo but leave program alive
*/
#define MAX_LEN 100
#define is_command(comm) (strncmp(comm,in_line,strlen(comm)) == 0)


/* Performs one iteration of the currently running demo */
void demo__process(demo)
demo_type demo;
{
    char in_line[MAX_LEN];       /* The current demo file line read in */

    /* Used to extract parameters from in_line */
    char str[MAX_LEN];     
    int  x,y;
    int  x_vel,y_vel;
    int  width,height;
    int  count;
    int  velocity;
    item_type item;


    /* Do not run the demo until the item static data has been initialized */
    if (!item__initted())
        return;

    if (demo->iterations > 0)
    {
        /* Pause for another iteration */
        demo->iterations--;
        return;
    }


    /* Read the next line of the demo */
    while (fgets(in_line,MAX_LEN,demo->file_ptr) != (char *)NULL)
    {
        if (in_line[0] == '#' || in_line[0] == '\n')
        {
            /* Comment */
            continue;
        }
        else 
        if (is_command("create_random"))
        {
            /* create_random %d1,%d2 */
            /* create %d1 random balls with a max velocity of %d2 */
            
            sscanf(in_line,"create_random %d,%d", &count, &velocity);
            for (x = 0; x < count; x++)
            {
                items__new_item(demo->items, 
                    rand_range(intf2int(room__get_left_wall( demo->room)),
                               intf2int(room__get_right_wall( demo->room))),
                    rand_range(intf2int(room__get_ceiling( demo->room)),
                               intf2int(room__get_floor( demo->room))),
                                rand_range( -velocity, velocity),
                                rand_range( -velocity, velocity));
            }
        }
        else    
        if (is_command("create"))
        {
            /* create %d1,%d2,%d3,%d4 */
            /* Create a ball with x,y position %d1,%d2 and 
               an x & y velocity of %d2,%d4 */

            sscanf(in_line,"create %d,%d,%d,%d\n", &x,&y, &x_vel,&y_vel);

            items__new_item(demo->items, x, y, x_vel, y_vel);
        }
        else 
        if (is_command("randomize"))
        {
            int min_x,max_x,min_y,max_y;


            /* randomize %d,%d,%d,%d */
            /* randomize velocities of all balls on screen 
               with a max velocity of %d */

            min_x = max_x = min_y = max_y = 0;
            sscanf(in_line,"randomize %d,%d,%d,%d", &min_x,&max_x,&min_y,&max_y);

            for (item = items__get_first( demo->items);
                 item != (item_type)0;
                 item = items__get_next( demo->items))
            {
                item__randomize( item, min_x, max_x, min_y, max_y);
            }
        }
        else 
        if (is_command("load_bitmap"))
        {
            /* load_bitmap %s %d1,%d2 */
            /* load a bitmap file (where %s is the file path) to an 
               x,y position of %d1,%d2 on the screen */

            sscanf(in_line,"load_bitmap %s %d,%d", str,&x,&y);
            room__load_bitmap(demo->room, str, x, y);
        }
        else 
        if (is_command("set_velocity"))
        {
            /* set_velocity %s %d1,%d2 */
            /* set ball velocities (%s is relative or absolute) to the x
               and y velocities %d1,%d2 */

            sscanf(in_line,"set_velocity %s %d,%d", str,&x_vel,&y_vel);

            if (strncmp(str,"rel",3) == 0)
            {
                for (item = items__get_first( demo->items);
                     item != (item_type)0;
                     item = items__get_next( demo->items))
                {
                    item__set_x_vel( item, item__get_x_vel(item) + x_vel);
                    item__set_y_vel( item, item__get_y_vel(item) + y_vel);
                }
            }
            else if (strncmp(str,"abs",3) == 0)
            {
                for (item = items__get_first( demo->items);
                     item != (item_type)0;
                     item = items__get_next( demo->items))
                {
                    item__set_x_vel( item, x_vel);
                    item__set_y_vel( item, y_vel);
                }
                room__flush( demo->room);
            }
            else
                fprintf(stderr, 
                        "Unrecognized set_velocity mode '%s'.\nValid modes: 'abs' or 'rel'.\n", str);
        }
        else 
        if (is_command("gravity"))
        {
            /* gravity %d */
            /* set gravity to %d */
            
            int gravity_value;

            sscanf(in_line,"gravity %d", &gravity_value);

            scrollbar__set_value( demo->gravity, gravity_value);
        }
        else 
        if (is_command("elasticity"))
        {
            /* elasticity %d */
            /* set elasticity to %d */

            int elasticity_value;

            sscanf(in_line,"elasticity %d", &elasticity_value);
            
            scrollbar__set_value( demo->elasticity, elasticity_value);
        }
        else 
        if (is_command("iterate"))
        {
            /* iterate %d */
            /* pause for %d iterations */

            sscanf(in_line,"iterate %d", &demo->iterations);

            if (demo->iterations)
                /* We are pausing - exit read loop */
                return;
        }
        else    
        if (is_command("perpetual"))
        {
            /* perpetual %s */  
            /* toggle perpetual (%s is 'on' or 'off') */

            sscanf(in_line,"%*s %s", str);

            menu__set_toggle( demo->menu, PERPETUAL, strcmp("on",str) == 0);
        }
        else 
        if (is_command("collide"))
        {
            /* collide %s */
            /* toggle ball collision (%s is 'on' or 'off') */

            sscanf(in_line,"%*s %s", str);

            menu__set_toggle( demo->menu, BALL_COLLIDE, strcmp("on",str) == 0);
        }
        else
#if 0
        if (is_command("item_size"))
        {
            /* item_width %d */
            /* change item size to %d,%d */

            sscanf(in_line,"item_size %d,%d", &width, &height);

            items__set_visible( demo->items, False);

            item__set_size( width, height);

            items__set_visible( demo->items, True);
        }
        else 
#endif
        if (is_command("window_width"))
        {
            /* window_width %d */
            /* Change window width to %d */

            sscanf(in_line,"window_width %d", &width);

            room__set_width( demo->room, width);
        }
        else 
        if (is_command("window_height"))
        {
            /* window_height %d */
            /* Change window height to %d */

            sscanf(in_line,"window_height %d", &height);

            room__set_height( demo->room, (Dimension)height);
        }
        else 
        if (is_command("clear"))
        {
            /* clear */
            /* delete all balls in window */

            items__destroy_all( demo->items);
        }
#if 0
        else 
        if (is_command("goto") || is_command("label"))
        {
            /* goto %s */
            /* Goto label %s */

            /* label %s */
            /* The label %s */

            printf("Not implemented\n");
        }
#endif
        else 
        if (is_command("exit") || is_command("quit"))
        {
            /* exit */
            /* quit */
            /* quit the xball program */

            if (!demo->interactive)
            {
                demo__stop( demo);
                exit(0);
            }
        }
        else 
        if (is_command("stop"))
        {
            /* stop */
            /* stop demo but leave program alive */

            demo__stop( demo);
            return;
        }
        else
        if (is_command("backwards"))
        {
            sscanf(in_line,"backwards %d", &count);

            items__move_items_backwards( demo->items, demo->room, count);
        }
        else
        if (is_command("visible"))
        {
            sscanf(in_line,"visible %s", str);

            items__set_visible( demo->items, strcmp("on",str) == 0);
        }
        else
        if (is_command("sleep"))
        {
            sscanf(in_line,"sleep %u", &count);

            sleep( (unsigned)count);
        }
        else
        {
            /* Could not recognize demo command */
            in_line[strlen(in_line)-1] = '\0'; /* Kill newline */
            printf("Unrecognized command '%s'\n", in_line);
        }
    }
  
    if (feof(demo->file_ptr))
    {
        /* At the end of the demo file.  Stop processing the demo */
        demo__stop( demo);
    }
}


/* Private object methods */

/* Loads the currently specified demo program and sensitises the stop demo 
   menu  */
static void demo_start( demo)
demo_type demo;
{
    demo->iterations = 0;

    /* Start up the demo file */

    if ((demo->file_ptr = fopen(demo->file_name, "r")) == (FILE *)0)
    {
        /* Problem opening demo file */
        fprintf(stderr,"Opening demo file: ");
        perror( demo->file_name);


        XtFree( demo->file_name);
        demo->file_name = (char *)0;
        return;
    }
    else
    {
        menu__set_sensitivity( demo->menu, STOP_DEMO, True);
    }
}
