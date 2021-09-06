/**********************************************************************
 * menu.c - defines the menu object
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
#include <string.h>
#include <sys/types.h>
#include <ctype.h>      /* For isspace */

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>

#ifdef MOTIF
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/CascadeB.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/ToggleBG.h>
#endif

#ifdef ATHENA
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#endif

/* Local headers */
#include "menu.h"
#include "misc.h"
#include "table.h"
#include "miscx.h"

#ifdef ATHENA
#include "on.xbm"
#include "off.xbm"
#endif

/* Macros */
#define WPATH_LEN 40

/* File scope variables */
typedef struct {
  char *menu_def;
} app_data,*app_data_ptr;
static app_data res_data;
static XtResource resources[] = {
  { "menuDef", "MenuDef", XtRString, sizeof(char *),
        XtOffset( app_data_ptr, menu_def), XtRString, NULL}
};

/* Structures */
typedef struct {
    char *           name;
    menuCallbackProc proc;
    caddr_t          closure;
} menuCallback;

typedef struct {
    char * menu_callback_name;
    char * optional_params;
    char * widget_class;
} closure_type;

/* Functions */
static void   menu_callback(/* w, menu, call_data*/);
static void   create_menus(/*menu, menubar, menu_def*/);
static void   create_menubar(/* menu, form, name*/);
static void   set_widget_and_do_callback(/* menu, w, set*/);
static void   set_widget_state(/* menu, w, state*/);
static Widget get_menu_widget(/* menu, widget_name*/);
static Bool   get_widget_state(/* menu, w*/);
static Widget create_pulldown(/* menubar, menu_button_name, pull_down_name*/);
static WidgetClass * get_pushbutton_class(/**/);
static char *        get_pushbutton_callback(/**/);
static WidgetClass * get_separator_class(/**/);
static char *        get_separator_callback(/**/);
static WidgetClass * get_togglebutton_class(/**/);
static char *        get_togglebutton_callback(/**/);


/* Public object methods */

/* Create the menu object.  The menubar definition resource is loaded
   and used to create the widgets that make up the menu system.  */
menu_type menu__create( form)
Widget form;
{
    menu_type menu = XtNew( menu_struct_type);

    menu->mc_table = table__create();

    menu->closure_table = table__create();

    create_menubar( menu, form, "menuBar");

    XtGetApplicationResources(get_toplevel(form), (XtPointer)&res_data, 
                              resources, XtNumber(resources), 
                              (ArgList)NULL,(Cardinal)0);


    convert_newlines( res_data.menu_def);

    create_menus( menu, menu->menubar, res_data.menu_def);

    XtManageChild( menu->menubar);

    return menu;
}


/* Destroy the menu system object */
void menu__destroy(menu)
menu_type menu;
{
    table__destroy( menu->mc_table);
    table__destroy( menu->closure_table);
    XtDestroyWidget( menu->menubar);

    XtFree( (char *)menu);
}


/* Registers an available callback.  This callback can then be
   referenced by the menu definition resource.  If the callback is
   referenced, the menuCallbackProc is invoked. */
void menu__add_callback( menu, action_name, menu_callback_proc, user_data)
menu_type        menu;
char            *action_name;
menuCallbackProc menu_callback_proc;
caddr_t          user_data;
{
    /* Store available callback into table */
    menuCallback *menu_node = XtNew( menuCallback);

    menu_node->name    = my_strdup( action_name);
    menu_node->proc    = menu_callback_proc;
    menu_node->closure = user_data;

    table__store( menu->mc_table, menu_node->name, (void *)menu_node);
}


/* Given the menu item widget name, toggles the widget's state and
   performs any menu callbacks registered for the callback that the
   widget references */
void menu__set_toggle( menu, widget_name, set)
menu_type menu;
char *    widget_name;
Bool      set;
{
    Widget w = get_menu_widget( menu, widget_name);

    if (w == (Widget)0)
      return;

    set_widget_and_do_callback( menu, w, set);
}



/* Return the state of the named widget's toggle state */
Bool  menu__get_toggle( menu, widget_name)
menu_type        menu;
char            *widget_name;
{
    Widget w = get_menu_widget( menu, widget_name);


    if (w == (Widget)0)
      return False;

    return get_widget_state( menu, w);
}


