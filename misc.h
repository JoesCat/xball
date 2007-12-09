/**********************************************************************
 * misc.h - misc function declarations
 *
 * Copyright 1993, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/
#ifndef __MISC_H__
#define __MISC_H__

#include <malloc.h>

char * get_string(/* char *str */);
char * my_strdup(/* char *str */);
int    rand_range(/* min, max*/);
void   convert_newlines(/* str*/);

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define SWAP(x,y,type) { type t; t = (x); (x) = (y); (y) = t; }

#undef MIN
#undef MAX
#define MIN(x,y) ((x) > (y) ? (y) : (x))
#define MAX(x,y) ((x) < (y) ? (y) : (x))

#ifndef EXIT_FAILURE
#define EXIT_FAILURE (int)1
#define EXIT_SUCCESS (int)0
#endif


#endif
