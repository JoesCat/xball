/**********************************************************************
 * list.h - declares the list object
 *
 * Copyright 1993, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

#ifndef __LIST_H__
#define __LIST_H__

typedef struct {
    void      **data;
    int         count;
    int         max_count;
    int         curr_item;
} list_struct, *list_type;

#define LIST_INITIAL_SIZE 100

list_type       list__create();
void            list__destroy(/* list */);
void *          list__add_start(/* list, data_item */);
void *          list__add_end(/* list, data_item */);
void *          list__get_first(/* list */);
void *          list__get_next(/* list */);
void *          list__get_last(/* list */);
void *          list__get_prev(/* list */);
void *          list__remove_last(/* list */);
void *          list__remove_first(/* list */);
void            list__remove_all(/* list */);
void *          list__remove(/* list, index */);

/* Inline functions */
#define list__get_cell(list, index) ((list)->data[ index])
#define list__set_cell(list, index) ((list)->data[ index])
#define list__get_count(list) ((list)->count)


#endif