/* Set or unset the sensitivity of the specified menu widget */
void menu__set_sensitivity( menu, widget_name, sensitive)
menu_type menu;
char *    widget_name;
Boolean   sensitive;
{
    Widget w = get_menu_widget( menu, widget_name);


    if (w == (Widget)0)
      return;

    XtVaSetValues( w, XtNsensitive, sensitive, NULL);
}


/* Returned the named menu widget, or NULL if not found */
static Widget get_menu_widget( menu, widget_name)
menu_type menu;
char *    widget_name;
{
    Widget w;
    char widget_path[50];


    sprintf(widget_path,"*%s", widget_name);
    w = XtNameToWidget( menu->menubar, widget_path);

    if (w == (Widget)0)
    {
        fprintf(stderr,"get_menu_widget: Cannot find menu widget '%s'\n", 
		widget_name);
    }

    return w;
}


/* menu_callback
   Called for any menu item that is selected.  Sets up the parameters
   and makes the callback to the proper menuCallbackProc function. */
static void menu_callback( w, menu, call_data)
Widget    w;
menu_type menu;
caddr_t * call_data;
{
    closure_type *     closure;
    menuCallback *     mc_node;
    menuCallbackStruct menu_cbs;


    /* Get data passed when callback was referenced in menu configuration */
    closure = (closure_type *)table__retrieve( menu->closure_table, XtName(w));
#ifdef DEBUG
    table__dump( menu->closure_table);
#endif


    if (closure == (closure_type *)0)
    {
        fprintf(stderr,"Cannot find menu callback closure for widget %s.\n",
                XtName(w));
        return;
    }

    if (strcmp( closure->widget_class, "ToggleButton") == 0)
    {
#ifdef ATHENA
        /* We have to toggle the athena widget by hand */
        set_widget_state( menu, w, !get_widget_state( menu, w));
#endif

        menu_cbs.set = get_widget_state( menu, w);
    }
    else
      /* Non-toggle button - just put any value there */
      menu_cbs.set = True;


    /* Get the callback information stored when the callback was registered */
    mc_node = (menuCallback *)table__retrieve( menu->mc_table, 
                                               closure->menu_callback_name);
#ifdef DEBUG
    table__dump( menu->mc_table);
#endif

    if (mc_node == (menuCallback *)0)
    {
        fprintf(stderr,"Cannot find menu callback '%s'\n", 
                closure->menu_callback_name);
        return;
    }

    /* Call the menu callback function */
    (*mc_node->proc)(w, mc_node->closure, &menu_cbs, closure->optional_params);
}


/**********************************************************************
 * Menu config text format:
 *
 *   XBall.menu: 
 *     pulldownName~
 *     widgetName,widgetClass,menuCallbackName,optionalParams...~
 *       .
 *       .
 *       .
 *     pulldownName2~
 *     widgetName,widgetClass,menuCallbackName,optionalParams...~
 *       .
 *       .
 *       .
 **********************************************************************/

