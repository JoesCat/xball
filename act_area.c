/**********************************************************************
 * act_area.c - Creates an action area
 *
 * An action area is a form widget holding an array of button widgets.
 * The name of the action area for is <name of the parent widget>_action_area.
 *
 **********************************************************************/

/* Written by Dan Heller.  Copyright 1991, O'Reilly && Associates.
 * This program is freely distributable without licensing fees and
 * is provided without guarantee or warrantee expressed or implied.
 * This program is -not- in the public domain.
 */

#include <X11/Intrinsic.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <stdio.h>

#include "act_area.h"

#define TIGHTNESS 20

Widget
CreateActionArea(parent, actions, num_actions)
Widget          parent;
ActionAreaItem *actions;
Cardinal        num_actions;
{
    Widget action_area, widget;
    char name[50];
    int i;


    sprintf(name,"%s_action_area", XtName(parent));
    action_area = 
        XtVaCreateWidget(name, xmFormWidgetClass, parent,
                         XmNfractionBase, TIGHTNESS*num_actions - 1,
                         XmNmarginHeight, 10,
                         XmNmarginWidth,  10,
                         NULL);

    for (i = 0; i < num_actions; i++) {
        widget = XtVaCreateManagedWidget(actions[i].label,
            xmPushButtonWidgetClass, action_area,
            XmNleftAttachment,       i? XmATTACH_POSITION : XmATTACH_FORM,
            XmNleftPosition,         TIGHTNESS*i,
            XmNtopAttachment,        XmATTACH_FORM,
            XmNbottomAttachment,     XmATTACH_FORM,
            XmNrightAttachment,
                    i != num_actions-1? XmATTACH_POSITION : XmATTACH_FORM,
            XmNrightPosition,        TIGHTNESS*i + (TIGHTNESS-1),
            XmNshowAsDefault,        i == 0,
            XmNdefaultButtonShadowThickness, 1,
            NULL);
        if (actions[i].callback)
            XtAddCallback(widget, XmNactivateCallback,
                actions[i].callback, (XtPointer)actions[i].data);
        if (i == 0 ) {
            /* Set the action_area's default button to the first widget
             * created (or, make the index a parameter to the function
             * or have it be part of the data structure). Also, set the
             * pane window constraint for max and min heights so this
             * particular pane in the PanedWindow is not resizable.
             */
            XtVaSetValues(parent,
                XmNdefaultButton, widget,
                NULL);
        }
    }

    XtManageChild(action_area);

    return action_area;
}
