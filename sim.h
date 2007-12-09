/**********************************************************************
 * sim.h - simulation object declarations
 *
 * Copyright 1993, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

#ifndef __SIM_H__
#define __SIM_H__

#include <X11/Intrinsic.h>

#include "items.h"
#include "room.h"
#include "demo.h"
#include "menu.h"

typedef struct {
    menu_type    menu;
    items_type   items;
    room_type    room;
    demo_type    demo;

    XtWorkProcId wpid;             /* Work process id */
    int          delay;            /* Amount of delay we should do */
    Bool         step_simulation;  /* True if we are steeping the simulation */
} sim_struct_type, *sim_type;


sim_type sim__create(/*w, items, room*/);
void     sim__destroy(/*sim*/);
void     sim__step_mcb(/*w, sim, call_data, extra*/);
void     sim__halt_mcb(/*w, sim, call_data, extra*/);


#endif

