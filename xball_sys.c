/**********************************************************************
 * xball_sys.c - xball system object definition.
 *
 * Copyright 1993, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

/* System Headers */
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>

#ifdef MOTIF
#include <Xm/Form.h>
#include <Xm/Label.h>
#endif

#ifdef ATHENA
#include <X11/Xaw/Form.h>
#endif

/* Local headers */
#include "xball_sys.h"
#include "names.h"
#include "misc.h"


/* Creates the xball system.  This object holds together the other
   objects in the system. */
xball_system_type xball_system__create( w)
Widget w;
{
    xball_system_type xball_system;
    menu_type         menu;
    room_type         room;
    demo_type         demo;
    help_type         help;
    sim_type          sim;
    items_type        items;
    file_sel_type     file_sel;
    scrollbar_type    gravity_bar;
    scrollbar_type    elasticity_bar;
    Widget            form_widget;


    typedef struct {
        int gravity;
        int elasticity;
        Boolean ball_collide;
        Boolean perpetual;
        Boolean halt_sim;
    } app_data,*app_data_ptr;
    app_data res_data;
    static XtResource resources[] = {
    { "gravity", "Gravity", XtRInt, sizeof(int),
          XtOffset(app_data_ptr,gravity), XtRImmediate, (caddr_t)GRAVITY },
    { "elasticity", "Elasticity", XtRInt, sizeof(int),
          XtOffset(app_data_ptr,elasticity), XtRImmediate,(caddr_t)ELASTICITY },
    { "collide", "Collide", XtRBoolean, sizeof(Boolean),
	XtOffset(app_data_ptr,ball_collide), XtRString, "False"},
    { "perpetual", "Perpetual", XtRBoolean, sizeof(Boolean),
	XtOffset(app_data_ptr,perpetual), XtRString, "False"},
    { "haltSim", "HaltSim", XtRBoolean, sizeof(Boolean),
	XtOffset(app_data_ptr,halt_sim), XtRString, "False"}
    };


    XtGetApplicationResources(w, (XtPointer)&res_data, 
                              resources, XtNumber(resources), 
                              (ArgList)NULL,(Cardinal)0);

    xball_system = XtNew( xball_system_struct);


    /* Create the various objects in the system */

#ifdef MOTIF
    xball_system->form_w = form_widget = 
        XtCreateWidget("form", xmFormWidgetClass, w, NULL,0);
#endif

#ifdef ATHENA
    xball_system->form_w = form_widget = 
        XtVaCreateWidget( "formAW", formWidgetClass, w, NULL);
#endif

    xball_system->menu = menu = menu__create( form_widget);

    xball_system->elasticity_bar = elasticity_bar =
        scrollbar__create( form_widget, ELASTICITY_BAR);

    xball_system->gravity_bar = gravity_bar =
        scrollbar__create( form_widget, GRAVITY_BAR);

    item__set_perpetual( menu__get_toggle( menu, PERPETUAL));
    item__set_ball_collide( menu__get_toggle( menu, BALL_COLLIDE));

    xball_system->file_sel = file_sel = file_sel__create(w);

    xball_system->items = items = items__create();
    xball_system->room = room = room__create( form_widget, items, file_sel);

    xball_system->demo = demo = demo__create(w, menu, gravity_bar,
                                             elasticity_bar, file_sel,
                                             items, room);

    xball_system->sim = sim = sim__create(w, menu, items, room, demo);

    xball_system->help = help = help__create(w);


    XtManageChild( form_widget);


    /* Add the scrollbar callbacks and set their values */

    scrollbar__add_callback( xball_system->gravity_bar,
                             room__set_gravity_cb,
                             (XtPointer)xball_system->room);
    scrollbar__set_value( xball_system->gravity_bar, res_data.gravity);

    scrollbar__add_callback( xball_system->elasticity_bar,
                             item__set_elasticity_cb,
                             (XtPointer)NULL);
    scrollbar__set_value( xball_system->elasticity_bar, res_data.elasticity);


    /* Register available menu callbacks */

    menu__add_callback(menu, "runDemo",  demo__run_mcb,  demo);
    menu__add_callback(menu, "stopDemo", demo__stop_mcb, demo);

    menu__add_callback(menu, "clear",      room__clear_mcb,       room);
    menu__add_callback(menu, "randomize",  room__randomize_mcb,   room);
    menu__add_callback(menu, "loadBitmap", room__load_bitmap_mcb, room);

    menu__add_callback(menu, "quit",   xball_system__quit_mcb,   xball_system);

    menu__add_callback(menu, "perpetual",   item__perpetual_mcb, NULL);
    menu__add_callback(menu, "ballCollide", item__collide_mcb,   NULL);
    menu__add_callback(menu, "haltSim",     sim__halt_mcb,       sim);
    menu__add_callback(menu, "stepSim",     sim__step_mcb,       sim);

    /* Set the values of these menu toggle widgets */
    menu__set_toggle( menu, PERPETUAL,     res_data.perpetual);
    menu__set_toggle( menu, BALL_COLLIDE,  res_data.ball_collide);
    menu__set_toggle( menu, HALT_SIM,      res_data.halt_sim);


    menu__add_callback(menu, "about",     help__display_mcb, help);
    menu__add_callback(menu, "onMenus",   help__display_mcb, help);
    menu__add_callback(menu, "onScroll",  help__display_mcb, help);
    menu__add_callback(menu, "onButtons", help__display_mcb, help);
    menu__add_callback(menu, "onDemos",   help__display_mcb, help);
    menu__add_callback(menu, "onParams",  help__display_mcb, help);

    return xball_system;
}


/* Free the xball system object */
void xball_system__destroy(xball_system)
xball_system_type xball_system;
{
    file_sel__destroy( xball_system->file_sel);
    help__destroy( xball_system->help);
    demo__destroy( xball_system->demo);
    room__destroy( xball_system->room);
    sim__destroy( xball_system->sim);
    items__destroy( xball_system->items);
    scrollbar__destroy( xball_system->elasticity_bar);
    scrollbar__destroy( xball_system->gravity_bar);
    menu__destroy( xball_system->menu);

    XtDestroyWidget( xball_system->form_w);



    XtFree( (char *)xball_system);
}


/* Called if the user selects the quit menu item */
void xball_system__quit_mcb( w, xball_system, call_data, extra)
Widget            w;
xball_system_type xball_system;
caddr_t           call_data;
char *            extra;
{
    XtCloseDisplay(XtDisplay(xball_system->form_w));

    exit(EXIT_SUCCESS);
}
