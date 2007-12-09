/**********************************************************************
 * miscx.c - Misc X, Xt and Xmu related functions
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
#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>
#include <X11/Xmu/Converters.h>


/**********************************************************************
 * setupCoverters - Setup user-defined Xt type converters.
 *
 * void setupConverters()
 *
 * Description:
 *   Sets up string to widget converter.
 **********************************************************************/
void setupConverters()
{
        static XtConvertArgRec parentCvtArgs[] = {
                {XtBaseOffset, (caddr_t)XtOffset(Widget, core.parent),
                        sizeof(Widget)}
        };

        XtAddConverter(XtRString, XtRWindow, XmuCvtStringToWidget,
                parentCvtArgs, XtNumber(parentCvtArgs));
}


/**********************************************************************
 * get_toplevel - Return the toplevel widget in the widget heirarchy.
 *
 * Widget get_toplevel(Widget w)
 *
 *
 * w - Any widget in the heirarchy
 *
 **********************************************************************/
Widget get_toplevel(w)
Widget w;
{
    /* Get the toplevel shell widget to change the cursor for all windows */
    while (!XtIsTopLevelShell( w))
        w = XtParent( w);

    return w;
}
