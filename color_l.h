/**********************************************************************
 * color_l.h - Color list object declaration
 *
 * Copyright 1993, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

#ifndef __COLOR_L_H__
#define __COLOR_L_H__

#include "list.h"


#define SHADES 4  /* Number of shades for colors in the rgb.txt file */


typedef struct {
    list_type shaded_4_list;
} color_list_struct, *color_list_type;

typedef struct {
    int    r,g,b;
    char * name;
    int    last_char;
} color_struct, *color_type;


color_list_type color_list__create(/*filename*/);
void            color_list__destroy(/*color_list*/);
color_type      color_list__remove(/* color_list, index*/);
int             color_list__get_count(/* color_list*/);
color_type      color_list__get_last(/* color_list*/);
color_type      color_list__get_prev(/* color_list*/);
#ifdef NOT_USED
char *          color__get_shade(/* color, shade_num*/);
#endif

/* Inline functions */
#define color_list__get_cell(list, index) \
    ((color_type)list__get_cell((list)->shaded_4_list, index))


#endif
