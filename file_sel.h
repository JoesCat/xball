/**********************************************************************
 * file_sel.h - Public interface to the file_sel object.
 * 
 * Copyright 1992, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

/**********************************************************************
 * File Sel Object
 *
 * The file object is allows you to request a file from the user.
 *
 * Public methods:
 *
 *   void file_sel__init(file_sel_type *file_sel, help_type *help)
 *     Initialise the file_sel object.
 *
 *   void file_sel__free( file_sel_type *file_sel)
 *     Free data associated with file_sel object
 *
 *   char *file_sel__display(file_sel_type *file_sel, char *title)
 *     Display the file selection box and return selected file
 *
 **********************************************************************/

#ifndef __FILE_SEL_H__
#define __FILE_SEL_H__

#include <X11/Intrinsic.h>

typedef struct {
    Widget dialog_w;
    Bool   searching;
    char * filename;

    Widget toplevel;
} file_sel_struct_type, *file_sel_type;


file_sel_type file_sel__create(/*toplevel*/);
void          file_sel__destroy(/*file_sel*/);
char         *file_sel__display(/*file_sel, title*/);

#endif /* __FILE_SEL_H__ */
