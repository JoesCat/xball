/**********************************************************************
 * file - description
 *
 * Copyright 1993, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

#ifndef __HELP_H__
#define __HELP_H__

#include <X11/Intrinsic.h>

typedef struct {
    Widget toplevel;
    Widget dialog_w;
    Widget text_w;
} help_struct_type, *help_type;


/* Public methods */
help_type help__create(/*w*/);
void      help__destroy(/*help*/);
void      help__display_mcb(/*w, help, call_data, extra*/);


#endif


