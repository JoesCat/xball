/**********************************************************************
 * file_sel.c - Object that asks the user for a filename.
 *
 * Copyright 1993, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

/**********************************************************************
 * File_sel Object
 *
 * Private Methods:
 *
 *   XtCallbackProc button_cb
 *     Callback if the user presses a button in the selection dialog.
 *  
 *
 * Private Data:
 *
 *   Bool searching - True while the dialog is up.
 *   char *filename - Filename the user selected.
 *   Widget dialog_w - the dialog widget.
 *
 **********************************************************************/

/* System headers */
#include <stdio.h>
#include <X11/Intrinsic.h>

#ifdef MOTIF
#include <Xm/FileSB.h>
#include <Xm/DialogS.h>
#endif

/* Local headers */
#include "file_sel.h"
#include "miscx.h"
#include "names.h"

/* Functions */
static void display_dialog(/* file_sel, title*/);


/* Create the file_sel object */
file_sel_type file_sel__create(toplevel)
Widget toplevel;
{
    file_sel_type file_sel = XtNew( file_sel_struct_type);

    file_sel->toplevel = toplevel;

    file_sel->searching = False;
    file_sel->filename = (char *)0;
    file_sel->dialog_w = (Widget)0;

    return file_sel;
}


/**********************************************************************
 * file_sel__free - Free data associated with file_sel object
 *
 * void file_sel__free( file_sel_type file_sel)
 *
 *
 * file_sel - the file_sel object
 *
 **********************************************************************/
void file_sel__destroy(file_sel)
file_sel_type file_sel;
{
    if (file_sel->filename != (char *)0)
    {
        XtFree( file_sel->filename);
    }

    if (file_sel->dialog_w != (Widget)0)
    {
        XtDestroyWidget( file_sel->dialog_w);
    }

    XtFree( (char *)file_sel);
}


/**********************************************************************
 * file_sel__display - Display the file selection box and return selected file
 *
 * char *file_sel__display(file_sel_type file_sel, char *title)
 *
 *
 * file_sel - the file_sel object
 * title - the string used for the dialog's popup window title
 * Returns - the filename selected by the user, or (char *)0 if canceled
 *
 * Description:
 *   If the title is (char *)0, a default title will be used.
 *   the returned string is not to be freed by the caller.
 *   The method only returns when the user is done with the dialog.
 *
 **********************************************************************/
char *file_sel__display(file_sel, title)
file_sel_type file_sel;
char *        title;
{
    /* Free filename if it was set in previous calls */
    if (file_sel->filename != (char *)0)
    {
        XtFree(file_sel->filename);
        file_sel->filename = (char *)0;
    }

    /* Display dialog to user and set the filename member */
    display_dialog( file_sel, title);

    return file_sel->filename;
}


/* Private object methods */

#ifdef MOTIF

static void button_cb(/*w, file_sel, call_data*/);


static void display_dialog( file_sel, title)
file_sel_type file_sel;
char *        title;
{
    XEvent event;
    XtAppContext app_context;


    file_sel->searching = True;

    if (file_sel->dialog_w == (Widget)0)
    {
        file_sel->dialog_w = 
            XmCreateFileSelectionDialog(file_sel->toplevel, FILE_SB, NULL, 0);

        XtAddCallback(file_sel->dialog_w, XmNcancelCallback,
                      button_cb, (XtPointer)file_sel);
        XtAddCallback(file_sel->dialog_w,XmNokCallback,    
                      button_cb, (XtPointer)file_sel);
        XtAddCallback(file_sel->dialog_w,XmNhelpCallback,
                      button_cb, (XtPointer)file_sel);
    }

    app_context = XtWidgetToApplicationContext( file_sel->dialog_w);

    if (title == (char *)0)
      title = "Select File";

    XtVaSetValues( file_sel->dialog_w,
                  XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
                  XtVaTypedArg,
                    XmNdialogTitle, XmRString, title, sizeof(char *),
                  NULL);


    XmFileSelectionDoSearch( file_sel->dialog_w, NULL);
    XtManageChild( file_sel->dialog_w); /* Manage the file selection widget */


    /* Keep processing Xt events until one of the callbacks resets the 
       searching data member */
    while (file_sel->searching)
    {
        XtAppNextEvent(app_context, &event);
        XtDispatchEvent(&event);
    }
}


/**********************************************************************
 * button_cb - Callback if the user presses a button in the selection dialog.
 *
 * XtCallbackProc button_cb
 *
 * Description:
 *   Brings down the file selection dialog.
 *   If the OK button was pressed, the selected string is saved.
 *   If the Cancel button was pressed...
 **********************************************************************/
static void button_cb(w, file_sel, call_data)
Widget w;
file_sel_type file_sel;
XmFileSelectionBoxCallbackStruct *call_data;
{
    XtUnmanageChild(w);  /* Bring down the file selection dialog */

    switch (call_data->reason)
    {
        case XmCR_OK:
        /* Get the selected filename as a (char *) */
        if (!XmStringGetLtoR(call_data->value, XmSTRING_DEFAULT_CHARSET, 
                             &file_sel->filename))
            /* Internal error - couldn't get the selected filename */
            break;

        if (file_sel->filename[0] == '\0')
        {
            /* No string was selected */
            XtFree( file_sel->filename);
            file_sel->filename = (char *)0;
        }
        break;


        case XmCR_HELP:
        break;


        case XmCR_CANCEL:
        break;
    }

    file_sel->searching = False;     /* Exit file_sel__display() routine */
}

#endif


#ifdef ATHENA

/* Display the file dialog to the user.  
   Call SelFile in filemenu.c */
static void display_dialog( file_sel, title)
file_sel_type file_sel;
char *        title;
{
    SelFile( file_sel->toplevel, /*prompt=*/NULL, /*cancel=*/NULL, 
             /*init_path=*/NULL, /*show_entry=*/NULL, 
             &file_sel->filename);
}

#endif