static void create_menus(menu, menubar, menu_def)
menu_type menu;
Widget    menubar;
char *    menu_def;
{
    Widget        pull_down;
    Widget        button;
    char          pull_down_name[30];
    char *        curr_pt = menu_def;
    char *        comma;
    char *        newline;
    char *        next_pt;
    WidgetClass * w_class_p;
    Arg           args[10];
    int           n;


    while (curr_pt != NULL)
    {
        /*next_pt = strpbrk( curr_pt, "\"{}");*/

        /* Search for first non-space */
        for (next_pt = curr_pt; 
             next_pt != '\0' && isspace( *next_pt); 
             next_pt++)
            /* Do nothing*/;

        if (*next_pt == '\0')
            break;

        switch (*next_pt)
        {
        case '{':
            printf("Sorry - '{' not yet implemented\n");
            curr_pt = strchr( next_pt, '\n'); /* Go to next line */
            break;
        case '}':
            printf("Sorry - '}' not yet implemented\n");
            curr_pt = strchr( next_pt, '\n'); /* Go to next line */
            break;
        default:
            comma = strchr( next_pt, ',');
            newline = strpbrk( next_pt, "\n ");
            /*newline = strchr( next_pt, '\n');*/
            if (newline == NULL || comma == NULL ||
                newline < comma)
            {
                /* At a menu header since no commas before the next newline */
                char *menu_button_name = get_string( next_pt);

                sprintf(pull_down_name, "%sPullDown", menu_button_name);

		pull_down = create_pulldown( menubar, menu_button_name, 
					     pull_down_name);

                free( menu_button_name);

                curr_pt = strchr( next_pt, '\n'); /* Go to next line */
            }
            else
            {
                /* At a menu item.  Get the parameters */
                char *item_str = get_string( next_pt);
                char *w_name;
                char *w_class;
                char *menu_callback_name;
                char *optional_params;
                char *callback;

                w_name  = strtok( item_str, ",");
                w_class = strtok( (char *)0, ",");
                menu_callback_name = strtok( (char *)0, ",");
                optional_params = strtok( (char *)0, "\n");

                n = 0;

                if (strcmp( w_class, "PushButton") == 0)
                {
		    w_class_p = get_pushbutton_class();
		    callback  = get_pushbutton_callback();
                }
                else
                if (strcmp( w_class, "Separator") == 0)
                {
		    w_class_p = get_separator_class();
		    callback  = get_separator_callback();
                }
                else
                if (strcmp( w_class, "ToggleButton") == 0)
                {
		    w_class_p = get_togglebutton_class();
		    callback  = get_togglebutton_callback();

#ifdef ATHENA
		    /* Need to initialize some values for the athena toggle */
                    XtSetArg(args[n], XtNleftBitmap, menu->off_bitmap); n++;
                    XtSetArg(args[n], XtNleftMargin, off_width+4); n++;
                    XtSetArg(args[n], XtNvertSpace, 50); n++;
#endif
                }

                button = XtCreateManagedWidget( w_name, *w_class_p, pull_down, 
                                                args, n);

                if (callback != (char *)0)
                {
                    closure_type * closure;

                    if (optional_params == (char *)0)
                        optional_params = "";

                    closure = (closure_type *)XtNew( closure_type);
                    closure->menu_callback_name= my_strdup(menu_callback_name);
                    closure->optional_params = my_strdup( optional_params);
                    closure->widget_class = my_strdup( w_class);

                    XtAddCallback( button, callback,menu_callback,
                                   (XtPointer)menu);

                    table__store( menu->closure_table, w_name,(void *)closure);
                }
                free( item_str);

                curr_pt = strchr( next_pt, '\n'); /* Go to next line */
            }
            break;
        }
    }
}


/* Private object methods */

#ifdef MOTIF

static void create_menubar( menu, form, name)
menu_type menu;
Widget    form;
char *    name;
{
    menu->menubar = XmCreateMenuBar( form, name, NULL, 0);
}


/* Sets the toggle widget to the specified value and performs menu
   callbacks to inform of the new value */
static void set_widget_and_do_callback( menu, w, set)
menu_type menu;
Widget    w;
Bool      set;
{
    set_widget_state( menu, w, set);

    XtCallCallbacks( w, XmNvalueChangedCallback, (char *)0);
}


/* Ser the state of the Motif menu "toggle" widget */
static void set_widget_state( menu, w, state)
menu_type menu;
Widget    w;
Bool      state;
{
    XtVaSetValues( w, XmNset, state, NULL);
}


/* Returns the state of the Motif menu widget */
static Bool get_widget_state( menu, w)
menu_type menu;
Widget    w;
{
    return XmToggleButtonGadgetGetState( w);
}


/* Create the pull down menu for Motif.  If the menu_button is named
   "help", it is designated the menu help button. */
static Widget create_pulldown( menubar, menu_button_name, pull_down_name)
Widget menubar;
char * menu_button_name;
char * pull_down_name;
{
    Widget pull_down;
    Widget button;


    pull_down = XmCreatePulldownMenu( menubar, pull_down_name, NULL, 0);

    button = 
        XtVaCreateManagedWidget( menu_button_name, xmCascadeButtonWidgetClass, 
				 menubar, 
				 XmNsubMenuId, pull_down,
				 NULL);

    if (strcmp( menu_button_name, "help") == 0)
    {
        XtVaSetValues(menubar, XmNmenuHelpWidget, button, NULL);
    }

    return pull_down;
}


static WidgetClass * get_pushbutton_class()
{
    return &xmPushButtonGadgetClass;
}


static char * get_pushbutton_callback()
{
    return XmNactivateCallback;
}


static WidgetClass * get_separator_class()
{
    return &xmSeparatorGadgetClass;
}


