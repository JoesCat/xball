/**********************************************************************
 * misc.c - Misc functions
 *
 * Copyright 1993, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

/* System Headers */
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "misc.h"

/* Given a string 'text\nmore text', allocates and returns the
   string before the newline. */
char *get_string(char *str) {
    char *tmp;
    char *newline = strchr(str,'\n');
    int len;

    if (newline == (char *)0)
        return (char *)0;

    len = newline - str;
    tmp = malloc(len + 1);
    if (tmp == NULL)
        return (char *)0;

    strncpy(tmp, str, len + 1);
    tmp[len] = '\0';

    return tmp;
}


/* Because some systems do not have strdup */
char *my_strdup(char *str) {
    char *ret_str = malloc(strlen(str) + 1);
    if (ret_str != NULL)
        strcpy(ret_str, str);
#ifdef DEBUG
    else
        printf("my_strdup failed\n");
#endif

    return ret_str;
}


/* Return a random value in the specified range */
int rand_range(int min, int max) {
    if (min == max)
        return min;
    else
        return random() % (max - min) + min;
}


/* Change '~' to \n in the passed string */
/* This silliness is due to fallback resources not handling \n in the 
   resource string */
void convert_newlines(char *str) {
    char *curr = strchr(str, '~');

    while (curr != 0) {
        *curr = '\n';
        curr = strchr(curr, '~');
    }
}
