/**********************************************************************
 * help.c - help object definition
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

#ifdef MOTIF
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Text.h>
#include <Xm/PushB.h>
#include <Xm/Separator.h>

typedef struct {
    char *label;                /* Button widget's name */
    void (*callback)();         /* Button widget's callback */
    caddr_t data;               /* Callback data */
} HelpButtonItem;

#endif


#ifdef ATHENA
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Command.h>
#endif

/* Local headers */
#include "help.h"
#include "names.h"
#include "misc.h"
#include "miscx.h"

/* File scope variables */
typedef struct {
  char *menu_help;
  char *scrollbar_help;
  char *pointer_help;
  char *demo_help;
  char *startup_help;
  char *about_help;
} app_data,*app_data_ptr;
static app_data res_data = { 0, 0, 0, 0, 0, 0 };
static XtResource resources[] = {
  { "menuHelp", "MenuHelp", XtRString, sizeof(char *),
        XtOffset( app_data_ptr, menu_help), XtRString, NULL},
  { "scrollbarHelp", "ScrollbarHelp", XtRString, sizeof(char *),
        XtOffset( app_data_ptr, scrollbar_help), XtRString, NULL},
  { "pointerHelp", "PointerHelp", XtRString, sizeof(char *),
        XtOffset( app_data_ptr, pointer_help), XtRString, NULL},
  { "demoHelp", "DemoHelp", XtRString, sizeof(char *),
        XtOffset( app_data_ptr, demo_help), XtRString, NULL},
  { "startupHelp", "StartupHelp", XtRString, sizeof(char *),
        XtOffset( app_data_ptr, startup_help), XtRString, NULL},
  { "aboutString", "AboutString", XtRString, sizeof(char *),
        XtOffset( app_data_ptr, about_help), XtRString, NULL}
};

/* Functions */
static void close_cb(/*w, help, call_data*/);
static void display_help_dialog(/*help, message*/);
static void create_help_dialog(/* help*/);
static void set_help_text(/* help, message*/);


/* Public object methods */

/* Create the help object */
help_type help__create(w)
Widget w;
{
    help_type help = XtNew( help_struct_type);

    help->toplevel = w;

    help->dialog_w = (Widget)0;

    return help;
}


/* Destroy the help object */
void help__destroy(help)
help_type help;
{
    XtDestroyWidget( help->dialog_w);

    XtFree( (char *)help);
}


/* A menu callback (mcb) that displayshelp to the user, based on the
   name of the menu button used to invoke the callback */
void help__display_mcb(w, help, call_data, extra)
Widget    w;
help_type help;
XtPointer call_data;
char *    extra;
{
    char * help_text;
    char * widget_name = XtName(w);


    if (res_data.menu_help == 0)
    {
        /* Read help text resources */
        XtGetApplicationResources(get_toplevel(w), (XtPointer)&res_data, 
                                  resources, XtNumber(resources), 
                                  (ArgList)NULL,(Cardinal)0);

	/* We have to convert the ~ to newlines because fallback
	   resources do not work if they have newlines embedded in them */
	/* convert_newlines is in misc.c */
        convert_newlines( res_data.menu_help);
        convert_newlines( res_data.scrollbar_help);
        convert_newlines( res_data.pointer_help);
        convert_newlines( res_data.demo_help);
        convert_newlines( res_data.startup_help);
        convert_newlines( res_data.about_help);
    }

    if (strcmp( widget_name, ON_MENUS) == 0)
        help_text = res_data.menu_help;
    else
    if (strcmp( widget_name, ON_SCROLL) == 0)
        help_text = res_data.scrollbar_help;
    else
    if (strcmp( widget_name, ON_BUTTONS) == 0)
        help_text = res_data.pointer_help;
    else
    if (strcmp( widget_name, ON_DEMOS) == 0)
        help_text = res_data.demo_help;
    else
    if (strcmp( widget_name, ON_PARAMS) == 0)
        help_text = res_data.startup_help;
    else
    if (strcmp( widget_name, ABOUT) == 0)
        help_text = res_data.about_help;

    /* Display the help dialog */
    display_help_dialog(help, help_text);
}


/* Private object methods */

/* Called when the user wants to close the help dialog */
static void close_cb(w, help, call_data)
Widget          w;
help_type       help;
XtPointer call_data;
{
    XtUnmanageChild( help->dialog_w);
}


/* Displays the passed message in the help dialog */
static void display_help_dialog(help, message)
help_type help;
char   *message;
{
    if (help->dialog_w == (Widget)0)
    {
        create_help_dialog( help);
    }

    set_help_text( help, message);

    XtManageChild( help->dialog_w);
}


#ifdef MOTIF

