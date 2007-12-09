/**********************************************************************
 * xball_sys.h - xball system object declarations
 *
 * Copyright 1993, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

#ifndef __XBALL_SYS_H__
#define __XBALL_SYS_H__

#include <X11/Intrinsic.h>

#include "menu.h"
#include "demo.h"
#include "room.h"
#include "items.h"
#include "sim.h"
#include "scrollbar.h"
#include "file_sel.h"
#include "help.h"

typedef struct {
    Widget              form_w;
    Widget              pane_w;
    menu_type           menu;
    demo_type           demo;
    items_type          items;
    room_type           room;
    sim_type            sim;
    help_type           help;
    scrollbar_type      gravity_bar;
    scrollbar_type      elasticity_bar;
    file_sel_type       file_sel;
} xball_system_struct, *xball_system_type;

xball_system_type xball_system__create(/* w*/);
void              xball_system__destroy(/*xball_system*/);
void              xball_system__quit_mcb(/* w, datap_ptr, call_data, extra*/);

#endif