static char * get_separator_callback()
{
    return (char *)0;
}


static WidgetClass * get_togglebutton_class()
{
    return &xmToggleButtonGadgetClass;
}


static char * get_togglebutton_callback()
{
    return XmNvalueChangedCallback;
}


#endif


#ifdef ATHENA

menu_type global_action_menu; /* We need to have a global menu object,
				 because actions do not pass client_data */

static void menu_action(/* w, event, params, num_params*/);

static void create_menubar( menu, form, name)
menu_type menu;
Widget    form;
char *    name;
{
    XtActionsRec  action;


    menu->menubar = 
        XtVaCreateManagedWidget( name, boxWidgetClass, form,
                                 XtNorientation, XtorientHorizontal,
                                 XtNresizable,   True,
                                 NULL);

    menu->off_bitmap = 
        XCreateBitmapFromData( XtDisplay( form),
                               RootWindowOfScreen( XtScreen( form)),
                               off_bits, off_width, off_height);
    menu->on_bitmap = 
        XCreateBitmapFromData( XtDisplay( form),
                               RootWindowOfScreen( XtScreen( form)),
                               on_bits, on_width, on_height);


    global_action_menu = menu;

    /* Add a global action for menu actions */
    /* Athena needs global actions to implement "menu accelerators" */
    action.string = "menu_action";
    action.proc   =  menu_action;
    XtAppAddActions( XtWidgetToApplicationContext( menu->menubar),
                     &action, (Cardinal)1);
}


/* Sets the toggle widget to the specified value and performs menu
   callbacks to inform of the new value */
static void set_widget_and_do_callback( menu, w, set)
menu_type menu;
Widget    w;
Bool      set;
{
    /* We set the toggle to the opposite of what we finally want,
       because of the way the Athena menu callback works with toggles is that
       it toggles the value for each callback. */

    set_widget_state( menu, w, !set);

    XtCallCallbacks( w, XtNcallback, (char *)0);
}


/* Set the state of the athena menu "toggle" widget */
static void set_widget_state( menu, w, state)
menu_type menu;
Widget    w;
Bool      state;
{
    XtVaSetValues(w, XtNleftBitmap, state ? menu->on_bitmap : menu->off_bitmap,
                  NULL);
}


/* Returns the state of the Athena menu "toggle" widget */
static Bool get_widget_state( menu, w)
menu_type menu;
Widget    w;
{
    Pixmap pixmap_set;


    XtVaGetValues( w, XtNleftBitmap, &pixmap_set, NULL);

    return (pixmap_set == menu->on_bitmap);
}


/* Create the Athena pulldown widget */
static Widget create_pulldown( menubar, menu_button_name, pull_down_name)
Widget menubar;
char * menu_button_name;
char * pull_down_name;
{
    Widget button;
    Widget pull_down;


    button = XtVaCreateManagedWidget( menu_button_name, menuButtonWidgetClass, 
				      menubar, NULL);

    pull_down = XtCreatePopupShell( "menu", simpleMenuWidgetClass, button, 
				    NULL, 0);

    return pull_down;
}


static WidgetClass * get_pushbutton_class()
{
    return &smeBSBObjectClass;
}


static char * get_pushbutton_callback()
{
    return XtNcallback;
}


static WidgetClass * get_separator_class()
{
    return &smeLineObjectClass;
}


static char * get_separator_callback()
{
    return (char *)0;
}


static WidgetClass * get_togglebutton_class()
{
    return &smeBSBObjectClass;
}


static char *get_togglebutton_callback()
{
    return XtNcallback;
}


/* This global action is used to implement keyboard accelerators in
   Athena.  To get a keyboard accelerator, a widget sets it translations
   to call the global "menu_callback" action, passing the menu action
   they want to perform. */
static void menu_action( w, event, params, num_params)
Widget   w;
XEvent * event;
String * params;
Cardinal num_params;
{
    char buff[50];
    menu_type menu = global_action_menu;


    /* Get the action name passed to the menu_action */
    if (num_params < 1)
    {
        fprintf(stderr,
                "No menu action name passed to the menu_action action\n");
        return;
    }


    /* Type of menu action is passed to the action */
    w = get_menu_widget( menu, params[0]);

    if (w == (Widget)0)
      return;

    menu_callback( w, menu, NULL);
}

#endif