static Widget
create_help_button(parent, action)
Widget          parent;
HelpButtonItem *action;
{
    char txt[100];
    Widget button, widget;

    sprintf(txt,"%s_action_area", XtName(parent));
    button = 
      XtVaCreateWidget(txt, xmFormWidgetClass, parent,
		       XmNfractionBase, 0,
		       XmNmarginHeight, 10,
		       XmNmarginWidth,  10,
		       NULL);
    widget = XtVaCreateManagedWidget(action->label,
				     xmPushButtonWidgetClass, button,
				     XmNleftAttachment,       XmATTACH_FORM,
				     XmNleftPosition,         0,
				     XmNtopAttachment,        XmATTACH_FORM,
				     XmNbottomAttachment,     XmATTACH_FORM,
				     XmNrightAttachment,      XmATTACH_FORM,
				     XmNrightPosition,        19,
				     XmNshowAsDefault,        1,
				     XmNdefaultButtonShadowThickness, 1,
				     NULL);
      if (action->callback) XtAddCallback(widget,
					XmNactivateCallback,
					action->callback,
					(XtPointer)action->data);
      // Default button should be first widget
      XtVaSetValues(parent,
		    XmNdefaultButton, widget,
		    NULL);
      
    XtManageChild(button);

    return button;
}



/* Motif help dialog */
static void create_help_dialog(help)
help_type help;
{
    Widget sep;
    Arg    wargs[10];
    int    n;

    static HelpButtonItem action_item = {
      "close", close_cb,  NULL
    };

    help->dialog_w = 
        XmCreateFormDialog( help->toplevel, HELP_DIALOG, NULL, 0);


    /* Create the action area */
    action_item.data = (XtPointer )help;
    create_help_button( help->dialog_w, 
                      &action_item);


    sep = XtVaCreateManagedWidget("separator", xmSeparatorWidgetClass,
                            help->dialog_w, 
                            NULL);

    n = 0;
    XtSetArg(wargs[n], XmNeditMode,           XmMULTI_LINE_EDIT); n++;
    XtSetArg(wargs[n], XmNeditable,           False); n++;
    XtSetArg(wargs[n], XmNverifyBell,         False); n++;
    XtSetArg(wargs[n], XmNtopAttachment,      XmATTACH_FORM); n++;
    XtSetArg(wargs[n], XmNleftAttachment,     XmATTACH_FORM); n++;
    XtSetArg(wargs[n], XmNrightAttachment,    XmATTACH_FORM); n++;
    XtSetArg(wargs[n], XmNbottomAttachment,   XmATTACH_WIDGET); n++;
    XtSetArg(wargs[n], XmNbottomWidget,       sep); n++;

    help->text_w = 
        XmCreateScrolledText( help->dialog_w, "help_text", wargs, n);
    XtManageChild( help->text_w);
}


/* Sets the help text in the Motif dialog */
static void set_help_text( help, message)
help_type help;
char * message;
{
    XmTextSetString( help->text_w, message);
}

#endif


#ifdef ATHENA

/* Creates the Athena help dialog */
static void create_help_dialog( help)
help_type help;
{
    Widget      form;
    Widget      button;
    char        buff[50];
    Dimension   border_width;
    Dimension   top_width;
    Dimension   top_height;
    Dimension   dialog_width;
    Dimension   dialog_height;
    int         x_offset, y_offset;
    Position    x,y;



    sprintf(buff, "%s_popup", HELP_DIALOG);
    help->dialog_w = 
        XtVaCreateWidget( buff, transientShellWidgetClass, help->toplevel,
                          XtNtransientFor,      help->toplevel,
                          XtNmappedWhenManaged, False,
                          NULL);

    form = XtVaCreateManagedWidget( HELP_DIALOG, formWidgetClass, 
                                    help->dialog_w, NULL);

    help->text_w = 
        XtVaCreateManagedWidget( "help_textAW", asciiTextWidgetClass, form,
                                 NULL);

    button = 
        XtVaCreateManagedWidget( "close", commandWidgetClass, form,
                                 XtNfromVert,   help->text_w,
                                 NULL);
    XtAddCallback( button, XtNcallback, close_cb, (XtPointer)help);

    XtInstallAccelerators( form, button);

    /* Manage so the shell widget's width and height will be set */
    XtManageChild( help->dialog_w);

    /* Position centered over the toplevel window */
    XtVaGetValues( help->toplevel,
                   XtNwidth,       &top_width,
                   XtNheight,      &top_height,
                   XtNborderWidth, &border_width,
                   NULL);

    XtVaGetValues( help->dialog_w,
                   XtNwidth,  &dialog_width,
                   XtNheight, &dialog_height,
                   NULL);

    x_offset = (top_width  - dialog_width ) / 2;
    y_offset = (top_height - dialog_height) / 2;

    XtTranslateCoords(help->toplevel, x_offset, y_offset, &x, &y);

    XtUnmanageChild( help->dialog_w);

    XtVaSetValues( help->dialog_w,
                   XtNx,                 x,
                   XtNy,                 y,
                   XtNmappedWhenManaged, True,
                   NULL);
}


/* Set the help string in the Athena help dialog */
static void set_help_text( help, message)
help_type help;
char * message;
{
    XtVaSetValues( help->text_w,
                   XtNstring, message,
                   NULL);
}

#endif
