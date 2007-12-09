/**********************************************************************
 * sim.c - definition of the sim object - controls the simulation
 *
 * Copyright 1993, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

/* Local headers */
#include "sim.h"
#include "misc.h"
#include "names.h"

/* System Headers */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

/* Macros */
#define INVALID_WPID 0
#ifndef DELAY
#define DELAY 0
#endif

/* Structures */
typedef struct {
    int delay;
} app_data,*app_data_ptr;
static app_data res_data;
static XtResource resources[] = {
    { "delay", "Delay", XtRInt, sizeof(int),
          XtOffset(app_data_ptr,delay), XtRImmediate, (caddr_t)DELAY}
};

/* Functions */
static Boolean background_processing(/*sim*/);
static int delay();
static int process_invisible_backwards(/*datap_ptr*/);


/* Public object methods */

/* Create the simulation object */
sim_type sim__create(w, menu, items, room, demo)
Widget     w;
menu_type  menu;
items_type items;
room_type  room;
demo_type  demo;
{
    sim_type sim = (sim_type)malloc( sizeof( sim_struct_type));

    sim->menu  = menu;
    sim->items = items;
    sim->room  = room;
    sim->demo  = demo;

    XtGetApplicationResources(w, (XtPointer)&res_data, 
                              resources, XtNumber(resources), 
                              (ArgList)NULL,(Cardinal)0);

    sim->step_simulation = False;
    sim->delay           = res_data.delay;

    sim->wpid = INVALID_WPID;

    return sim;
}


/* Destroys the simulation object */
void sim__destroy(sim)
sim_type sim;
{
    XtRemoveWorkProc(sim->wpid);

    free( sim);
}


/* Steps the simulation one iteration */
void sim__step_mcb(w, sim, call_data, extra)
Widget   w;
sim_type sim;
caddr_t  call_data;
char *   extra;
{
    background_processing( sim);
}


/* Halts the simulation */
void sim__halt_mcb(w, sim, call_data, extra)
Widget               w;
sim_type             sim;
menuCallbackStruct * call_data;
char               * extra;
{
    sim->step_simulation = call_data->set;

    if (!sim->step_simulation)
    {
        /* Stepping currently turned off */
        /* Add background processing to perform simulation in real-time */
        sim->wpid = XtAppAddWorkProc(XtWidgetToApplicationContext(w),
                                     background_processing, (char *)sim);

	/* The 'Step simulation' menu item is greyed since sim is running */
	menu__set_sensitivity( sim->menu, STEP_SIM, /*sensitive=*/False);
    }
    else
    {
        /* Stepping turned on - remove the work proc if it has been started */
        if (sim->wpid != INVALID_WPID)
            XtRemoveWorkProc(sim->wpid);

        sim->wpid = INVALID_WPID;

	/* The 'Step simulation' menu item is ungreyed since sim not running */
	menu__set_sensitivity( sim->menu, STEP_SIM, /*sensitive=*/True);
    }
}


/* Private object methods */

/* Performs one iteration of the demo and simulation */
static Boolean background_processing(sim)
sim_type sim;
{
    room_type room = sim->room;

    if (demo__running(sim->demo))
        demo__process( sim->demo);

    /* Delay */
    delay(); 

    items__move_items( sim->items, room);

    return /*remove work procedure=*/False;
}


/* Slow the simulation some amount */
/* Slow more if there are less balls, less if there are more balls */
static int delay(
)
{
  static int first_flag = 1;
  static struct timeval last = {0,0};
  struct timeval tv;
  struct timezone tz;
  const int STEP = (10 * 1000); //10mSec

  gettimeofday(&tv, &tz);

  if (first_flag) {
    first_flag = 0;
    last = tv;
  }

  // Compute required delay
  int delta = tv.tv_usec - last.tv_usec;
  if (tv.tv_sec != last.tv_sec) {
    delta += (tv.tv_sec - last.tv_sec) * 1000000;
  }

  if (delta < STEP) {
    usleep(STEP - delta);
  }
  last = tv;
}
